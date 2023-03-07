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
//==============================================================================
// ninja -C out/host trace_sample
// ./out/host/obj/pw_trace_tokenized/trace_sample
// python pw_trace_tokenized/py/trace.py -i out1.bin -o trace.json
//       ./out/host/obj/pw_trace_tokenized/trace_sample
#include <stdio.h>

#include <array>
#include <chrono>

#include "pw_ring_buffer/prefixed_entry_ring_buffer.h"
#include "pw_trace/trace.h"

#ifndef SAMPLE_APP_SLEEP_MILLIS
#include <thread>
#define SAMPLE_APP_SLEEP_MILLIS(millis) \
  std::this_thread::sleep_for(std::chrono::milliseconds(millis));
#endif  // SAMPLE_APP_SLEEP_MILLIS

using namespace std::chrono;

namespace {

// Time helper function
auto start = system_clock::now();
uint32_t GetTimeSinceBootMillis() {
  auto delta = system_clock::now() - start;
  return floor<milliseconds>(delta).count();
}

// Creating a very simple runnable with predictable behaviour to help with the
// example. Each Runnable, has a method ShouldRun which indicates if it has work
// to do, calling Run will then do the work.
class SimpleRunnable {
 public:
  virtual const char* Name() const = 0;
  virtual bool ShouldRun() = 0;
  virtual void Run() = 0;
  virtual ~SimpleRunnable() {}
};

// Processing module
// Uses trace_id and groups to track the multiple stages of "processing".
// These are intentionally long running so they will be processing concurrently.
// The trace ID is used to seperates these concurrent jobs.
#undef PW_TRACE_MODULE_NAME
#define PW_TRACE_MODULE_NAME "Processing"
class ProcessingTask : public SimpleRunnable {
 public:
  // Run task maintains a buffer of "jobs" which just sleeps for an amount of
  // time and reposts the job until the value is zero. This gives an async
  // behaviour where multiple of the same job are happening concurrently, and
  // also has a nesting effect of a job having many stages.
  struct Job {
    uint32_t job_id;
    uint8_t value;
  };
  struct JobBytes {
    union {
      Job job;
      std::byte bytes[sizeof(Job)];
    };
  };
  ProcessingTask() {
    // Buffer is used for the job queue.
    pw::span<std::byte> buf_span = pw::span<std::byte>(
        reinterpret_cast<std::byte*>(jobs_buffer_), sizeof(jobs_buffer_));
    jobs_.SetBuffer(buf_span)
        .IgnoreError();  // TODO(b/242598609): Handle Status properly
  }
  const char* Name() const override { return "Processing Task"; }
  bool ShouldRun() override { return jobs_.EntryCount() > 0; }
  void Run() override {
    JobBytes job_bytes;
    size_t bytes_read;

    // Trace the job count backlog
    size_t entry_count = jobs_.EntryCount();

    // Get the next job from the queue.
    jobs_.PeekFront(job_bytes.bytes, &bytes_read)
        .IgnoreError();  // TODO(b/242598609): Handle Status properly
    jobs_.PopFront()
        .IgnoreError();  // TODO(b/242598609): Handle Status properly
    Job& job = job_bytes.job;

    // Process the job
    ProcessingJob(job);
    if (job.value > 0) {  // repost for more work if value > 0
      AddJobInternal(job.job_id, job.value - 1);
    } else {
      PW_TRACE_END("Job", "Process", job.job_id);
    }
    PW_TRACE_INSTANT_DATA("job_backlog_count",
                          "@pw_arg_counter",
                          &entry_count,
                          sizeof(entry_count));
  }
  void AddJob(uint32_t job_id, uint8_t value) {
    PW_TRACE_START_DATA(
        "Job", "Process", job_id, "@pw_py_struct_fmt:B", &value, sizeof(value));
    AddJobInternal(job_id, value);
  }

 private:
  static constexpr size_t kMaxJobs = 10;
  static constexpr size_t kProcessingTimePerValueMillis = 250;
  Job jobs_buffer_[kMaxJobs];
  pw::ring_buffer::PrefixedEntryRingBuffer jobs_{false};

  void ProcessingJob(const Job& job) {
    PW_TRACE_FUNCTION("Process", job.job_id);
    for (uint8_t i = 0; i < job.value; i++) {
      PW_TRACE_SCOPE("loop", "Process", job.job_id);
      SAMPLE_APP_SLEEP_MILLIS(50);  // Fake processing time
      SomeProcessing(&job);
    }
  }

  void SomeProcessing(const Job* job) {
    uint32_t id = job->job_id;
    PW_TRACE_FUNCTION("Process", id);
    SAMPLE_APP_SLEEP_MILLIS(
        kProcessingTimePerValueMillis);  // Fake processing time
  }
  void AddJobInternal(uint32_t job_id, uint8_t value) {
    JobBytes job{.job = {.job_id = job_id, .value = value}};
    jobs_.PushBack(job.bytes)
        .IgnoreError();  // TODO(b/242598609): Handle Status properly
  }
} processing_task;

// Input Module
// Uses traces in groups to indicate the different steps of reading the new
// event.
// Uses an instant data event to dump the read sample into the trace.
#undef PW_TRACE_MODULE_NAME
#define PW_TRACE_MODULE_NAME "Input"
class InputTask : public SimpleRunnable {
  // Every second generate new output
 public:
  const char* Name() const override { return "Input Task"; }
  bool ShouldRun() override {
    return (GetTimeSinceBootMillis() - last_run_time_ > kRunInterval);
  }
  void Run() override {
    last_run_time_ = GetTimeSinceBootMillis();
    PW_TRACE_FUNCTION("Input");
    SAMPLE_APP_SLEEP_MILLIS(50);
    uint8_t value = GetValue();
    PW_TRACE_INSTANT_DATA("value", "@pw_arg_counter", &value, sizeof(value));
    processing_task.AddJob(sample_count_, value);
    sample_count_++;
  }

 private:
  uint8_t GetValue() {
    PW_TRACE_FUNCTION("Input");
    SAMPLE_APP_SLEEP_MILLIS(100);  // Fake processing time
    return sample_count_ % 4 + 1;
  }
  size_t sample_count_ = 0;
  uint32_t last_run_time_ = 0;
  static constexpr uint32_t kRunInterval = 1000;
} input_task;

// Simple main loop acting as the "Kernel"
// Uses simple named trace durations to indicate which task/job is running
#undef PW_TRACE_MODULE_NAME
#define PW_TRACE_MODULE_NAME "Kernel"
void StartFakeKernel() {
  std::array<SimpleRunnable*, 2> tasks = {&input_task, &processing_task};

  bool idle = false;
  while (true) {
    bool have_any_run = false;
    for (auto& task : tasks) {
      if (task->ShouldRun()) {
        if (idle) {
          PW_TRACE_END("Idle", "Idle");
          idle = false;
        }
        have_any_run = true;
        // The task name is not a string literal and is therefore put in the
        // data section, so it can also work with tokenized trace.
        PW_TRACE_START_DATA(
            "Running", "@pw_arg_group", task->Name(), strlen(task->Name()));
        task->Run();
        PW_TRACE_END_DATA(
            "Running", "@pw_arg_group", task->Name(), strlen(task->Name()));
      }
    }
    if (!idle && !have_any_run) {
      PW_TRACE_START("Idle", "Idle");
      idle = true;
    }
  }
}

}  // namespace

void RunTraceSampleApp() { StartFakeKernel(); }
