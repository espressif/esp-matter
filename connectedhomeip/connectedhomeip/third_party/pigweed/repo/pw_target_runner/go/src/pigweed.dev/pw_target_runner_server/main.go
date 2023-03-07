// Copyright 2019 The Pigweed Authors
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy of
// the License at
//
//	https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations under
// the License.
package main

import (
	"errors"
	"flag"
	"fmt"
	"io/ioutil"
	"log"

	"github.com/golang/protobuf/proto"
	"pigweed.dev/pw_target_runner"

	pb "pigweed.dev/proto/pw_target_runner/exec_server_config_pb"
)

// ServerOptions contains command-line options for the server.
type ServerOptions struct {
	// Path to a server configuration file.
	config string

	// Port on which to run.
	port int
}

// configureServerFromFile sets up the server with workers specifyed in a
// config file. The file contains a pw.target_runner.ServerConfig protobuf
// message in canonical protobuf text format.
func configureServerFromFile(s *pw_target_runner.Server, filepath string) error {
	content, err := ioutil.ReadFile(filepath)
	if err != nil {
		return err
	}

	var config pb.ServerConfig
	if err := proto.UnmarshalText(string(content), &config); err != nil {
		return err
	}

	log.Printf("Parsed server configuration from %s\n", filepath)

	runners := config.GetRunner()
	if runners == nil {
		return nil
	}

	// Create an exec worker for each of the runner messages listed in the
	// config and register them with the server.
	for i, runner := range runners {
		// Build the complete command for the worker from its "command"
		// and "args" fields in the proto message. The command is
		// required; arguments are optional.
		cmd := []string{runner.GetCommand()}
		if cmd[0] == "" {
			msg := fmt.Sprintf(
				"ServerConfig.runner[%d] does not specify a command; skipping\n", i)
			return errors.New(msg)
		}

		if args := runner.GetArgs(); args != nil {
			cmd = append(cmd, args...)
		}

		worker := pw_target_runner.NewExecDeviceRunner(i, cmd)
		s.RegisterWorker(worker)

		log.Printf(
			"Registered ExecDeviceRunner %s with args %v\n",
			cmd[0],
			cmd[1:])
	}

	return nil
}

func main() {
	configPtr := flag.String("config", "", "Path to server configuration file")
	portPtr := flag.Int("port", 8080, "Server port")

	flag.Parse()

	server := pw_target_runner.NewServer()

	if *configPtr != "" {
		if err := configureServerFromFile(server, *configPtr); err != nil {
			log.Fatalf("Failed to parse config file %s: %v", *configPtr, err)
		}
	}

	if err := server.Bind(*portPtr); err != nil {
		log.Fatal(err)
	}

	if err := server.Serve(); err != nil {
		log.Fatalf("Failed to start server: %v", err)
	}
}
