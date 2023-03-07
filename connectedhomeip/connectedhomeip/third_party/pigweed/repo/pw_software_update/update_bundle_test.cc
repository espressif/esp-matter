// Copyright 2021 The Pigweed Authors
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy of
// the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations under
// the License.

#include <array>

#include "gtest/gtest.h"
#include "pw_blob_store/blob_store.h"
#include "pw_kvs/fake_flash_memory.h"
#include "pw_kvs/test_key_value_store.h"
#include "pw_software_update/blob_store_openable_reader.h"
#include "pw_software_update/bundled_update_backend.h"
#include "pw_software_update/update_bundle_accessor.h"
#include "pw_stream/memory_stream.h"
#include "test_bundles.h"

#define ASSERT_OK(status) ASSERT_EQ(OkStatus(), status)
#define ASSERT_FAIL(status) ASSERT_NE(OkStatus(), status)

namespace pw::software_update {
namespace {

constexpr size_t kBufferSize = 256;
static constexpr size_t kFlashAlignment = 16;
constexpr size_t kSectorSize = 2048;
constexpr size_t kSectorCount = 2;
constexpr size_t kMetadataBufferSize =
    blob_store::BlobStore::BlobWriter::RequiredMetadataBufferSize(0);

class TestBundledUpdateBackend final : public BundledUpdateBackend {
 public:
  TestBundledUpdateBackend()
      : manifest_reader_({}), trusted_root_memory_reader_({}) {}

  Status ApplyReboot() override { return Status::Unimplemented(); }
  Status PostRebootFinalize() override { return OkStatus(); }

  Status ApplyTargetFile(std::string_view, stream::Reader&, size_t) override {
    return OkStatus();
  }

  Result<uint32_t> EnableBundleTransferHandler(std::string_view) override {
    return 0;
  }

  void DisableBundleTransferHandler() override {}

  void SetTrustedRoot(ConstByteSpan trusted_root) {
    trusted_root_memory_reader_ = stream::MemoryReader(trusted_root);
    trusted_root_reader_ = stream::IntervalReader(
        trusted_root_memory_reader_,
        0,
        trusted_root_memory_reader_.ConservativeReadLimit());
  }

  void SetCurrentManifest(ConstByteSpan current_manifest) {
    manifest_reader_ = stream::MemoryReader(current_manifest);
  }

  void SetManifestWriter(stream::Writer* writer) { manifest_writer_ = writer; }

  virtual Result<stream::SeekableReader*> GetRootMetadataReader() override {
    return &trusted_root_reader_;
  }

  Status BeforeManifestRead() override {
    before_manifest_read_called_ = true;
    if (manifest_reader_.ConservativeReadLimit() > 0) {
      return OkStatus();
    }
    return Status::NotFound();
  }

  bool BeforeManifestReadCalled() { return before_manifest_read_called_; }

  Result<stream::SeekableReader*> GetManifestReader() override {
    return &manifest_reader_;
  }

  Status BeforeManifestWrite() override {
    before_manifest_write_called_ = true;
    return (manifest_writer_) ? OkStatus() : Status::NotFound();
  }

  bool BeforeManifestWriteCalled() { return before_manifest_write_called_; }

  Status AfterManifestWrite() override {
    after_manifest_write_called_ = true;
    return OkStatus();
  }

  bool AfterManifestWriteCalled() { return after_manifest_write_called_; }

  Result<stream::Writer*> GetManifestWriter() override {
    return manifest_writer_;
  }

  virtual Status SafelyPersistRootMetadata(
      [[maybe_unused]] stream::IntervalReader root_metadata) override {
    new_root_persisted_ = true;
    trusted_root_reader_ = root_metadata;
    return OkStatus();
  }

  bool IsNewRootPersisted() const { return new_root_persisted_; }

 private:
  stream::IntervalReader trusted_root_reader_;
  stream::MemoryReader manifest_reader_;
  stream::Writer* manifest_writer_ = nullptr;
  bool before_manifest_read_called_ = false;
  bool before_manifest_write_called_ = false;
  bool after_manifest_write_called_ = false;
  bool new_root_persisted_ = false;

  // A memory reader for buffer passed by SetTrustedRoot(). This will be used
  // to back `trusted_root_reader_`
  stream::MemoryReader trusted_root_memory_reader_;
};

class UpdateBundleTest : public testing::Test {
 public:
  UpdateBundleTest()
      : blob_flash_(kFlashAlignment),
        blob_partition_(&blob_flash_),
        bundle_blob_("TestBundle",
                     blob_partition_,
                     nullptr,
                     kvs::TestKvs(),
                     kBufferSize),
        blob_reader_(bundle_blob_) {}

  blob_store::BlobStoreBuffer<kBufferSize>& bundle_blob() {
    return bundle_blob_;
  }

  BlobStoreOpenableReader& blob_reader() { return blob_reader_; }

  TestBundledUpdateBackend& backend() { return backend_; }

  void StageTestBundle(ConstByteSpan bundle_data) {
    ASSERT_OK(bundle_blob_.Init());
    blob_store::BlobStore::BlobWriter blob_writer(bundle_blob(),
                                                  metadata_buffer_);
    ASSERT_OK(blob_writer.Open());
    ASSERT_OK(blob_writer.Write(bundle_data));
    ASSERT_OK(blob_writer.Close());
  }

  // A helper to verify that all bundle operations are disallowed because
  // the bundle is not open or verified.
  void VerifyAllBundleOperationsDisallowed(
      UpdateBundleAccessor& update_bundle) {
    // We need to check specificially that failure is due to rejecting
    // unverified/unopen bundle, not anything else.
    ASSERT_EQ(update_bundle.GetManifest().status(),
              Status::FailedPrecondition());
    ASSERT_EQ(update_bundle.GetTargetPayload("any").status(),
              Status::FailedPrecondition());
    ASSERT_EQ(update_bundle.GetTargetPayload(protobuf::String({})).status(),
              Status::FailedPrecondition());
    ASSERT_EQ(update_bundle.PersistManifest(), Status::FailedPrecondition());
    ASSERT_EQ(update_bundle.GetTotalPayloadSize().status(),
              Status::FailedPrecondition());
  }

  // A helper to verify that UpdateBundleAccessor::OpenAndVerify() fails and
  // that all bundle operations are disallowed as a result. Also check whether
  // root metadata should be expected to be persisted.
  void CheckOpenAndVerifyFail(UpdateBundleAccessor& update_bundle,
                              bool expect_new_root_persisted) {
    ASSERT_FALSE(backend().IsNewRootPersisted());
    ASSERT_FAIL(update_bundle.OpenAndVerify());
    ASSERT_EQ(backend().IsNewRootPersisted(), expect_new_root_persisted);
    VerifyAllBundleOperationsDisallowed(update_bundle);

    ASSERT_OK(update_bundle.Close());
    VerifyAllBundleOperationsDisallowed(update_bundle);
  }

 private:
  kvs::FakeFlashMemoryBuffer<kSectorSize, kSectorCount> blob_flash_;
  kvs::FlashPartition blob_partition_;
  blob_store::BlobStoreBuffer<kBufferSize> bundle_blob_;
  BlobStoreOpenableReader blob_reader_;
  std::array<std::byte, kMetadataBufferSize> metadata_buffer_;
  TestBundledUpdateBackend backend_;
};

}  // namespace

TEST_F(UpdateBundleTest, GetTargetPayload) {
  backend().SetTrustedRoot(kDevSignedRoot);
  StageTestBundle(kTestDevBundle);
  UpdateBundleAccessor update_bundle(blob_reader(), backend());

  ASSERT_OK(update_bundle.OpenAndVerify());

  {
    stream::IntervalReader res = update_bundle.GetTargetPayload("file1");
    ASSERT_OK(res.status());

    const char kExpectedContent[] = "file 1 content";
    char read_buffer[sizeof(kExpectedContent) + 1] = {0};
    ASSERT_TRUE(res.Read(read_buffer, sizeof(kExpectedContent)).ok());
    ASSERT_STREQ(read_buffer, kExpectedContent);
  }

  {
    stream::IntervalReader res = update_bundle.GetTargetPayload("file2");
    ASSERT_OK(res.status());

    const char kExpectedContent[] = "file 2 content";
    char read_buffer[sizeof(kExpectedContent) + 1] = {0};
    ASSERT_TRUE(res.Read(read_buffer, sizeof(kExpectedContent)).ok());
    ASSERT_STREQ(read_buffer, kExpectedContent);
  }

  {
    stream::IntervalReader res = update_bundle.GetTargetPayload("non-exist");
    ASSERT_EQ(res.status(), Status::NotFound());
  }
}

TEST_F(UpdateBundleTest, PersistManifest) {
  backend().SetTrustedRoot(kDevSignedRoot);
  StageTestBundle(kTestDevBundle);
  UpdateBundleAccessor update_bundle(blob_reader(), backend());

  ASSERT_OK(update_bundle.OpenAndVerify());

  std::byte manifest_buffer[sizeof(kTestBundleManifest)] = {};
  stream::MemoryWriter manifest_writer(manifest_buffer);
  backend().SetManifestWriter(&manifest_writer);
  ASSERT_FALSE(backend().BeforeManifestWriteCalled());
  ASSERT_FALSE(backend().AfterManifestWriteCalled());
  ASSERT_OK(update_bundle.PersistManifest());
  ASSERT_TRUE(backend().BeforeManifestWriteCalled());
  ASSERT_TRUE(backend().AfterManifestWriteCalled());

  ASSERT_EQ(
      memcmp(manifest_buffer, kTestBundleManifest, sizeof(kTestBundleManifest)),
      0);
}

TEST_F(UpdateBundleTest, PersistManifestFailIfNotVerified) {
  backend().SetTrustedRoot(kDevSignedRoot);
  StageTestBundle(kTestBadProdSignature);
  UpdateBundleAccessor update_bundle(blob_reader(), backend());

  ASSERT_FAIL(update_bundle.OpenAndVerify());

  std::byte manifest_buffer[sizeof(kTestBundleManifest)];
  stream::MemoryWriter manifest_writer(manifest_buffer);
  backend().SetManifestWriter(&manifest_writer);
  ASSERT_FALSE(backend().BeforeManifestWriteCalled());
  ASSERT_FALSE(backend().AfterManifestWriteCalled());
  ASSERT_FAIL(update_bundle.PersistManifest());
  ASSERT_FALSE(backend().BeforeManifestWriteCalled());
  ASSERT_FALSE(backend().AfterManifestWriteCalled());
}

TEST_F(UpdateBundleTest, SelfVerificationWithIncomingRoot) {
  StageTestBundle(kTestDevBundleWithRoot);
  UpdateBundleAccessor update_bundle(
      blob_reader(), backend(), /* disable_verification = */ true);

  ASSERT_OK(update_bundle.OpenAndVerify());
  // Self verification must not persist anything.
  ASSERT_FALSE(backend().IsNewRootPersisted());

  // Manifest persisting should be allowed as well.
  std::byte manifest_buffer[sizeof(kTestBundleManifest)];
  stream::MemoryWriter manifest_writer(manifest_buffer);
  backend().SetManifestWriter(&manifest_writer);
  ASSERT_OK(update_bundle.PersistManifest());

  ASSERT_EQ(
      memcmp(manifest_buffer, kTestBundleManifest, sizeof(kTestBundleManifest)),
      0);
}

TEST_F(UpdateBundleTest, SelfVerificationWithoutIncomingRoot) {
  StageTestBundle(kTestDevBundle);
  UpdateBundleAccessor update_bundle(
      blob_reader(), backend(), /* disable_verification = */ true);

  ASSERT_OK(update_bundle.OpenAndVerify());
}

TEST_F(UpdateBundleTest, SelfVerificationWithMessedUpRoot) {
  StageTestBundle(kTestDevBundleWithProdRoot);
  UpdateBundleAccessor update_bundle(
      blob_reader(), backend(), /* disable_verification = */ true);

  ASSERT_FAIL(update_bundle.OpenAndVerify());
}

TEST_F(UpdateBundleTest, SelfVerificationChecksMissingHashes) {
  StageTestBundle(kTestBundleMissingTargetHashFile0);
  UpdateBundleAccessor update_bundle(
      blob_reader(), backend(), /* disable_verification = */ true);

  ASSERT_FAIL(update_bundle.OpenAndVerify());
}

TEST_F(UpdateBundleTest, SelfVerificationChecksBadHashes) {
  StageTestBundle(kTestBundleMismatchedTargetHashFile0);
  UpdateBundleAccessor update_bundle(
      blob_reader(), backend(), /* disable_verification = */ true);

  ASSERT_FAIL(update_bundle.OpenAndVerify());
}

TEST_F(UpdateBundleTest, SelfVerificationIgnoresUnsignedBundle) {
  StageTestBundle(kTestUnsignedBundleWithRoot);
  UpdateBundleAccessor update_bundle(
      blob_reader(), backend(), /* disable_verification = */ true);

  ASSERT_OK(update_bundle.OpenAndVerify());
}

TEST_F(UpdateBundleTest, OpenAndVerifySucceedsWithAllVerification) {
  backend().SetTrustedRoot(kDevSignedRoot);
  backend().SetCurrentManifest(kTestBundleManifest);
  StageTestBundle(kTestProdBundle);
  UpdateBundleAccessor update_bundle(blob_reader(), backend());

  ASSERT_FALSE(backend().IsNewRootPersisted());
  ASSERT_FALSE(backend().BeforeManifestReadCalled());
  ASSERT_OK(update_bundle.OpenAndVerify());
  ASSERT_TRUE(backend().IsNewRootPersisted());
  ASSERT_TRUE(backend().BeforeManifestReadCalled());

  ASSERT_OK(update_bundle.Close());
  VerifyAllBundleOperationsDisallowed(update_bundle);
}

TEST_F(UpdateBundleTest,
       OpenAndVerifyWithoutIncomingRootSucceedsWithAllVerification) {
  backend().SetTrustedRoot(kDevSignedRoot);
  backend().SetCurrentManifest(kTestBundleManifest);
  // kTestDevBundle does not contain an incoming root. See
  // pw_software_update/py/pw_software_update/generate_test_bundle.py for
  // detail of generation.
  StageTestBundle(kTestDevBundle);
  UpdateBundleAccessor update_bundle(blob_reader(), backend());

  ASSERT_FALSE(backend().IsNewRootPersisted());
  ASSERT_FALSE(backend().BeforeManifestReadCalled());
  ASSERT_OK(update_bundle.OpenAndVerify());
  ASSERT_FALSE(backend().IsNewRootPersisted());
  ASSERT_TRUE(backend().BeforeManifestReadCalled());

  ASSERT_OK(update_bundle.Close());
  VerifyAllBundleOperationsDisallowed(update_bundle);
}

TEST_F(UpdateBundleTest, OpenAndVerifyFailsOnMismatchedRootKeyAndSignature) {
  backend().SetTrustedRoot(kDevSignedRoot);
  backend().SetCurrentManifest(kTestBundleManifest);
  // kTestMismatchedRootKeyAndSignature has a dev root metadata that is
  // prod signed. The root metadata will not be able to verify itself.
  // See pw_software_update/py/pw_software_update/generate_test_bundle.py for
  // detail of generation.
  StageTestBundle(kTestMismatchedRootKeyAndSignature);
  UpdateBundleAccessor update_bundle(blob_reader(), backend());
  CheckOpenAndVerifyFail(update_bundle, false);
}

TEST_F(UpdateBundleTest, OpenAndVerifyFailsOnBadProdSignature) {
  backend().SetTrustedRoot(kDevSignedRoot);
  backend().SetCurrentManifest(kTestBundleManifest);
  StageTestBundle(kTestBadProdSignature);
  UpdateBundleAccessor update_bundle(blob_reader(), backend());
  CheckOpenAndVerifyFail(update_bundle, false);
}

TEST_F(UpdateBundleTest, OpenAndVerifyFailsOnBadTargetsSignature) {
  backend().SetTrustedRoot(kDevSignedRoot);
  backend().SetCurrentManifest(kTestBundleManifest);
  StageTestBundle(kTestBadTargetsSignature);
  UpdateBundleAccessor update_bundle(blob_reader(), backend());
  CheckOpenAndVerifyFail(update_bundle, true);
}

TEST_F(UpdateBundleTest, OpenAndVerifyFailsOnBadTargetsRollBack) {
  backend().SetTrustedRoot(kDevSignedRoot);
  backend().SetCurrentManifest(kTestBundleManifest);
  StageTestBundle(kTestTargetsRollback);
  UpdateBundleAccessor update_bundle(blob_reader(), backend());
  CheckOpenAndVerifyFail(update_bundle, true);
}

TEST_F(UpdateBundleTest, OpenAndVerifySucceedsWithoutExistingManifest) {
  backend().SetTrustedRoot(kDevSignedRoot);
  StageTestBundle(kTestProdBundle);
  UpdateBundleAccessor update_bundle(blob_reader(), backend());

  ASSERT_FALSE(backend().IsNewRootPersisted());
  ASSERT_OK(update_bundle.OpenAndVerify());
  ASSERT_TRUE(backend().IsNewRootPersisted());
}

TEST_F(UpdateBundleTest, OpenAndVerifyFailsOnRootRollback) {
  backend().SetTrustedRoot(kDevSignedRoot);
  backend().SetCurrentManifest(kTestBundleManifest);
  StageTestBundle(kTestRootRollback);
  UpdateBundleAccessor update_bundle(blob_reader(), backend());
  CheckOpenAndVerifyFail(update_bundle, false);
}

TEST_F(UpdateBundleTest, OpenAndVerifyFailsOnMismatchedTargetHashFile0) {
  backend().SetTrustedRoot(kDevSignedRoot);
  backend().SetCurrentManifest(kTestBundleManifest);
  // `kTestBundleMismatchedTargetHashFile0` is auto generated by
  // pw_software_update/py/pw_software_update/generate_test_bundle.py.
  // The hash value for file 0 in the targets metadata is made incorrect.
  StageTestBundle(kTestBundleMismatchedTargetHashFile0);
  UpdateBundleAccessor update_bundle(blob_reader(), backend());
  CheckOpenAndVerifyFail(update_bundle, true);
}

TEST_F(UpdateBundleTest, OpenAndVerifyFailsOnMismatchedTargetHashFile1) {
  backend().SetTrustedRoot(kDevSignedRoot);
  backend().SetCurrentManifest(kTestBundleManifest);
  // `kTestBundleMismatchedTargetHashFile1` is auto generated by
  // pw_software_update/py/pw_software_update/generate_test_bundle.py
  // The hash value for file 1 in the targets metadata is made incorrect.
  StageTestBundle(kTestBundleMismatchedTargetHashFile1);
  UpdateBundleAccessor update_bundle(blob_reader(), backend());
  CheckOpenAndVerifyFail(update_bundle, true);
}

TEST_F(UpdateBundleTest, OpenAndVerifyFailsOnMissingTargetHashFile0) {
  backend().SetTrustedRoot(kDevSignedRoot);
  backend().SetCurrentManifest(kTestBundleManifest);
  // `kTestBundleMismatchedTargetHashFile0` is auto generated by
  // pw_software_update/py/pw_software_update/generate_test_bundle.py.
  // The hash value for file 0 is removed.
  StageTestBundle(kTestBundleMissingTargetHashFile0);
  UpdateBundleAccessor update_bundle(blob_reader(), backend());
  CheckOpenAndVerifyFail(update_bundle, true);
}

TEST_F(UpdateBundleTest, OpenAndVerifyFailsOnMissingTargetHashFile1) {
  backend().SetTrustedRoot(kDevSignedRoot);
  backend().SetCurrentManifest(kTestBundleManifest);
  // `kTestBundleMismatchedTargetHashFile1` is auto generated by
  // pw_software_update/py/pw_software_update/generate_test_bundle.py
  // The hash value for file 1 is removed.
  StageTestBundle(kTestBundleMissingTargetHashFile1);
  UpdateBundleAccessor update_bundle(blob_reader(), backend());
  CheckOpenAndVerifyFail(update_bundle, true);
}

TEST_F(UpdateBundleTest, OpenAndVerifyFailsOnMismatchedTargetLengthFile0) {
  backend().SetTrustedRoot(kDevSignedRoot);
  backend().SetCurrentManifest(kTestBundleManifest);
  // `kTestBundleMismatchedTargetLengthFile0` is auto generated by
  // pw_software_update/py/pw_software_update/generate_test_bundle.py.
  // The length value for file 0 in the targets metadata is made incorrect (1).
  StageTestBundle(kTestBundleMismatchedTargetLengthFile0);
  UpdateBundleAccessor update_bundle(blob_reader(), backend());
  CheckOpenAndVerifyFail(update_bundle, true);
}

TEST_F(UpdateBundleTest, OpenAndVerifyFailsOnMismatchedTargetLengthFile1) {
  backend().SetTrustedRoot(kDevSignedRoot);
  backend().SetCurrentManifest(kTestBundleManifest);
  // `kTestBundleMismatchedTargetLengthFile1` is auto generated by
  // pw_software_update/py/pw_software_update/generate_test_bundle.py.
  // The length value for file 0 in the targets metadata is made incorrect (1).
  StageTestBundle(kTestBundleMismatchedTargetLengthFile1);
  UpdateBundleAccessor update_bundle(blob_reader(), backend());
  CheckOpenAndVerifyFail(update_bundle, true);
}

TEST_F(UpdateBundleTest, OpenAndVerifySucceedsWithPersonalizedOutFile0) {
  backend().SetTrustedRoot(kDevSignedRoot);
  backend().SetCurrentManifest(kTestBundleManifest);
  // `kTestBundlePersonalizedOutFile0` is auto generated by
  // pw_software_update/py/pw_software_update/generate_test_bundle.py
  // The payload for file 0 is removed from the bundle to emulate being
  // personalized out.
  StageTestBundle(kTestBundlePersonalizedOutFile0);
  UpdateBundleAccessor update_bundle(blob_reader(), backend());

  ASSERT_OK(update_bundle.OpenAndVerify());
}

TEST_F(UpdateBundleTest, OpenAndVerifySucceedsWithPersonalizedOutFile1) {
  backend().SetTrustedRoot(kDevSignedRoot);
  backend().SetCurrentManifest(kTestBundleManifest);
  // `kTestBundlePersonalizedOutFile1` is auto generated by
  // pw_software_update/py/pw_software_update/generate_test_bundle.py
  // The payload for file 1 is removed from the bundle to emulate being
  // personalized out.
  StageTestBundle(kTestBundlePersonalizedOutFile1);
  UpdateBundleAccessor update_bundle(blob_reader(), backend());

  ASSERT_OK(update_bundle.OpenAndVerify());
}

TEST_F(UpdateBundleTest,
       PersonalizationVerificationFailsWithoutDeviceManifest) {
  backend().SetTrustedRoot(kDevSignedRoot);
  // `kTestBundlePersonalizedOutFile0` is auto generated by
  // pw_software_update/py/pw_software_update/generate_test_bundle.py
  // The payload for file 0 is removed from the bundle to emulate being
  // personalized out.
  StageTestBundle(kTestBundlePersonalizedOutFile0);
  UpdateBundleAccessor update_bundle(blob_reader(), backend());

  ASSERT_FAIL(update_bundle.OpenAndVerify());
}

}  // namespace pw::software_update
