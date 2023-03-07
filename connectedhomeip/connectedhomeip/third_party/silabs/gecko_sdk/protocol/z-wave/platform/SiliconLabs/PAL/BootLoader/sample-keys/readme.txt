This folder contains sample keys for signing and encrypting firmware images (GBL files).

It is important that the sample keys are not used for production. Each vendor should have 
his own set of keys. New kyes can be generated with simplicity commander by running the following
commands on the command line.

Creating new signing keys:
commander gbl keygen --type ecc-p256 -o manufacturer_sign.key

Creating new encryption keys:
commander gbl keygen --type aes-ccm -o manufacturer_encrypt.key

For OTA upgrade to work in a product the token file must be flashed to the device after
1) the bootloader has been flashed
2) the application has been flashed

commander flash --tokengroup znet --tokenfile manufacturer_encrypt.key --tokenfile manufacturer_sign.key-tokens.txt 

