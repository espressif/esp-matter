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
	"errors"
	"fmt"
	"log"
	"os"
	"sync"
	"sync/atomic"
	"time"

	pb "pigweed.dev/proto/pw_target_runner/target_runner_pb"
)

// RunRequest represents a client request to run a single executable on-device.
type RunRequest struct {
	// Filesystem path to the executable.
	Path string

	// Channel to which the response is sent back.
	ResponseChannel chan<- *RunResponse

	// Time when the request was queued. Internal to the worker pool.
	queueStart time.Time
}

// RunResponse is the response sent after a run request is processed.
type RunResponse struct {
	// Length of time that the run request was queued before being handled
	// by a worker. Set by the worker pool.
	QueueTime time.Duration

	// Length of time the runner command took to run the executable.
	// Set by the worker pool.
	RunTime time.Duration

	// Raw output of the execution.
	Output []byte

	// Result of the run.
	Status pb.RunStatus

	// Error that occurred during the run, if any. If this is not nil, none
	// of the other fields in this struct are guaranteed to be valid.
	Err error
}

// DeviceRunner represents a worker which handles run requests.
type DeviceRunner interface {
	// WorkerStart is the lifecycle hook called when the worker routine is
	// started. Any resources required by the worker should be initialized
	// here.
	WorkerStart() error

	// HandleRunRequest is the method called when an executable is scheduled
	// to run on the worker by the worker pool. It processes the request,
	// runs the executable, and returns an appropriate response.
	HandleRunRequest(*RunRequest) *RunResponse

	// WorkerExit is the lifecycle hook called before the worker exits.
	// Should be used to clean up any resources used by the worker.
	WorkerExit()
}

// WorkerPool represents a collection of device runners which run on-device
// binaries. The worker pool distributes requests to run binaries among its
// available workers.
type WorkerPool struct {
	activeWorkers uint32
	logger        *log.Logger
	workers       []DeviceRunner
	waitGroup     sync.WaitGroup
	reqChannel    chan *RunRequest
	quitChannel   chan bool
}

var (
	errWorkerPoolActive    = errors.New("Worker pool is running")
	errNoRegisteredWorkers = errors.New("No workers registered in pool")
)

// newWorkerPool creates an empty worker pool.
func newWorkerPool(name string) *WorkerPool {
	logPrefix := fmt.Sprintf("[%s] ", name)
	return &WorkerPool{
		logger:      log.New(os.Stdout, logPrefix, log.LstdFlags),
		workers:     make([]DeviceRunner, 0),
		reqChannel:  make(chan *RunRequest, 1024),
		quitChannel: make(chan bool, 64),
	}
}

// RegisterWorker adds a new worker to the pool. This cannot be done when the
// pool is processing requests; Stop() must be called first.
func (p *WorkerPool) RegisterWorker(worker DeviceRunner) error {
	if p.Active() {
		return errWorkerPoolActive
	}
	p.workers = append(p.workers, worker)
	return nil
}

// Start launches all registered workers in the pool.
func (p *WorkerPool) Start() error {
	if p.Active() {
		return errWorkerPoolActive
	}

	p.logger.Printf("Starting %d workers\n", len(p.workers))
	for _, worker := range p.workers {
		p.waitGroup.Add(1)
		atomic.AddUint32(&p.activeWorkers, 1)
		go p.runWorker(worker)
	}

	return nil
}

// Stop terminates all running workers in the pool. The work queue is not
// cleared; queued requests persist and can be processed by calling Start()
// again.
func (p *WorkerPool) Stop() {
	if !p.Active() {
		return
	}

	// Send N quit commands to the workers and wait for them to exit.
	for i := uint32(0); i < p.activeWorkers; i++ {
		p.quitChannel <- true
	}
	p.waitGroup.Wait()

	p.logger.Println("All workers in pool stopped")
}

// Active returns true if any worker routines are currently running.
func (p *WorkerPool) Active() bool {
	return p.activeWorkers > 0
}

// QueueExecutable adds an executable to the worker pool's queue. If no workers
// are registered in the pool, this operation fails and an immediate response is
// sent back to the requester indicating the error.
func (p *WorkerPool) QueueExecutable(req *RunRequest) {
	if len(p.workers) == 0 {
		p.logger.Printf("Attempt to queue executable %s with no active workers", req.Path)
		req.ResponseChannel <- &RunResponse{
			Err: errNoRegisteredWorkers,
		}
		return
	}

	p.logger.Printf("Queueing executable %s\n", req.Path)

	// Start tracking how long the request is queued.
	req.queueStart = time.Now()
	p.reqChannel <- req
}

// runWorker is a function run by the worker pool in a separate goroutine for
// each of its registered workers. The function is responsible for calling the
// appropriate worker lifecycle hooks and processing requests as they come in
// through the worker pool's queue.
func (p *WorkerPool) runWorker(worker DeviceRunner) {
	defer func() {
		atomic.AddUint32(&p.activeWorkers, ^uint32(0))
		p.waitGroup.Done()
	}()

	if err := worker.WorkerStart(); err != nil {
		return
	}

processLoop:
	for {
		// Force the quit channel to be processed before the request
		// channel by using a select statement with an empty default
		// case to make the read non-blocking. If the quit channel is
		// empty, the code will fall through to the main select below.
		select {
		case q, ok := <-p.quitChannel:
			if q || !ok {
				break processLoop
			}
		default:
		}

		select {
		case q, ok := <-p.quitChannel:
			if q || !ok {
				break processLoop
			}
		case req, ok := <-p.reqChannel:
			if !ok {
				continue
			}

			queueTime := time.Since(req.queueStart)

			runStart := time.Now()
			res := worker.HandleRunRequest(req)
			res.RunTime = time.Since(runStart)

			res.QueueTime = queueTime
			req.ResponseChannel <- res
		}
	}

	worker.WorkerExit()
}
