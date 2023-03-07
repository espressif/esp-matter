To list curves:

   openssl ecparam -list_curves

To generate an ECDSA key:

   openssl ecparam -genkey -name secp384r1 -out k.pem

To print out the ECDSA key:

   openssl ec -in k.pem -noout -text


https://kjur.github.io/jsrsasign/sample/sample-ecdsa.html
https://superuser.com/questions/1103401/generate-an-ecdsa-key-and-csr-with-openssl
