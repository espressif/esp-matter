## ESP-Matter OTA Provider

The OTA Provider will maintain a cache array of OTA candidates, which is used to store previous results of QueryImage command.

1. After receiving the QueryImage command from the OTA Requestor, the OTA Provider will handle the command asynchronously.

    a. If there is an existing backend command processing, the OTA provider will reply a response with Busy status.

2. The OTA Provider will look up the OTA candidates cache array to find whether there is an available update for the specific VendorID and ProductID in the command data.

    a. If there is already a candidate record for the specific VendorID and ProductID with valid SoftwareVersion, the OTA Provider will reply a UpdateAvailable reponse and start BDXTransfer.

    b. If there is no record for the specific VendorID, ProductID, and SoftwareVersion, the OTA Provider will try to fetch the candidate from the MainNet or TestNet DCL (Distributed Compliance Ledger).
       b1. If there is an error during candidate fetching, the OTA provider will reply a response with NotAvailable status.
       b2. If finishing candidate fetching, the OTA provider will reply a response with UpdateAvailable status and start BDXTransfer.

3. When the BDXTransfer of the OTA Provider receives a BDXInit message, it will establish an HTTP(S) connection to the URL of the OTA candidate and start downloading the image.

4. When the BDXTransfer of the OTA Provider receives a QueryBlock message, it will read the HTTP response for the HTTP(S) connection, prepare a Block message, and send it to the Requestor.\

Note: For the first QueryBlock message, the OTA Provider will verify the header of the image from the HTTP response.
