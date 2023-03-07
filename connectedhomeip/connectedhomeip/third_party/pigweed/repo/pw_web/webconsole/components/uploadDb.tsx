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

import Button from '@mui/material/Button';
import {Alert} from '@mui/material';
import {useState, useRef} from 'react';

interface Props {
  onUpload: (db: string) => void
}

function testTokenDB(tokenCsv: string) {
  const lines = tokenCsv.trim().split(/\r?\n/).map(line => line.split(/,/));
  lines.forEach((line) => {
    // CSV has no columns or has malformed number.
    if (line.length < 2 || !/^[a-fA-F0-9]+$/.test(line[0])) {
      throw new Error("Not a valid token database.")
    }
  });
}

export default function BtnUploadDB({onUpload}: Props) {
  const [uploaded, setUploaded] = useState(false);
  const [error, setError] = useState("");
  const uploadInputRef = useRef<HTMLInputElement>(null);

  if (uploaded) return (<Alert severity="success">DB Loaded</Alert>)
  return (
    <>
      <input
        ref={uploadInputRef}
        type="file"
        accept="text/*"
        style={{display: "none"}}
        onChange={async e => {
          const tokenCsv = await readFile(e.target.files![0]);
          try {
            testTokenDB(tokenCsv);
            onUpload(tokenCsv);
            setUploaded(true);
            setError("");
          }
          catch (e: any) {
            if (e instanceof Error) setError(e.message);
            else setError("Error loading token database.");
          }
        }}
      />
      <Button
        onClick={() => uploadInputRef.current && uploadInputRef.current.click()}
        variant="contained">
        Upload token database
      </Button>
      {error && <Alert severity="error">{error}</Alert>}
    </>
  )
}

function readFile(file: Blob): Promise<string> {
  return new Promise((resolve, reject) => {
    if (!file) return resolve('');
    const reader = new FileReader();
    reader.onload = function (e) {
      resolve(String(e.target!.result));
    };
    reader.readAsText(file);
  });
}
