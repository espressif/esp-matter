.. _module-pw_target_runner-go:

--
Go
--

Server
------

.. TODO(frolv): Build and host documentation using godoc and link to it.

Full API documentation for the server library can be found here.

Example program
^^^^^^^^^^^^^^^

The code below implements a very basic test server with two test workers which
print out the path of the tests they are scheduled to run.

.. code-block:: go

  package main

  import (
  	"flag"
  	"log"

  	pb "pigweed.dev/proto/pw_target_runner/target_runner_pb"
  	"pigweed.dev/pw_target_runner"
  )

  // Custom test worker that implements the interface server.UnitTestRunner.
  type MyWorker struct {
  	id int
  }

  func (w *MyWorker) WorkerStart() error {
  	log.Printf("Starting test worker %d\n", w.id)
  	return nil
  }

  func (w *MyWorker) WorkerExit() {
  	log.Printf("Exiting test worker %d\n", w.id)
  }

  func (w *MyWorker) HandleRunRequest(req *server.UnitTestRunRequest) *server.UnitTestRunResponse {
  	log.Printf("Worker %d running unit test %s\n", w.id, req.Path)
  	return &server.UnitTestRunResponse{
  		Output: []byte("Success!"),
  		Status: pb.TestStatus_SUCCESS,
  	}
  }

  // To run:
  //
  //   $ go build -o server
  //   $ ./server -port 80
  //
  func main() {
  	port := flag.Int("port", 8080, "Port on which to run server")
        flag.Parse()

  	s := server.New()

  	// Create and register as many unit test workers as you need.
  	s.RegisterWorker(&MyWorker{id: 0})
  	s.RegisterWorker(&MyWorker{id: 1})

  	if err := s.Bind(*port); err != nil {
  		log.Fatalf("Failed to bind to port %d: %v", *port, err)
  	}

  	if err := s.Serve(); err != nil {
  		log.Fatalf("Failed to start server: %v", err)
  	}
  }
