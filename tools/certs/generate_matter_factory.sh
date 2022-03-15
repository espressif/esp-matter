#!/bin/sh
# Script to generate the DACs and matter factory partition

# TODO: Lot of stuff here is hardcoded, and could be done better
# by using the arguments to the script or even better, switch to python

CHIP_CERT_TOOL="../../connectedhomeip/connectedhomeip/out/host/chip-cert gen-att-cert"
NVS_GENERATOR="$IDF_PATH/components/nvs_flash/nvs_partition_generator/nvs_partition_gen.py"

PAA_CERT_SUBJECT="ESP Matter PAA 01"
PAI_CERT_SUBJECT="ESP Matter PAI 01"
DAC_CERT_SUBJECT="ESP Matter DAC 01"

CERT_VALID_FROM="2022-01-01 14:23:43"
CERT_LIFETIME=4294967295

VENDOR_ID=FFF1
PRODUCT_ID=8000

OUT_DIR=out/$VENDOR_ID-$PRODUCT_ID
NAME_PREFIX="$OUT_DIR/ESP-Matter"
mkdir -p $OUT_DIR

# PAA_CERT_NAME="$NAME_PREFIX-PAA-Cert.pem"
PAA_CERT_NAME="../../connectedhomeip/connectedhomeip/credentials/test/attestation/Chip-Test-PAA-FFF1-Cert.pem"
PAI_CERT_NAME="$NAME_PREFIX-PAI-Cert.pem"
DAC_CERT_NAME="$NAME_PREFIX-DAC-Cert.pem"

PAI_CERT_NAME_DER="$NAME_PREFIX-PAI-Cert.der"
DAC_CERT_NAME_DER="$NAME_PREFIX-DAC-Cert.der"

# PAA_PRIVATE_KEY_NAME="$NAME_PREFIX-PAA-Private-Key.pem"
PAA_PRIVATE_KEY_NAME="../../connectedhomeip/connectedhomeip/credentials/test/attestation/Chip-Test-PAA-FFF1-Key.pem"
PAI_PRIVATE_KEY_NAME="$NAME_PREFIX-PAI-Private-Key.pem"
DAC_PRIVATE_KEY_NAME="$NAME_PREFIX-DAC-Private-Key.pem"

DAC_PRIVATE_KEY_BIN="$NAME_PREFIX-DAC-Private-Key.bin"
DAC_PUBLIC_KEY_BIN="$NAME_PREFIX-DAC-Public-Key.bin"

# @brief Convert certificates in PEM format to DER format
#
# @param $1 - PEM Certificate File Name
# @param $2 - DER Certificate File Name
convert_pem_to_der()
{
    openssl x509 -inform pem -in $1 -out $2 -outform DER > /dev/null 2>&1
}

generate_paa()
{
    $CHIP_CERT_TOOL --type a \
                    --subject-cn "$PAA_CERT_SUBJECT" \
                    --valid-from "$CERT_VALID_FROM" \
                    --lifetime "$CERT_LIFETIME" \
                    --out-key "$PAA_PRIVATE_KEY_NAME" \
                    --out "$PAA_CERT_NAME"

    if [ $? -ne 0 ]; then
        echo "Failed to generate PAA certificate"
        exit 1
    fi

    echo "Generated PAA certificate"
}

generate_pai()
{
    $CHIP_CERT_TOOL --type i \
                    --subject-cn "$PAI_CERT_SUBJECT" \
                    --subject-vid "$VENDOR_ID" \
                    --valid-from "$CERT_VALID_FROM" \
                    --lifetime "$CERT_LIFETIME" \
                    --ca-key "$PAA_PRIVATE_KEY_NAME" \
                    --ca-cert "$PAA_CERT_NAME" \
                    --out-key "$PAI_PRIVATE_KEY_NAME" \
                    --out "$PAI_CERT_NAME"

    if [ $? -ne 0 ]; then
        echo "Failed to generate PAA certificate"
        exit 1
    fi

    convert_pem_to_der $PAI_CERT_NAME $PAI_CERT_NAME_DER
    echo "Generated PAI certificate"
}

generate_keypair_bin()
{
    openssl ec -in $DAC_PRIVATE_KEY_NAME | openssl asn1parse | awk -F ":" '/OCTET STRING/ { print $4 }' \
        | xxd -r -p > $DAC_PRIVATE_KEY_BIN

    openssl ec -inform PEM -in $DAC_PRIVATE_KEY_NAME -text | grep -A 5 'pub:'  | sed "1 d" \
            | tr -d ':\n ' | xxd -r -p > $DAC_PUBLIC_KEY_BIN
}

generate_dac()
{
    $CHIP_CERT_TOOL --type d \
                    --subject-cn "$DAC_CERT_SUBJECT" \
                    --subject-vid "$VENDOR_ID" \
                    --subject-pid "$PRODUCT_ID" \
                    --valid-from "$CERT_VALID_FROM" \
                    --lifetime "$CERT_LIFETIME" \
                    --ca-key "$PAI_PRIVATE_KEY_NAME" \
                    --ca-cert "$PAI_CERT_NAME" \
                    --out-key "$DAC_PRIVATE_KEY_NAME" \
                    --out "$DAC_CERT_NAME"

    if [ $? -ne 0 ]; then
        echo "Failed to generate PAA certificate"
        exit 1
    fi

    generate_keypair_bin $DAC_PRIVATE_KEY_NAME $DAC_PRIVATE_KEY_BIN $DAC_PUBLIC_KEY_BIN 
    convert_pem_to_der $DAC_CERT_NAME $DAC_CERT_NAME_DER
    echo "Generated DAC certificate, key pair"
}

populate_csv()
{

cat << EOF >> $OUT_DIR/matter_factory.csv
key,type,encoding,value
chip_creds,namespace,,
dac_cert,file,binary,$DAC_CERT_NAME_DER
pai_cert,file,binary,$PAI_CERT_NAME_DER
dac_pubkey,file,binary,$DAC_PUBLIC_KEY_BIN
dac_privkey,file,binary,$DAC_PRIVATE_KEY_BIN
cert_dclrn,file,hex2bin,cert_dclrn.bin
EOF

}

print_flash_help()
{
    echo ""
    echo "=============================================================="
    echo "Generated matter factory partition:$1"
    echo "Please flash the partition to the device using esptool.py"
    echo "esptool.py -p <serial-port> write_flash $2 $1"
    echo "=============================================================="
}

generate_matter_factory_partition()
{
    populate_csv

    if [ $? -ne 0 ]; then
        echo "Failed to generate CSV file"
        exit 1
    fi

    $NVS_GENERATOR generate $OUT_DIR/matter_factory.csv $OUT_DIR/matter_factory.bin 0x6000

    if [ $? -ne 0 ]; then
        echo "Failed to generate matter factory partition"
        exit 1
    fi

    print_flash_help $OUT_DIR/matter_factory.bin 0x6000
}

main()
{
    generate_pai
    generate_dac
    generate_matter_factory_partition
}

# Execution starts here
main
