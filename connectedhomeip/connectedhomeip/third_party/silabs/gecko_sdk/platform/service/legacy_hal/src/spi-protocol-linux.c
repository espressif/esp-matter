/***************************************************************************//**
 * @file
 * @brief Example host SPI Protocol implementation for interfacing with NCP.
 *
 *
 * SPI Protocol Interface:
 *
 * void halNcpSerialInit(void)
 * void halNcpSerialPowerup(void)
 * void halNcpSerialPowerdown(void)
 * EzspStatus halNcpHardReset(void)
 * EzspStatus halNcpHardResetReqBootload(bool requestBootload)
 * void halNcpWakeUp(void)
 * void halNcpSendCommand(void)
 * void halNcpSendRawCommand(void)
 * EzspStatus halNcpPollForResponse(void)
 * bool halNcpHasData(void)
 * void halNcpIsAwakeIsr(bool isAwake)
 * uint8_t *halNcpFrame
 * uint8_t halNcpSpipErrorByte
 * bool halNcpVerifySpiProtocolVersion(void)
 * bool halNcpVerifySpiProtocolActive(void)
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
#include PLATFORM_HEADER
#include "stack/include/error.h"
#include "hal/hal.h"
#include "spi-protocol-common.h"

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <netinet/in.h>
#include <net/if.h>
#include <poll.h>
#include <signal.h>
#include <semaphore.h>

#if defined(__linux__) || defined(__linux)
  #include <sys/ioctl.h>
  #include <linux/types.h>
  #include <linux/spi/spidev.h>
#else

// stub implementation

int ioctl(int fd, ...)
{
  return 0;
}

struct spi_ioc_transfer {
  unsigned long tx_buf;
  unsigned long rx_buf;
  int len;
  int delay_usecs;
  int speed_hz;
  int bits_per_word;
  bool cs_change;
};

  #define SPI_IOC_MESSAGE(x) 1
  #define SPI_IOC_WR_MODE 1
  #define SPI_IOC_RD_MODE 1
  #define SPI_IOC_WR_BITS_PER_WORD 1
  #define SPI_IOC_RD_BITS_PER_WORD 1
  #define SPI_IOC_WR_MAX_SPEED_HZ 1
  #define SPI_IOC_RD_MAX_SPEED_HZ 1

#endif

//Define some basic SPI Protocol parameters described in the NCP Datasheet
#define SPIP_VERSION_COMMAND            0x0A
#define SPIP_ALIVE_COMMAND              0x0B
#define SPIP_FRAME_TERMINATOR           0xA7
#define SPIP_NCP_WAS_RESET_RESPONSE     0x00
#define SPIP_DESIRED_VERSION_RESPONSE   0x82
#define SPIP_DESIRED_ALIVE_RESPONSE     0xC1
#define SPIP_BUFFER_SIZE                136
#define SPIP_MAX_PAYLOAD_FRAME_LENGTH   133
#define SPIP_EZSP_LENGTH_INDEX          1

#define NCP_SPI_BITS_PER_WORD         8

// The application can customize SPI and GPIO by definiging the below macros.
// Otherwise, common defaults are used.

#ifndef NCP_SPI_DEVICE
  #define NCP_SPI_DEVICE              "/dev/spidev0.0"
#endif

#ifndef NCP_SPI_MODE
  #define NCP_SPI_MODE                0
#endif

#ifndef NCP_SPI_SPEED_HZ
  #define NCP_SPI_SPEED_HZ            (1024 * 1024) // 1 MHz
#endif

#ifndef NCP_CHIP_SELECT_GPIO
  #define NCP_CHIP_SELECT_GPIO          "8"
#endif

#ifndef NCP_HOST_INT_GPIO
  #define NCP_HOST_INT_GPIO             "22"
#endif

#ifndef NCP_RESET_GPIO
  #define NCP_RESET_GPIO                "23"
#endif

#ifndef NCP_WAKE_GPIO
  #define NCP_WAKE_GPIO                 "24"
#endif

#ifndef NCP_RESET_DELAY_US
  #define NCP_RESET_DELAY_US            26
#endif

#define WAIT_SECTION_TIMEOUT_MS       350

#define WAKE_HANDSHAKE_TIMEOUT_MS     300

#define STARTUP_TIMEOUT_MS            7500

#define INTER_COMMAND_SPACING_MS      1

// Try the GPIO setup steps for a total of 10 mS each.  In our testing, this
// typically takes 1-2 mS.  Note:  if there is a failure, the code will log
// the failure reason and assert.
#define GPIO_SETUP_SLEEP_US            10
#define GPIO_SETUP_MAX_LOOP_COUNT    10000

static int ncpSpiFd = -1;
static int ncpChipSelectFd = -1;
static int ncpIntFd = -1;
static int ncpResetFd = -1;
static int ncpWakeFd = -1;

//To save RAM usage, define a single transaction buffer.  This buffer is
//filled by the EZSP for a Command, then overwritten with a Response.
uint8_t halNcpSpipBuffer[SPIP_BUFFER_SIZE];
//This error byte is the third byte found in a special SPI Protocol error case.
//It provides more detail concerning the error.  Refer to the NCP Datasheet.
uint8_t halNcpSpipErrorByte;
//Provide easy references to the buffer for EZSP.  This handle is
//made externally available (to EZSP) via the spi protocol header.
uint8_t *halNcpFrame = halNcpSpipBuffer + SPIP_EZSP_LENGTH_INDEX;

//The NCP mandates an inter-command spacing between the rising edge of nSSEL
//(end transaction) to the falling edge of nSSEL (start transaction).
//The inter-command spacing will be measured with a timer and an interrupt.
//Use a flag to indicate if OK to transmit (the interrupt will set the flag)
bool halNcpClearToTransmit = false;

//A simple flag that is either set or cleared on powerup to record if the
//NCP has data for the Host.  This flag is necessary because we cannot
//guarantee exiting sleep that the Host recorded an assertion of nHOST_INT.
bool halNcpHostIntAssertedOnPowerup = false;

//A globabl status for the SPI Protocol.  This status is used to track the
//current state of the SPI Protocol.  Specifically, we need to track state
//between sending a command and polling for a response.
EzspStatus spipStatus = EZSP_SPI_ERR_FATAL;

static uint8_t ncpTxBuffer[1];
static uint8_t ncpRxBuffer[1];

#define SPI_IOC_TRANSFERS 1
struct spi_ioc_transfer spiTransfer[SPI_IOC_TRANSFERS] = {
  {
    .tx_buf        = 0,
    .rx_buf        = 0,
    .len           = 1,
    .delay_usecs   = 0,
    .speed_hz      = NCP_SPI_SPEED_HZ,
    .bits_per_word = NCP_SPI_BITS_PER_WORD,
    .cs_change     = true,// Tells some SPI drivers not to override GPIO output.
  }
};

static uint32_t traceMask = 0xFF;
static bool logEnabled = false; // set by halNcpEnableLogging() if log file is opened
static bool doFlush = true;
static FILE *logFile = NULL;

typedef enum {
  RSVD_LOG_CLIENT_RX,
  RSVD_LOG_CLIENT_TX,
  LOG_SPI_RX,
  LOG_SPI_TX,
  LOG_SPI_LOGIC,
  LOG_MAX_TYPE  // Must be last
} LogEvent;

static const char * const logEventNames[] = {
  "RSVD_LOG_CLIENT_RX",
  "RSVD_LOG_CLIENT_TX",
  "SPI-RX",
  "SPI-TX",
  "SPI-LOG",
};

static uint32_t lastLogEventTime;

static void logTimestamp(void);
static void logEvent(LogEvent type, char *subType, uint8_t *data, uint16_t length);
static void logStatus(char *format, ...);

static void sleepDelayMicroseconds(int delay)
{
  assert(delay > 0);

  struct timeval timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec = delay;
  assert(select(0, NULL, NULL, NULL, &timeout) >= 0);
}

sem_t *spiSemaphore = NULL;
bool semaphoreLocked = false;

static void lockSemaphore(void);
static void unlockSemaphore(void);

bool halNcpOpenSemaphore(char *name, bool create)
{
  if (name == NULL) {
    return false;
  }

  const char *action = create ? "create" : "open existing";
  logStatus("Attempting to %s semaphore \"%s\"", action, name);

  if (create) {
    if (sem_unlink(name) == 0) {
      logStatus("unlinked previous semaphore");
    } else if (errno == ENOENT) {
      logStatus("no previous semaphore to unlink");
    } else {
      logStatus("sem_unlink failed with errno %d (%s)", errno, strerror(errno));
      return false;
    }

    mode_t prevMask = umask(0);

    spiSemaphore = sem_open(name, O_CREAT | O_EXCL, S_IRWXU, 1);

    umask(prevMask);
  } else {
    spiSemaphore = sem_open(name, 0);
  }

  if (spiSemaphore == SEM_FAILED) {
    logStatus("sem_open failed with errno %d (%s)", errno, strerror(errno));
    return false;
  }

  logStatus("Semaphore \"%s\" open", name);
  return true;
}

bool halNcpCloseSemaphore(void)
{
  if (spiSemaphore != NULL) {
    unlockSemaphore();

    if (sem_close(spiSemaphore) != 0) {
      perror("Semaphore could not be closed");
      logStatus("sem_close failed with errno %d (%s)", errno, strerror(errno));
      return false;
    }
  }

  return true;
}

static void lockSemaphore(void)
{
  if (spiSemaphore == NULL || semaphoreLocked) {
    return;
  }

  if (sem_trywait(spiSemaphore) == 0) {
    semaphoreLocked = true;
  } else {
    if (errno == EAGAIN) {
      logStatus("Waiting for locked semaphore");
      if (sem_wait(spiSemaphore) == 0) {
        logStatus("Semaphore obtained");
        semaphoreLocked = true;
      } else {
        perror("Unable to lock semaphore");
        logStatus("sem_wait failed with errno %d (%s)", errno, strerror(errno));
      }
    } else {
      perror("Unable to lock semaphore");
      logStatus("sem_trywait failed with errno %d (%s)", errno, strerror(errno));
    }
  }
}

static void unlockSemaphore(void)
{
  if (spiSemaphore == NULL || !semaphoreLocked) {
    return;
  }

  if (sem_post(spiSemaphore) == 0) {
    semaphoreLocked = false;
  } else {
    perror("Unable to unlock semaphore");
    logStatus("sem_post failed with errno %d (%s)", errno, strerror(errno));
  }
}

//------------------------------------------------------------------------------
// Serial connection to NCP.

static uint8_t spiWriteRead(uint8_t data)
{
  ncpTxBuffer[0] = data;

  logEvent(LOG_SPI_TX, NULL, ncpTxBuffer, 1);
  assert(ioctl(ncpSpiFd, SPI_IOC_MESSAGE(SPI_IOC_TRANSFERS), spiTransfer) >= 0);
  logEvent(LOG_SPI_RX, NULL, ncpRxBuffer, 1);

  return ncpRxBuffer[0];
}

static void interTransactionDelay(void)
{
  sleepDelayMicroseconds(INTER_COMMAND_SPACING_MS * 1000);
}

typedef enum {
  DIR_INPUT,
  DIR_OUTPUT
} GpioDir;

#define DEV_STRING_MAX   256
#define GPIO_STRING_MAX  256

static void exportGpioDevice(const char *gpioNumber)
{
  int fd = -1;
  int length = strlen(gpioNumber);

  fd = open("/sys/class/gpio/export", O_WRONLY);

  // If setting up the GPIO fails, there is no way to recover.  Alert the
  // developer.
  if (fd == -1) {
    logStatus("Cannot open /sys/class/gpio/export.");
    assert(0);
  }

  // If the GPIOs are already configured, this write will fail.  Do not need to
  // assert, as there are valid reasons for this to occur.
  if (write(fd, gpioNumber, length) <= 0) {
    logStatus("Cannot write to /sys/class/gpio/export.");
  }

  close(fd);
}

static void directionGpioDevice(const char *gpioNumber, const char *direction)
{
  int fd = -1;
  char filename[GPIO_STRING_MAX];
  int length, loopCount = 0;

  length = strlen(direction);

  sprintf(filename, "/sys/class/gpio/gpio%s/direction", gpioNumber);

  while (fd == -1 && loopCount < GPIO_SETUP_MAX_LOOP_COUNT) {
    fd = open(filename, O_WRONLY);
    sleepDelayMicroseconds(GPIO_SETUP_SLEEP_US);
    loopCount++;
  }

  // If setting up the GPIO fails, there is no way to recover.  Alert the
  // developer.
  if (fd == -1) {
    logStatus("Cannot open %s.", filename);
    assert(0);
  }
  if (write(fd, direction, length) <= 0) {
    logStatus("Cannot write to %s.", filename);
    assert(0);
  }

  close(fd);
}

static void edgeGpioDevice(const char *gpioNumber, const char *edge)
{
  int fd = -1;
  char filename[GPIO_STRING_MAX];
  int length, loopCount = 0;

  length = strlen(edge);

  sprintf(filename, "/sys/class/gpio/gpio%s/edge", gpioNumber);

  while (fd == -1 && loopCount < GPIO_SETUP_MAX_LOOP_COUNT) {
    fd = open(filename, O_WRONLY);
    sleepDelayMicroseconds(GPIO_SETUP_SLEEP_US);
    loopCount++;
  }

  // If setting up the GPIO fails, there is no way to recover.  Alert the
  // developer.
  if (fd == -1) {
    logStatus("Cannot open %s.", filename);
    assert(0);
  }
  if (write(fd, edge, length) <= 0) {
    logStatus("Cannot write to %s.", filename);
    assert(0);
  }

  close(fd);
}

static int openGpioDevice(const char *gpioName, const char *displayName, GpioDir dir)
{
  int fd = -1;
  if (gpioName && *gpioName) {
    char gpioDevicePath[GPIO_STRING_MAX + 1] = "/sys/class/gpio/gpio";
    if (gpioName[0] == '/') { // Full path on command line overrides
      strncpy(gpioDevicePath, gpioName, sizeof(gpioDevicePath) - 1);
    } else {
      strncat(gpioDevicePath, gpioName, sizeof(gpioDevicePath) - strlen(gpioDevicePath) - 1);
      strncat(gpioDevicePath, "/value", sizeof(gpioDevicePath) - strlen(gpioDevicePath) - 1);
    }
    fd = open(gpioDevicePath, (dir == DIR_INPUT ? O_RDONLY : O_WRONLY) /* | O_NONBLOCK */);  //FIXME: Need NONBLOCK?
    if (fd < 0) {
      logStatus("Cannot open %s device %s.", displayName, gpioName);
      perror("gpio open failed");
      exit(1);
    }
    logStatus("Opened %s device %s.", displayName, gpioName);
  }
  return fd;
}

static void openNcpChipSelect(const char *chipSelectGpio)
{
  ncpChipSelectFd = openGpioDevice(chipSelectGpio, "nCS", DIR_OUTPUT);
}

static void openNcpReset(const char *resetGpio)
{
  ncpResetFd = openGpioDevice(resetGpio, "nRESET", DIR_OUTPUT);
}

static void openNcpWake(const char *wakeGpio)
{
  ncpWakeFd = openGpioDevice(wakeGpio, "nWAKE", DIR_OUTPUT);
}

static void openNcpHostInterrupt(const char *hostIntGpio)
{
  ncpIntFd = openGpioDevice(hostIntGpio, "nHOST_INT", DIR_INPUT);
}

static void openNcpSpiSerial(const char *spiDevice)
{
  ncpSpiFd = open(spiDevice, O_RDWR /* | O_NONBLOCK */);  //FIXME: Need NONBLOCK?
  if (ncpSpiFd < 0) {
    logStatus("Cannot open SPI device %s.", spiDevice);
    perror("spi open failed");
    exit(1);
  }
  logStatus("Opened SPI device %s.", spiDevice);

  // SPI mode
  uint8_t mode = NCP_SPI_MODE;
  assert(ioctl(ncpSpiFd, SPI_IOC_WR_MODE, &mode) >= 0);
  assert(ioctl(ncpSpiFd, SPI_IOC_RD_MODE, &mode) >= 0);
  logEvent(LOG_SPI_LOGIC, "SPI-Mode", (uint8_t*)&mode, sizeof(mode));
  uint8_t bits = NCP_SPI_BITS_PER_WORD;
  assert(ioctl(ncpSpiFd, SPI_IOC_WR_BITS_PER_WORD, &bits) >= 0);
  assert(ioctl(ncpSpiFd, SPI_IOC_RD_BITS_PER_WORD, &bits) >= 0);
  logEvent(LOG_SPI_LOGIC, "SPI-Bits", (uint8_t*)&bits, sizeof(bits));
  uint32_t speed = NCP_SPI_SPEED_HZ;
  assert(ioctl(ncpSpiFd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) >= 0);
  assert(ioctl(ncpSpiFd, SPI_IOC_RD_MAX_SPEED_HZ, &speed) >= 0);
  logEvent(LOG_SPI_LOGIC, "SPI-Speed", (uint8_t*)&speed, sizeof(speed));
}

static void halNcpAssertChipSelect(void)
{
  assert(ncpChipSelectFd >= 0);

  uint8_t gpioPinState = '0';
  (void) write(ncpChipSelectFd, &gpioPinState, 1);
}

static void halNcpDeassertChipSelect(void)
{
  assert(ncpChipSelectFd >= 0);

  uint8_t gpioPinState = '1';
  (void) write(ncpChipSelectFd, &gpioPinState, 1);
}

static void halNcpAssertWake(void)
{
  assert(ncpWakeFd >= 0);

  uint8_t gpioPinState = '0';
  (void) write(ncpWakeFd, &gpioPinState, 1);
}

static void halNcpDeassertWake(void)
{
  assert(ncpWakeFd >= 0);

  uint8_t gpioPinState = '1';
  (void) write(ncpWakeFd, &gpioPinState, 1);
}

static void halNcpAssertReset(void)
{
  assert(ncpResetFd >= 0);

  uint8_t gpioPinState = '0';
  (void) write(ncpResetFd, &gpioPinState, 1);
}

static void halNcpDeassertReset(void)
{
  assert(ncpResetFd >= 0);

  uint8_t gpioPinState = '1';
  (void) write(ncpResetFd, &gpioPinState, 1);
}

static bool ncpHostIntAsserted(void)
{
  assert(ncpIntFd >= 0);

  uint8_t gpioPinState = '1'; // Assume deasserted, is low-true signal
  (void) lseek(ncpIntFd, 0, SEEK_SET);
  (void) read(ncpIntFd, &gpioPinState, 1);
  logEvent(LOG_SPI_LOGIC, "INT=", &gpioPinState, sizeof(gpioPinState));
  return(gpioPinState == '0');
}

static bool waitForHostInterruptWithMsTimeout(int milliseconds)
{
  bool result = ncpHostIntAsserted();

  if (!result) {
    struct pollfd pollFdArray = { 0 };

    pollFdArray.fd = ncpIntFd;
    pollFdArray.events = POLLPRI | POLLERR;

    int numEvents = poll(&pollFdArray, 1, milliseconds);

    assert(numEvents >= 0);

    result = ncpHostIntAsserted() || numEvents;
  }

  return result;
}

void halNcpSerialInit(void)
{
  char spiDevArg[] = NCP_SPI_DEVICE;
  char chipSelectGpioArg[] = NCP_CHIP_SELECT_GPIO;
  char intGpioArg[] = NCP_HOST_INT_GPIO;
  char resetGpioArg[] = NCP_RESET_GPIO;
  char wakeGpioArg[] = NCP_WAKE_GPIO;

  // Setup sysfs GPIO entries.  Note:  it takes time for the OS to create these
  // so we need to wait in the open funcitons.
  exportGpioDevice(chipSelectGpioArg);
  exportGpioDevice(intGpioArg);
  exportGpioDevice(resetGpioArg);
  exportGpioDevice(wakeGpioArg);

  // Note:  it takes time for the OS to create the GPIO structures, so we need
  // to wait here in the open statements.
  directionGpioDevice(chipSelectGpioArg, "high");
  directionGpioDevice(intGpioArg, "in");
  edgeGpioDevice(intGpioArg, "falling");
  directionGpioDevice(resetGpioArg, "high");
  directionGpioDevice(wakeGpioArg, "high");

  // Now, we can open the GPIO structures.
  openNcpHostInterrupt(intGpioArg);

  openNcpSpiSerial(spiDevArg);

  openNcpChipSelect(chipSelectGpioArg);
  halNcpDeassertChipSelect();

  openNcpReset(resetGpioArg);
  halNcpDeassertReset();

  openNcpWake(wakeGpioArg);
  halNcpDeassertWake();

  spiTransfer[0].tx_buf = (uint64_t)ncpTxBuffer;
  spiTransfer[0].rx_buf = (uint64_t)ncpRxBuffer;
}

void halNcpSerialPowerup(void)
{
  //check to see if nHOST_INT is already asserted and record the state
  halNcpHostIntAssertedOnPowerup = ncpHostIntAsserted();
}

//void halNcpSerialPowerdown(void)
//{
//  // stub, we don't currently support sleepy POSIX hosts
//}

EzspStatus halNcpHardResetReqBootload(bool requestBootload)
{
  // lock the semaphore, it will be released after the timeout or when the first
  // response reception finishes
  lockSemaphore();

  //reset the NCP and wait for it to come back.
  halNcpAssertReset();
  //the state of the nWAKE pin while the NCP is booting asks the NCP to
  //either enter the bootloader (nWAKE is low) or continue like normal
  if (requestBootload) {
    //request bootloader
    halNcpAssertWake();
  } else {
    //continue with normal boot
    halNcpDeassertWake();
  }

  sleepDelayMicroseconds(NCP_RESET_DELAY_US);

  halNcpClearToTransmit = true;

  halNcpDeassertReset();

  halNcpHostIntAssertedOnPowerup = false;

  if (!waitForHostInterruptWithMsTimeout(STARTUP_TIMEOUT_MS)) {
    halNcpDeassertWake();
    logEvent(LOG_SPI_LOGIC, "halNcpHardResetReqBootload failed: no HOST_INT", NULL, 0);
    unlockSemaphore();
    return EZSP_SPI_ERR_STARTUP_TIMEOUT;
  }

  //make sure nWAKE is back to idle
  halNcpDeassertWake();

  //the first check better return false due to the NCP resetting
  if (halNcpVerifySpiProtocolActive() != false
      || spipStatus != EZSP_SPI_ERR_NCP_RESET) {
    logEvent(LOG_SPI_LOGIC, "halNcpHardResetReqBootload failed: did not receive reset frame", NULL, 0);
    return EZSP_SPI_ERR_STARTUP_FAIL;
  }
  //the second check better return true indicating the SPIP is active
  if (halNcpVerifySpiProtocolActive() != true) {
    logEvent(LOG_SPI_LOGIC, "halNcpHardResetReqBootload failed: NCP not active", NULL, 0);
    return EZSP_SPI_ERR_STARTUP_FAIL;
  }
  //the third check better return true indicating the proper SPIP version
  if (halNcpVerifySpiProtocolVersion() != true) {
    logEvent(LOG_SPI_LOGIC, "halNcpHardResetReqBootload failed: SPIP version mismatch", NULL, 0);
    return EZSP_SPI_ERR_STARTUP_FAIL;
  }

  return EZSP_SUCCESS;
}

EzspStatus halNcpHardReset(void)
{
  return halNcpHardResetReqBootload(false);
}

void halNcpWakeUp(void)
{
  bool ncpIsAwake = false;

  if (ncpHostIntAsserted() || halNcpHostIntAssertedOnPowerup) {
    ncpIsAwake = true; // NCP is already awake
  } else {
    halNcpAssertWake();
    ncpIsAwake = waitForHostInterruptWithMsTimeout(WAKE_HANDSHAKE_TIMEOUT_MS);
    halNcpDeassertWake();

    halNcpHostIntAssertedOnPowerup = false;
  }

  halNcpClearToTransmit = true;
  halNcpIsAwakeIsr(ncpIsAwake); // inform upper layers
}

//This function makes no assumption about the data in the SpipBuffer, it will
//just faithly try to perform the transaction
void halNcpSendRawCommand(void)
{
  uint8_t i;
  uint8_t length;
  uint8_t payloadLength;
  spipStatus = EZSP_SPI_ERR_FATAL;

  lockSemaphore();

  if (!halNcpClearToTransmit) {
    interTransactionDelay();
  }

  //start the transaction
  halNcpAssertChipSelect();

  //the next command is not clear to send
  halNcpClearToTransmit = false;

  //determine the length of the Command
  if (halNcpSpipBuffer[0] == 0xFE) { //EZSP payload
    payloadLength = halNcpSpipBuffer[1];
    length = payloadLength + 2;
  } else if (halNcpSpipBuffer[0] == 0xFD) { //Bootloader payload
    payloadLength = halNcpSpipBuffer[1];
    length = payloadLength + 2;
  } else {
    payloadLength = 1;
    length = 1;
  }
  //guard against oversized payloads which could cause problems
  if (payloadLength > SPIP_MAX_PAYLOAD_FRAME_LENGTH) {
    spipStatus = EZSP_SPI_ERR_EZSP_COMMAND_OVERSIZED;
  } else {
    spipStatus = EZSP_SUCCESS; //the command can now be marked as successful
    for (i = 0; i < length; i++) {
      spiWriteRead(halNcpSpipBuffer[i]);
    }
    //finish Command with the Frame Terminator
    spiWriteRead(SPIP_FRAME_TERMINATOR);

    halNcpHostIntAssertedOnPowerup = false;
  }
}

static void endNcpPoll(EzspStatus status)
{
  spipStatus = status;

  halNcpHostIntAssertedOnPowerup = false;

  // end transaction
  halNcpDeassertChipSelect();

  logEvent(LOG_SPI_LOGIC, "endNcpPoll", (uint8_t*)&status, sizeof(status));

  // wait inter-command spacing
  interTransactionDelay();

  halNcpClearToTransmit = true;

  unlockSemaphore();
}

EzspStatus halNcpPollForResponse(void)
{
  int i;
  uint8_t spipByte;

  //SendCommand failed because of an oversized command, return an error
  if (spipStatus == EZSP_SPI_ERR_EZSP_COMMAND_OVERSIZED) {
    endNcpPoll(EZSP_SPI_ERR_EZSP_COMMAND_OVERSIZED);
    return spipStatus;
  }

  spipStatus = EZSP_SPI_ERR_FATAL; //start from a fatal state
  halNcpSpipErrorByte = 0xFF;

  // check for nHOST_INT falling edge, keep waiting if not asserted
  // other hosts poll once and return EZSP_SPI_WAITING_FOR_RESPONSE if the
  // timer has not expired. if needed we could achieve this behavior by using
  // a background thread or possibly a POSIX timer (using CLOCK_MONOTONIC)
  if (!waitForHostInterruptWithMsTimeout(WAIT_SECTION_TIMEOUT_MS)) {
    //if we wait past our timeout period, return an error
    endNcpPoll(EZSP_SPI_ERR_WAIT_SECTION_TIMEOUT);
    return spipStatus;
  }

  //clock the SPI until we receive the actual response (spiByte!=0xFF)
  // first byte is always 0xFF
  spipByte = spiWriteRead(0xFF);

  for (i = 0; spipByte == 0xFF && i < WAIT_SECTION_TIMEOUT_MS; i++) {
    sleepDelayMicroseconds(1000);
    spipByte = spiWriteRead(0xFF);
  }

  //if there's no response, return an error
  if (spipByte == 0xFF) {
    endNcpPoll(EZSP_SPI_ERR_WAIT_SECTION_TIMEOUT);
    return spipStatus;
  }

  //determine the type of response and the length, then receive the rest
  if (spipByte < 0x05) { //other error conditions
    //error conditions mean there is a error byte to receive
    //record the Error Byte
    halNcpSpipErrorByte = spiWriteRead(0xFF);
    if (spipByte == SPIP_NCP_WAS_RESET_RESPONSE) {
      spipStatus = EZSP_SPI_ERR_NCP_RESET;
    } else if (spipByte == 0x01) {
      spipStatus = EZSP_SPI_ERR_OVERSIZED_EZSP_FRAME;
    } else if (spipByte == 0x02) {
      spipStatus = EZSP_SPI_ERR_ABORTED_TRANSACTION;
    } else if (spipByte == 0x03) {
      spipStatus = EZSP_SPI_ERR_MISSING_FRAME_TERMINATOR;
    } else if (spipByte == 0x04) {
      spipStatus = EZSP_SPI_ERR_UNSUPPORTED_SPI_COMMAND;
    }
  } else if ((spipByte == 0xFE) //normal EZSP payload
             || (spipByte == 0xFD)) {  //normal Bootloader payload
    halNcpSpipBuffer[0] = spipByte; //save the spipByte into the buffer
    halNcpSpipBuffer[1] = spiWriteRead(0xFF); //rx the length byte
    //guard against oversized messages which could cause serious problems
    if (halNcpSpipBuffer[1] > SPIP_MAX_PAYLOAD_FRAME_LENGTH) {
      endNcpPoll(EZSP_SPI_ERR_EZSP_RESPONSE_OVERSIZED);
      return spipStatus;
    }
    for (i = 2; i < halNcpSpipBuffer[1] + 2; i++) {
      halNcpSpipBuffer[i] = spiWriteRead(0xFF); //rx the message
    }
    spipStatus = EZSP_SUCCESS;
  } else if ((spipByte & 0xC0) == 0x80 ) {  //SPI Protocol Version Response
    halNcpSpipBuffer[0] = spipByte;
    spipStatus = EZSP_SUCCESS;
  } else if ((spipByte & 0xC0) == 0xC0 ) {  //SPI Protocol Status response
    halNcpSpipBuffer[0] = spipByte;
    //check for frame terminator
    spipStatus = EZSP_SUCCESS;
  }

  // if the frame seems valid so far, check for frame terminator
  if ( spipStatus != EZSP_SPI_ERR_FATAL
       && spiWriteRead(0xFF) != SPIP_FRAME_TERMINATOR) {
    spipStatus = EZSP_SPI_ERR_NO_FRAME_TERMINATOR;
  }

  endNcpPoll(spipStatus);
  return spipStatus;
}

void halNcpSendCommand(void)
{
  //The function halNcpSendCommand() is used by EZSP.  All other
  //calls use halNcpSendRawCommand() since it allows specifying the SPIP Byte.
  halNcpSpipBuffer[0] = 0xFE; //mark the Command an EZSP Frame
  halNcpSendRawCommand(); //call the raw SendCommand
}

bool halNcpHasData(void)
{
  //if nHOST_INT asserted on powerup, 260 has data
  if (halNcpHostIntAssertedOnPowerup) {
    halNcpHostIntAssertedOnPowerup = false;
    logEvent(LOG_SPI_LOGIC, "halNcpHasData on PWRUP", NULL, 0);
    return true;
  } else {
    //if nHOST_INT asserted normally, 260 has data
    return ncpHostIntAsserted();
  }
}

bool halNcpVerifySpiProtocolVersion(void)
{
  EzspStatus status = EZSP_SPI_WAITING_FOR_RESPONSE;
  //send the SPI Protocol Version request byte
  //and check it against our expected version response
  halNcpSpipBuffer[0] = SPIP_VERSION_COMMAND;
  halNcpSendRawCommand();
  while (status == EZSP_SPI_WAITING_FOR_RESPONSE) {
    status = halNcpPollForResponse();
  }

  uint8_t tmp = SPIP_DESIRED_VERSION_RESPONSE;

  logEvent(LOG_SPI_LOGIC, "halNcpVerifySpiProtocolVersion status", (uint8_t*)&status, sizeof(status));
  logEvent(LOG_SPI_LOGIC, "halNcpVerifySpiProtocolVersion received", (uint8_t*)&halNcpSpipBuffer[0], sizeof(halNcpSpipBuffer[0]));
  logEvent(LOG_SPI_LOGIC, "halNcpVerifySpiProtocolVersion expected", (uint8_t*)&tmp, 1);

  if ((status == EZSP_SUCCESS)
      && (halNcpSpipBuffer[0] == SPIP_DESIRED_VERSION_RESPONSE)) {
    return true;
  }

  return false;
}

bool halNcpVerifySpiProtocolActive(void)
{
  EzspStatus status = EZSP_SPI_WAITING_FOR_RESPONSE;
  //send the SPI Protocol Status request byte
  //and check it against the desired "alive" response
  halNcpSpipBuffer[0] = SPIP_ALIVE_COMMAND;
  halNcpSendRawCommand();
  while (status == EZSP_SPI_WAITING_FOR_RESPONSE) {
    status = halNcpPollForResponse();
  }

  uint8_t tmp = SPIP_DESIRED_ALIVE_RESPONSE;

  logEvent(LOG_SPI_LOGIC, "halNcpVerifySpiProtocolActive status", (uint8_t*)&status, sizeof(status));
  logEvent(LOG_SPI_LOGIC, "halNcpVerifySpiProtocolActive received", (uint8_t*)&halNcpSpipBuffer[0], sizeof(halNcpSpipBuffer[0]));
  logEvent(LOG_SPI_LOGIC, "halNcpVerifySpiProtocolActive expected", (uint8_t*)&tmp, 1);

  if ((status == EZSP_SUCCESS)
      && (halNcpSpipBuffer[0] == SPIP_DESIRED_ALIVE_RESPONSE)) {
    return true;
  }
  return false;
}

static uint32_t unixTimeToMilliseconds(struct timeval *tv)
{
  uint32_t now = (tv->tv_sec * 1000) + (tv->tv_usec / 1000);
  return now;
}

bool halNcpEnableLogging(char *fileName)
{
  logFile = fopen(fileName, "w");
  if (logFile == NULL) {
    return false;
  }
  logEnabled = true;
  logStatus("traceMask = 0x%02X", traceMask);
  return true;
}

bool halNcpEndLogging(void)
{
  if (logFile != NULL) {
    return fclose(logFile) == 0;
  }

  return true;
}

static void logTimestamp(void)
{
  struct timeval tv;
  struct tm *tm;
  char datetime[64];
  gettimeofday(&tv, NULL);
  tm = localtime(&tv.tv_sec);
  assert(tm != NULL);
  strftime(datetime, sizeof(datetime), "%Y-%m-%d %H:%M:%S", tm);
  uint32_t now = unixTimeToMilliseconds(&tv);
  uint32_t delta = elapsedTimeInt32u(lastLogEventTime, now);
  lastLogEventTime = now;
  if (delta == 0) {
    fprintf(logFile, "                                    ");
  } else {
    if (delta > 1000) {
      fprintf(logFile, "\n");
    }
    fprintf(logFile, "[%s.%03d +%4d.%03d] ",
            datetime, (int)(tv.tv_usec / 1000), delta / 1000, delta % 1000);
  }
  if (doFlush) {
    fflush(logFile);
  }
}

static void logEvent(LogEvent type, char *subType, uint8_t *data, uint16_t length)
{
  if (logEnabled) {
    if ((1 << type) & traceMask) {
      logTimestamp();
      fprintf(logFile, "[%s] [", logEventNames[type]);
      if (subType != NULL) {
        fprintf(logFile, "%s", subType);
      }
      uint16_t i;
      for (i = 0; i < length; i++) {
        fprintf(logFile, " %02X", data[i]);
      }
      fprintf(logFile, "]\n");
      if (doFlush) {
        fflush(logFile);
      }
    }
  }
}

static void logStatus(char *format, ...)
{
  if (logEnabled) {
    logTimestamp();
    fprintf(logFile, "[status] [");
    va_list ap;
    va_start(ap, format);
    vfprintf(logFile, format, ap);
    va_end(ap);
    fprintf(logFile, "]\n");
    if (doFlush) {
      fflush(logFile);
    }

    printf("[%s] ", __FILE__);
    va_start(ap, format);
    vfprintf(stdout, format, ap);
    va_end(ap);
    fprintf(stdout, "\n");
    fflush(stdout);
  }
}

int halNcpGetIntFd(void)
{
  return ncpIntFd;
}
