// Copyright 2019 The Pigweed Authors
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

package pw_target_runner

import (
	"fmt"
	"log"
	"os"
	"os/exec"

	pb "pigweed.dev/proto/pw_target_runner/target_runner_pb"
)

// ExecDeviceRunner is a struct that implements the DeviceRunner interface,
// running its executables through a command with the path of the executable as
// an argument.
type ExecDeviceRunner struct {
	command []string
	logger  *log.Logger
}

// NewExecDeviceRunner creates a new ExecDeviceRunner with a custom logger.
func NewExecDeviceRunner(id int, command []string) *ExecDeviceRunner {
	logPrefix := fmt.Sprintf("[ExecDeviceRunner %d] ", id)
	logger := log.New(os.Stdout, logPrefix, log.LstdFlags)
	return &ExecDeviceRunner{command, logger}
}

// WorkerStart starts the worker. Part of DeviceRunner interface.
func (r *ExecDeviceRunner) WorkerStart() error {
	r.logger.Printf("Starting worker")
	return nil
}

// WorkerExit exits the worker. Part of DeviceRunner interface.
func (r *ExecDeviceRunner) WorkerExit() {
	r.logger.Printf("Exiting worker")
}

// HandleRunRequest runs a requested binary by executing the runner's command
// with the binary path as an argument. The combined stdout and stderr of the
// command is returned as the run output.
func (r *ExecDeviceRunner) HandleRunRequest(req *RunRequest) *RunResponse {
	res := &RunResponse{Status: pb.RunStatus_SUCCESS}

	r.logger.Printf("Running executable %s\n", req.Path)

	// Copy runner command args, appending the binary path to the end.
	args := append([]string(nil), r.command[1:]...)
	args = append(args, req.Path)

	cmd := exec.Command(r.command[0], args...)
	output, err := cmd.CombinedOutput()

	if err != nil {
		if e, ok := err.(*exec.ExitError); ok {
			// A nonzero exit status is interpreted as a failure.
			r.logger.Printf("Command exited with status %d\n", e.ExitCode())
			res.Status = pb.RunStatus_FAILURE
		} else {
			// Any other error with the command execution is
			// reported as an internal error to the requester.
			r.logger.Printf("Command failed: %v\n", err)
			res.Err = err
			return res
		}
	}

	res.Output = output
	return res
}
