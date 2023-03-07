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
#include "pw_thread_freertos/util.h"

#include "FreeRTOS.h"
#include "list.h"
#include "pw_function/function.h"
#include "pw_log/log.h"
#include "pw_status/status.h"
#include "pw_status/try.h"
#include "task.h"

// The externed symbols below are all internal FreeRTOS kernel variables from
// FreeRTOS/Source/tasks.c needed in order to iterate through all of the threads
// from interrupts which the native APIs do not permit.

extern "C" PRIVILEGED_DATA volatile BaseType_t xSchedulerRunning;

extern "C" PRIVILEGED_DATA TaskHandle_t volatile pxCurrentTCB;

// Prioritised ready tasks.
extern "C" PRIVILEGED_DATA List_t pxReadyTasksLists[configMAX_PRIORITIES];

// Points to the delayed task list currently being used.
extern "C" PRIVILEGED_DATA List_t* volatile pxDelayedTaskList;

// Points to the delayed task list currently being used to hold tasks that have
// overflowed the current tick count.
extern "C" PRIVILEGED_DATA List_t* volatile pxOverflowDelayedTaskList;

#if INCLUDE_vTaskDelete == 1
// Tasks that have been deleted - but their memory not yet freed.
extern "C" PRIVILEGED_DATA List_t xTasksWaitingTermination;
#endif  // INCLUDE_vTaskDelete == 1

#if INCLUDE_vTaskSuspend == 1
// Tasks that are currently suspended.
extern "C" PRIVILEGED_DATA List_t xSuspendedTaskList;
#endif  // INCLUDE_vTaskSuspend == 1

namespace pw::thread::freertos {
namespace {

Status ForEachThreadInList(List_t* list,
                           const eTaskState default_list_state,
                           const ThreadCallback& cb) {
  if (listCURRENT_LIST_LENGTH(list) == 0) {
    return OkStatus();
  }

  Status status = OkStatus();
  // Note that these are pointers to the thread control blocks, however the
  // list macros from FreeRTOS do not cast the types and ergo we use void *.
  void* current_thread;
  void* first_thread_in_list;
  listGET_OWNER_OF_NEXT_ENTRY(first_thread_in_list, list);
  do {
    listGET_OWNER_OF_NEXT_ENTRY(current_thread, list);
    // We must finish the list iteration to restore the list state, but
    // we want to stop invoking callbacks upon the first failure.
    if (status.ok()) {
      // Note that the lists do not contain the running state, so instead
      // check for each thread whether it is currently running.
      const TaskHandle_t current_thread_handle =
          reinterpret_cast<TaskHandle_t>(current_thread);
      if (!cb(current_thread_handle,
              current_thread_handle == pxCurrentTCB ? eRunning
                                                    : default_list_state)) {
        status = Status::Aborted();
      }
    }
  } while (current_thread != first_thread_in_list);
  return status;
}

}  // namespace

Status ForEachThread(const ThreadCallback& cb) {
  if (xSchedulerRunning == pdFALSE) {
    return Status::FailedPrecondition();
  }

  for (size_t i = 0; i < configMAX_PRIORITIES; ++i) {
    PW_TRY(ForEachThreadInList(&pxReadyTasksLists[i], eReady, cb));
  }
  PW_TRY(ForEachThreadInList(pxDelayedTaskList, eBlocked, cb));
  PW_TRY(ForEachThreadInList(pxOverflowDelayedTaskList, eBlocked, cb));
#if INCLUDE_vTaskDelete == 1
  PW_TRY(ForEachThreadInList(&xTasksWaitingTermination, eDeleted, cb));
#endif  // INCLUDE_vTaskDelete == 1
#if INCLUDE_vTaskSuspend == 1
  PW_TRY(ForEachThreadInList(&xSuspendedTaskList, eSuspended, cb));
#endif  // INCLUDE_vTaskSuspend == 1
  return OkStatus();
}

}  // namespace pw::thread::freertos
