/**
 * Board initialization.
 *
 * @copyright 2021 Silicon Laboratories Inc.
 */

#include <board_init.h>
#include <board.h>
#include <zw_region_config.h>

void Board_Init(void)
{
  Board_Initialize();
  BRD420xBoardInit(ZW_REGION);
}
