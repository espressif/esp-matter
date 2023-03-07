// Copyright 2020 The Pigweed Authors
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
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <iterator>

namespace pw::tokenizer {

// Reads entries from a binary token string database. This class does not copy
// or modify the contents of the database.
//
// A binary token database is comprised of a 16-byte header followed by an array
// of 8-byte entries and a table of null-terminated strings. The header
// specifies the number of entries. Each entry contains information about a
// tokenized string: the token and removal date, if any. All fields are
// little-endian.
//
//            Header
//            ======
//   Offset  Size  Field
//   -----------------------------------
//        0     6  Magic number (TOKENS)
//        6     2  Version (00 00)
//        8     4  Entry count
//       12     4  Reserved
//
//             Entry
//             =====
//   Offset  Size  Field
//   -----------------------------------
//        0     4  Token
//        4     4  Removal date (d m yy)
//
// Entries are sorted by token. A string table with a null-terminated string for
// each entry in order follows the entries.
//
// Entries are accessed by iterating over the database. A O(n) Find function is
// also provided. In typical use, a TokenDatabase is preprocessed by a
// Detokenizer into a std::unordered_map.
class TokenDatabase {
 public:
  // Internal struct that describes how the underlying binary token database
  // stores entries. RawEntries generally should not be used directly. Instead,
  // use an Entry, which contains a pointer to the entry's string.
  struct RawEntry {
    uint32_t token;
    uint32_t date_removed;
  };

  static_assert(sizeof(RawEntry) == 8u);

  // An entry in the token database. This struct adds the string to a RawEntry.
  struct Entry {
    // The token calculated for this string.
    uint32_t token;

    // The date the token and string was removed from the database, or
    // 0xFFFFFFFF if it was never removed. Dates are encoded such that natural
    // integer sorting sorts from oldest to newest dates. The day is stored an
    // an 8-bit day, 8-bit month, and 16-bit year, packed into a little-endian
    // uint32_t.
    uint32_t date_removed;

    // The null-terminated string represented by this token.
    const char* string;
  };

  // Iterator for TokenDatabase values. Note that this is not a normal STL-style
  // iterator, since * returns a value instead of a reference.
  class Iterator {
   public:
    constexpr Iterator(const RawEntry* raw_entry, const char* string)
        : raw_(raw_entry), string_(string) {}

    // Constructs a TokenDatabase::Entry for the entry this iterator refers to.
    constexpr Entry entry() const {
      return {raw_->token, raw_->date_removed, string_};
    }

    constexpr Iterator& operator++() {
      raw_ += 1;
      // Move string_ to the character beyond the next null terminator.
      while (*string_++ != '\0') {
      }
      return *this;
    }
    constexpr Iterator operator++(int) {
      Iterator previous(raw_, string_);
      operator++();
      return previous;
    }
    constexpr bool operator==(const Iterator& rhs) const {
      return raw_ == rhs.raw_;
    }
    constexpr bool operator!=(const Iterator& rhs) const {
      return raw_ != rhs.raw_;
    }

    // Derefencing a TokenDatabase::Iterator returns an Entry, not an Entry&.
    constexpr Entry operator*() const { return entry(); }

    // Point directly into the underlying RawEntry. Strings are not accessible
    // via this operator.
    constexpr const RawEntry* operator->() const { return raw_; }

    constexpr ptrdiff_t operator-(const Iterator& rhs) const {
      return raw_ - rhs.raw_;
    }

   private:
    const RawEntry* raw_;
    const char* string_;
  };

  // A list of token entries returned from a Find operation. This object can be
  // iterated over or indexed as an array.
  class Entries {
   public:
    constexpr Entries(const Iterator& begin, const Iterator& end)
        : begin_(begin), end_(end) {}

    // The number of entries in this list.
    constexpr size_t size() const { return end_ - begin_; }

    // True of the list is empty.
    constexpr bool empty() const { return begin_ == end_; }

    // Accesses the specified entry in this set. Returns an Entry object, which
    // is constructed from the underlying raw entry. The index must be less than
    // size(). This operation is O(n) in size().
    Entry operator[](size_t index) const;

    constexpr const Iterator& begin() const { return begin_; }
    constexpr const Iterator& end() const { return end_; }

   private:
    Iterator begin_;
    Iterator end_;
  };

  // Returns true if the provided data is a valid token database. This checks
  // the magic number ("TOKENS"), version (which must be 0), and that there is
  // is one string for each entry in the database. A database with extra strings
  // or other trailing data is considered valid.
  template <typename ByteArray>
  static constexpr bool IsValid(const ByteArray& bytes) {
    return HasValidHeader(bytes) && EachEntryHasAString(bytes);
  }

  // Creates a TokenDatabase and checks if the provided data is valid at compile
  // time. Accepts references to constexpr containers (array, span, string_view,
  // etc.) with static storage duration. For example:
  //
  //   constexpr char kMyData[] = ...;
  //   constexpr TokenDatabase db = TokenDatabase::Create<kMyData>();
  //
  template <const auto& kDatabaseBytes>
  static constexpr TokenDatabase Create() {
    static_assert(
        HasValidHeader<decltype(kDatabaseBytes)>(kDatabaseBytes),
        "Databases must start with a 16-byte header that begins with TOKENS.");

    static_assert(EachEntryHasAString<decltype(kDatabaseBytes)>(kDatabaseBytes),
                  "The database must have at least one string for each entry.");

    return TokenDatabase(std::data(kDatabaseBytes));
  }

  // Creates a TokenDatabase from the provided byte array. The array may be a
  // span, array, or other container type. If the data is not valid, returns a
  // default-constructed database for which ok() is false.
  //
  // Prefer the Create overload that takes the data as a template parameter
  // whenever possible, since that function checks the integrity of the data at
  // compile time.
  template <typename ByteArray>
  static constexpr TokenDatabase Create(const ByteArray& database_bytes) {
    return IsValid<ByteArray>(database_bytes)
               ? TokenDatabase(std::data(database_bytes))
               : TokenDatabase();  // Invalid database.
  }
  // Creates a database with no data. ok() returns false.
  constexpr TokenDatabase() : begin_{.data = nullptr}, end_{.data = nullptr} {}

  // Returns all entries associated with this token. This is a O(n) operation.
  Entries Find(uint32_t token) const;

  // Returns the total number of entries (unique token-string pairs).
  constexpr size_t size() const {
    return (end_.data - begin_.data) / sizeof(RawEntry);
  }

  // True if this database was constructed with valid data.
  constexpr bool ok() const { return begin_.data != nullptr; }

  Iterator begin() const { return Iterator(begin_.entry, end_.data); }
  Iterator end() const { return Iterator(end_.entry, nullptr); }

 private:
  struct Header {
    std::array<char, 6> magic;
    uint16_t version;
    uint32_t entry_count;
    uint32_t reserved;
  };

  static_assert(sizeof(Header) == 2 * sizeof(RawEntry));

  template <typename ByteArray>
  static constexpr bool HasValidHeader(const ByteArray& bytes) {
    static_assert(sizeof(*std::data(bytes)) == 1u);

    if (std::size(bytes) < sizeof(Header)) {
      return false;
    }

    // Check the magic number and version.
    for (size_t i = 0; i < kMagicAndVersion.size(); ++i) {
      if (bytes[i] != kMagicAndVersion[i]) {
        return false;
      }
    }

    return true;
  }

  template <typename ByteArray>
  static constexpr bool EachEntryHasAString(const ByteArray& bytes) {
    const size_t entries = ReadEntryCount(std::data(bytes));

    // Check that the data is large enough to have a string table.
    if (std::size(bytes) < StringTable(entries)) {
      return false;
    }

    // Count the strings in the string table.
    size_t string_count = 0;
    for (auto i = std::begin(bytes) + StringTable(entries); i < std::end(bytes);
         ++i) {
      string_count += (*i == '\0') ? 1 : 0;
    }

    // Check that there is at least one string for each entry.
    return string_count >= entries;
  }

  // Reads the number of entries from a database header. Cast to the bytes to
  // uint8_t to avoid sign extension if T is signed.
  template <typename T>
  static constexpr uint32_t ReadEntryCount(const T* header_bytes) {
    const T* bytes = header_bytes + offsetof(Header, entry_count);
    return static_cast<uint8_t>(bytes[0]) |
           static_cast<uint8_t>(bytes[1]) << 8 |
           static_cast<uint8_t>(bytes[2]) << 16 |
           static_cast<uint8_t>(bytes[3]) << 24;
  }

  // Calculates the offset of the string table.
  static constexpr size_t StringTable(size_t entries) {
    return sizeof(Header) + entries * sizeof(RawEntry);
  }

  // The magic number that starts the table is "TOKENS". The version is encoded
  // next as two bytes.
  static constexpr std::array<char, 8> kMagicAndVersion = {
      'T', 'O', 'K', 'E', 'N', 'S', '\0', '\0'};

  template <typename Byte>
  constexpr TokenDatabase(const Byte bytes[])
      : TokenDatabase(bytes + sizeof(Header),
                      bytes + StringTable(ReadEntryCount(bytes))) {
    static_assert(sizeof(Byte) == 1u);
  }

  // It is illegal to reinterpret_cast in constexpr functions, but acceptable to
  // use unions. Instead of using a reinterpret_cast to change the byte pointer
  // to a RawEntry pointer, have a separate overload for each byte pointer type
  // and store them in a union.
  constexpr TokenDatabase(const char* begin, const char* end)
      : begin_{.data = begin}, end_{.data = end} {}

  constexpr TokenDatabase(const unsigned char* begin, const unsigned char* end)
      : begin_{.unsigned_data = begin}, end_{.unsigned_data = end} {}

  constexpr TokenDatabase(const signed char* begin, const signed char* end)
      : begin_{.signed_data = begin}, end_{.signed_data = end} {}

  // Store the beginning and end pointers as a union to avoid breaking constexpr
  // rules for reinterpret_cast.
  union {
    const RawEntry* entry;
    const char* data;
    const unsigned char* unsigned_data;
    const signed char* signed_data;
  } begin_, end_;
};

}  // namespace pw::tokenizer
