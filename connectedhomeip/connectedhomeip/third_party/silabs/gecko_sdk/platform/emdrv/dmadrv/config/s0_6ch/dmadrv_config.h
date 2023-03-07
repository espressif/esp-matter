#ifndef DMADRV_CONFIG_H
#define DMADRV_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <o EMDRV_DMADRV_DMA_IRQ_PRIORITY> DMA interrupt priority <0-3>
// <i> Priority of the DMA interrupt. Smaller number equals higher priority.
// <i> Default: 3
#define EMDRV_DMADRV_DMA_IRQ_PRIORITY 3

// <o EMDRV_DMADRV_DMA_CH_COUNT> Number of available channels <1-6>
// <i> Number of DMA channels supported by the driver. A lower channel count
// <i> will reduce RAM memory footprint. The default is to support all channels
// <i> on the device.
// <i> Default: 6
#define EMDRV_DMADRV_DMA_CH_COUNT 6

// <<< end of configuration section >>>

#endif // DMADRV_CONFIG_H
