// Copyright 2022 The Pigweed Authors
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

/** Parses CSV Database for easier lookups */

export class TokenDatabase {
  private tokens: Map<number, string> = new Map();

  constructor(readonly csv: string) {
    this.parseTokensToTokensMap(csv.split(/\r?\n/));
  }

  has(token: number): boolean {
    return this.tokens.has(token);
  }

  get(token: number): string | undefined {
    return this.tokens.get(token);
  }

  private parseTokensToTokensMap(csv: string[]) {
    for (const [lineNumber, line] of Object.entries(
      csv.map(line => line.split(/,/))
    )) {
      if (!line[0] || !line[2]) {
        continue;
      }
      if (!/^[a-fA-F0-9]+$/.test(line[0])) {
        // Malformed number
        console.error(
          new Error(
            `TokenDatabase number ${line[0]} at line ` +
              `${lineNumber} is not a valid hex number`
          )
        );
        continue;
      }
      const tokenNumber = parseInt(line[0], 16);
      // To extract actual string value of a token number, we:
      // - Slice token number and whitespace that are in [0] and [1] of line.
      // - Join the rest as a string and trim the trailing quotes.
      const data = line.slice(2).join(',').slice(1, -1);
      this.tokens.set(tokenNumber, data);
    }
  }
}
