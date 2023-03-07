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

#include <algorithm>
#include <cctype>
#include <functional>
#include <iostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "pw_log/log.h"
#include "pw_span/span.h"

namespace {

// String used to prompt for user input in the CLI loop.
constexpr char kPrompt[] = ">";

// Convert the provided string to a lowercase equivalent.
std::string ToLower(std::string_view view) {
  std::string str{view};
  std::transform(str.begin(), str.end(), str.begin(), [](char c) {
    return std::tolower(c);
  });
  return str;
}

// Scan an input line for tokens, returning a vector containing each token.
// Tokens are either whitespace delimited strings or a quoted string which may
// contain spaces and is terminated by another quote. When delimiting by
// whitespace any consecutive sequence of whitespace is treated as a single
// delimiter.
//
// For example, the tokenization of the following line:
//
//   The duck said "quack, quack" before   eating   its bread
//
// Would result in the following tokens:
//
//   ["The", "duck", "said", "quack, quack", "before", "eating", "its", "bread"]
//
std::vector<std::string_view> TokenizeLine(std::string_view line) {
  size_t token_start = 0;
  size_t index = 0;
  bool in_quote = false;
  std::vector<std::string_view> tokens;

  while (index < line.size()) {
    // Trim leading/trailing whitespace for each token.
    while (index < line.size() && std::isspace(line[index])) {
      ++index;
    }

    if (index >= line.size()) {
      // Have reached the end and no further tokens remain.
      break;
    }

    token_start = index++;
    if (line[token_start] == '"') {
      in_quote = true;
      // Don't include the quote character.
      ++token_start;
    }

    // In a token, scan for the end of the token.
    while (index < line.size()) {
      if ((in_quote && line[index] == '"') ||
          (!in_quote && std::isspace(line[index]))) {
        break;
      }
      ++index;
    }

    if (index >= line.size() && in_quote) {
      PW_LOG_WARN("Assuming closing quote at EOL.");
    }

    tokens.push_back(line.substr(token_start, index - token_start));
    in_quote = false;
    ++index;
  }

  return tokens;
}

// Context supplied to (and mutable by) each command.
struct CommandContext {
  // When set to `true`, the CLI will exit once the active command returns.
  bool quit = false;
};

// Commands are given mutable CommandContext and a span tokens in the line of
// the command.
using Command =
    std::function<bool(CommandContext*, pw::span<std::string_view>)>;

// Echoes all arguments provided to cout.
bool CommandEcho(CommandContext* /*context*/,
                 pw::span<std::string_view> tokens) {
  bool first = true;
  for (const auto& token : tokens.subspan(1)) {
    if (!first) {
      std::cout << ' ';
    }

    std::cout << token;
    first = false;
  }
  std::cout << std::endl;

  return true;
}

// Quit the CLI.
bool CommandQuit(CommandContext* context,
                 pw::span<std::string_view> /*tokens*/) {
  context->quit = true;
  return true;
}

}  // namespace

int main(int /*argc*/, char* /*argv*/[]) {
  CommandContext context;
  std::unordered_map<std::string, Command> commands{
      {"echo", CommandEcho},
      {"exit", CommandQuit},
      {"quit", CommandQuit},
  };

  // Enter CLI loop.
  while (true) {
    // Prompt for input.
    std::string line;
    std::cout << kPrompt << ' ' << std::flush;
    std::getline(std::cin, line);

    // Tokenize provided line.
    auto tokens = TokenizeLine(line);
    if (tokens.empty()) {
      continue;
    }

    // Search for provided command.
    auto it = commands.find(ToLower(tokens[0]));
    if (it == commands.end()) {
      PW_LOG_ERROR("Unrecognized command \"%.*s\".",
                   static_cast<int>(tokens[0].size()),
                   tokens[0].data());
      continue;
    }

    // Invoke the command.
    Command command = it->second;
    command(&context, tokens);
    if (context.quit) {
      break;
    }
  }

  return EXIT_SUCCESS;
}
