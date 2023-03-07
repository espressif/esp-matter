/***************************************************************************//**
 * @file
 * @brief I2C SPM examples functions
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#include <string.h>
#include <stdio.h>
#include "sl_i2cspm.h"
#include "sl_i2cspm_instances.h"
#include "sl_simple_led.h"
#include "sl_simple_led_instances.h"
#include "FreeRTOS.h"
#include "task.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

#ifndef I2CSPM_TASK_STACK_SIZE
#define I2CSPM_TASK_STACK_SIZE          256
#endif

#ifndef I2CSPM_TASK_PRIO
#define I2CSPM_TASK_PRIO                20
#endif

#ifndef EXAMPLE_USE_STATIC_ALLOCATION
#define EXAMPLE_USE_STATIC_ALLOCATION   1
#endif

// The limits are set relative to the initial temperature
#define TEMPERATURE_BAND_C               4

// Si7021 Configuration Settings
#define SI7021_I2C_DEVICE                (sl_i2cspm_sensor) /**< I2C device used to control the Si7021  */
#define SI7021_I2C_BUS_ADDRESS           0x40               /**< I2C bus address                        */
#define SI7021_DEVICE_ID                 0x15               /**< Si7021 device ID                       */

// Si7021 command macro definitions
#define SI7021_CMD_MEASURE_RH            0xE5               /**< Measure Relative Humidity, Hold Master Mode */
#define SI7021_CMD_MEASURE_RH_NO_HOLD    0xF5               /**< Measure Relative Humidity, No Hold Master Mode */
#define SI7021_CMD_MEASURE_TEMP          0xE3               /**< Measure Temperature, Hold Master Mode */
#define SI7021_CMD_MEASURE_TEMP_NO_HOLD  0xF3               /**< Measure Temperature, No Hold Master Mode */
#define SI7021_CMD_READ_TEMP             0xE0               /**< Read Temperature Value from Previous RH Measurement */
#define SI7021_CMD_RESET                 0xFE               /**< Reset */
#define SI7021_CMD_WRITE_USER_REG1       0xE6               /**< Write RH/T User Register 1 */
#define SI7021_CMD_READ_USER_REG1        0xE7               /**< Read RH/T User Register 1 */
#define SI7021_CMD_WRITE_HEATER_CTRL     0x51               /**< Write Heater Control Register */
#define SI7021_CMD_READ_HEATER_CTRL      0x11               /**< Read Heater Control Register */
#define SI7021_CMD_READ_ID_BYTE1         { 0xFA, 0x0F }       /**< Read Electronic ID 1st Byte */
#define SI7021_CMD_READ_ID_BYTE2         { 0xFC, 0xC9 }       /**< Read Electronic ID 2nd Byte */
#define SI7021_CMD_READ_FW_REV           { 0x84, 0xB8 }       /**< Read Firmware Revision */

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/

static uint32_t relative_humidity;
static int32_t temperature;
static int32_t start_temperature;
static int32_t high_limit, low_limit;

/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/

static void i2cspm_task(void *arg);

/***************************************************************************//**
 * Function to perform I2C transactions on the Si7021
 *
 * This function is used to perform I2C transactions on the Si7021
 * including read, write and continued write read operations.
 ******************************************************************************/
static I2C_TransferReturn_TypeDef SI7021_transaction(uint16_t flag,
                                                     uint8_t *writeCmd,
                                                     size_t writeLen,
                                                     uint8_t *readCmd,
                                                     size_t readLen)
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;

  seq.addr = SI7021_I2C_BUS_ADDRESS << 1;
  seq.flags = flag;

  switch (flag) {
    // Send the write command from writeCmd
    case I2C_FLAG_WRITE:
      seq.buf[0].data = writeCmd;
      seq.buf[0].len  = writeLen;

      break;

    // Receive data into readCmd of readLen
    case I2C_FLAG_READ:
      seq.buf[0].data = readCmd;
      seq.buf[0].len  = readLen;

      break;

    // Send the write command from writeCmd
    // and receive data into readCmd of readLen
    case I2C_FLAG_WRITE_READ:
      seq.buf[0].data = writeCmd;
      seq.buf[0].len  = writeLen;

      seq.buf[1].data = readCmd;
      seq.buf[1].len  = readLen;

      break;

    default:
      return i2cTransferUsageFault;
  }

  // Perform the transfer and return status from the transfer
  ret = I2CSPM_Transfer(SI7021_I2C_DEVICE, &seq);

  return ret;
}

/***************************************************************************//**
 * Function to decode Relative Humidity from the read value
 *
 * This function decodes RH data using the formula provided in the Si7021
 * datasheet. Returns the value in Percent.
 ******************************************************************************/
static uint32_t decode_rh(uint8_t* read_register)
{
  uint32_t rhValue;

  // Formula to decode read RH from the Si7021 Datasheet
  rhValue = ((uint32_t) read_register[0] << 8) + (read_register[1] & 0xfc);
  rhValue = (((rhValue) * 125) >> 16) - 6;

  return rhValue;
}

/***************************************************************************//**
 * Function to decode Temperature from the read Temperature value
 *
 * This function decodes Temperature using the formula provided in the Si7021
 * datasheet. Returns the value in Celsius.
 ******************************************************************************/
static uint32_t decode_temp(uint8_t* read_register)
{
  uint32_t tempValue;
  float actual_temp;
  uint32_t rounded_temp;

  // Formula to decode read Temperature from the Si7021 Datasheet
  tempValue = ((uint32_t) read_register[0] << 8) + (read_register[1] & 0xfc);
  actual_temp = (((tempValue) * 175.72) / 65536) - 46.85;

  // Round the temperature to an integer value
  rounded_temp = (uint32_t)(actual_temp + 0.5 - (actual_temp < 0));

  return rounded_temp;
}

/***************************************************************************//**
 * Function to measure humidity and temperature from the Si7021
 *
 * This function measures current humidity and temperature using the Si7021,
 * returning values using the pointer inputs.
 ******************************************************************************/
static void SI7021_measure(uint32_t *rhData, int32_t *tData)
{
  I2C_TransferReturn_TypeDef ret;
  uint8_t cmd;
  uint8_t readData[2];
  uint32_t timeout;

  // Start no-hold measurement by writing command to si7021
  cmd = SI7021_CMD_MEASURE_RH_NO_HOLD;
  ret = SI7021_transaction(I2C_FLAG_WRITE, &cmd, 2, NULL, 0);
  EFM_ASSERT(ret == i2cTransferDone);

  // Wait for data to become ready
  timeout = 10;
  while (timeout--) {
    ret = SI7021_transaction(I2C_FLAG_READ, NULL, 0, readData, 2);
    if (ret == i2cTransferDone) {
      break;
    } else if (ret == i2cTransferNack) {
      // Si7021 returns a Nack if data not ready
      const TickType_t xDelay = pdMS_TO_TICKS(10);
      vTaskDelay(xDelay);
    }
  }

  EFM_ASSERT(timeout > 0);

  // Data is ready, decode the RH value
  *rhData = decode_rh(readData);

  // Read the temperature measured during RH measurement
  cmd = SI7021_CMD_READ_TEMP;
  ret = SI7021_transaction(I2C_FLAG_WRITE_READ, &cmd, 1, readData, 2);
  EFM_ASSERT(ret == i2cTransferDone);

  *tData = decode_temp(readData);
}

/***************************************************************************//**
 * Helper function to Initialize the upper and lower limits
 *
 * This function sets the limits based on the temperature when the application
 * is started.
 ******************************************************************************/
static void initialise_temp_limits()
{
  // Get current temperature
  SI7021_measure(&relative_humidity, &start_temperature);

  // Set temperature limits based on initial temperature.
  low_limit = start_temperature - (TEMPERATURE_BAND_C / 2);
  high_limit = start_temperature + (TEMPERATURE_BAND_C / 2);
}

/***************************************************************************//**
 * Helper function to turn on led 0 or 1 based on current temperature
 *
 * This function turns on led 0 or 1 based on the current temperature and
 * lower and upper limits.
 ******************************************************************************/
static void set_leds(int32_t temp)
{
  if (temp > high_limit) {
    // For higher temperature, turn led0 on and turn led1 off
    sl_led_turn_on(&sl_led_led0);
    sl_led_turn_off(&sl_led_led1);
    printf("Turning LED0 on!\r\n");
  } else if (temp < low_limit) {
    // For lower temperature, turn led1 on and turn led0 off
    sl_led_turn_off(&sl_led_led0);
    sl_led_turn_on(&sl_led_led1);
    printf("Turning LED1 on!\r\n");
  }
}

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * Initialize i2cspm example.
 ******************************************************************************/
void i2cspm_init(void)
{
  TaskHandle_t xHandle = NULL;

#if (EXAMPLE_USE_STATIC_ALLOCATION == 1)

  static StaticTask_t xTaskBuffer;
  static StackType_t  xStack[I2CSPM_TASK_STACK_SIZE];

  // Create I2CSPM Task without using any dynamic memory allocation
  xHandle = xTaskCreateStatic(i2cspm_task,
                              "i2cspm task",
                              I2CSPM_TASK_STACK_SIZE,
                              ( void * ) NULL,
                              tskIDLE_PRIORITY + 1,
                              xStack,
                              &xTaskBuffer);

  // Since puxStackBuffer and pxTaskBuffer parameters are not NULL,
  // it is impossible for xHandle to be null. This check is for
  // rigorous example demonstration.
  EFM_ASSERT(xHandle != NULL);

#else

  BaseType_t xReturned = pdFAIL;

  // Create Blink Task using dynamic memory allocation
  xReturned = xTaskCreate(i2cspm_task,
                          "i2cspm task",
                          I2CSPM_TASK_STACK_SIZE,
                          ( void * ) NULL,
                          tskIDLE_PRIORITY + 1,
                          &xHandle);

  // Unlike task creation using static allocation, dynamic task creation can very likely
  // fail due to lack of memory. Checking the return value is relevant.
  EFM_ASSERT(xReturned == pdPASS);

#endif
}

/*******************************************************************************
 * i2cspm task.
 ******************************************************************************/
static void i2cspm_task(void *arg)
{
  (void)&arg;

  I2C_TransferReturn_TypeDef ret;
  uint8_t cmdReadId2[2] = SI7021_CMD_READ_ID_BYTE2;
  uint8_t deviceId[8];
  TickType_t xDelay;

  // Wait for sensor to become ready
  xDelay = pdMS_TO_TICKS(80);
  vTaskDelay(xDelay);

  // Check for device presence  and compare device ID
  ret = SI7021_transaction(I2C_FLAG_WRITE_READ, cmdReadId2, 2, deviceId, 8);

  // Make sure transfer was successful
  EFM_ASSERT(ret == i2cTransferDone);

  // Check the Received Device ID
  EFM_ASSERT(deviceId[0] == SI7021_DEVICE_ID);

  // printf is configured to redirect to vcom in project file
  printf("\r\nWelcome to the I2C SPM sample application using FreeRTOS\r\n\r\n");

  // Get current temperature
  initialise_temp_limits();

  //Use the provided calculation macro to convert milliseconds to OS ticks
  xDelay = pdMS_TO_TICKS(1000);

  while (1) {
    //Wait for specified delay
    vTaskDelay(xDelay);

    // Measure the current humidity and temperature
    SI7021_measure(&relative_humidity, &temperature);

    // Print the current humidity and temperature to vcom
    printf("\r\n");
    printf("Relative Humidity = %ld%%\r\n", relative_humidity);
    printf("Temperature = %ld C\r\n", temperature);

    // set appropriate LEDs (led0 or 1) based on temperature
    set_leds(temperature);
  }
}
