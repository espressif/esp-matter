#!/bin/bash

if [ "$1" == "--help" ] || [ "$1" == "" ]; then
	echo "Test Serial Recovery."
	echo "Usage: $0 <board> <serial_port>"
	echo "  e.g. $0 nrf52840_pca10056 /dev/ttyACM0"
	echo ""
	echo "Will print SUCCESS or FAIL before exiting when running the test."
	echo "Will return an error code unless SUCCESS."
	exit -1
fi

build() {
	if [ -d "$1" ]; then rm -r $1; fi
	mkdir $1
	pushd $1
	cmake -GNinja $2
	ninja
	popd
}

do_test() {
	mcumgr --conntype=serial --connstring="$2,baud=115200" \
		image upload -e $1

	rc=$?;
	if [[ $rc != 0 ]]; then
		echo
		echo "FAIL"
		echo
		exit $rc;
	fi
}

if [ "$1" = "nrf52840dk_nrf52840" ]; then
	flash_offset=0xC200
elif [ "$1" = "nrf9160dk_nrf9160" ]; then
	flash_offset=0x10200
elif [ "$1" = "nrf5340dk_nrf5340_cpuapp" ]; then
	flash_offset=0x10200
fi

build "build_hello" "-DBOARD=$1 ${ZEPHYR_BASE}/samples/hello_world -DCONFIG_FLASH_LOAD_OFFSET=$flash_offset"

python3 ${ZEPHYR_BASE}/../bootloader/mcuboot/scripts/imgtool.py sign --key \
	${ZEPHYR_BASE}/../bootloader/mcuboot/root-rsa-2048.pem --header-size \
	0x200 --align 4 --version 1 --slot-size 0x10000 --pad-header \
	build_hello/zephyr/zephyr.bin build_hello/signed_hello.bin

build "build_mcuboot1" "-DBOARD=$1 ${ZEPHYR_BASE}/../bootloader/mcuboot/boot/zephyr\
	-DCONFIG_MCUBOOT_SERIAL=y -DCONFIG_UART_CONSOLE=n -DCONFIG_BOOT_SERIAL_DETECT_PIN_VAL=1 -DCONFIG_HW_STACK_PROTECTION=y"

pushd build_mcuboot1
nrfjprog --recover
ninja flash
popd

do_test build_hello/signed_hello.bin $2
# do_test build_hello/zephyr/zephyr.bin

build "build_mcuboot2" "-DBOARD=$1 ${ZEPHYR_BASE}/../bootloader/mcuboot/boot/zephyr\
	-DCONFIG_ZEPHYR_TRY_MASS_ERASE=n -DCONFIG_LOG=n"

pushd build_mcuboot2
ninja flash

echo -e "\n" > uart.log
while read -t 1 l< $2; do
	echo $l >> uart.log
done
cat uart.log
popd

if grep -F "Hello World!" build_mcuboot2/uart.log
then
	echo
	echo SUCCESS
	echo
	exit 0
else
	echo
	echo FAIL
	echo
	exit 1
fi
