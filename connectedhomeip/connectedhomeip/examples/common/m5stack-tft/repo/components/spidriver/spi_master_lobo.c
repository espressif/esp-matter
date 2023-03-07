// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


/*
----------------------------------------
Non DMA version of the spi_master driver
----------------------------------------
------------------------------------------------------------------------------------
Based on esp-idf 'spi_master', modified by LoBo (https://github.com/loboris) 03/2017
------------------------------------------------------------------------------------

* Transfers data to SPI device in direct mode, not using DMA
* All configuration options (bus, device, transaction) are the same as in spi_master driver
* Transfers uses the semaphore (taken in select function & given in deselect function) to protect the transfer
* Number of the devices attached to the bus which uses hardware CS can be 3 ('NO_CS')
* Additional devices which uses software CS can be attached to the bus, up to 'NO_DEV'
* 'spi_bus_initialize' & 'spi_bus_remove' functions are removed, spi bus is initiated/removed in spi_lobo_bus_add_device/spi_lobo_bus_remove_device when needed
* 'spi_lobo_bus_add_device' function has added parameter 'bus_config' and automatically initializes spi bus device if not already initialized
* 'spi_lobo_bus_remove_device' automatically removes spi bus device if no other devices are attached to it.
* Devices can have individual bus_configs, so different mosi, miso, sck pins can be configured for each device
    Reconfiguring the bus is done automaticaly in 'spi_lobo_device_select' function
* 'spi_lobo_device_select' & 'spi_lobo_device_deselect' functions handles devices configuration changes and software CS
* Some helper functions are added ('spi_lobo_get_speed', 'spi_lobo_set_speed', ...)
* All structures are available in header file for easy creation of user low level spi functions. See **tftfunc.c** source for examples.
* Transimt and receive lenghts are limited only by available memory


Main driver's function is 'spi_lobo_transfer_data()'

 * TRANSMIT 8-bit data to spi device from 'trans->tx_buffer' or 'trans->tx_data' (trans->lenght/8 bytes)
 * and RECEIVE data to 'trans->rx_buffer' or 'trans->rx_data' (trans->rx_length/8 bytes)
 * Lengths must be 8-bit multiples!
 * If trans->rx_buffer is NULL or trans->rx_length is 0, only transmits data
 * If trans->tx_buffer is NULL or trans->length is 0, only receives data
 * If the device is in duplex mode (LB_SPI_DEVICE_HALFDUPLEX flag NOT set), data are transmitted and received simultaneously.
 * If the device is in half duplex mode (LB_SPI_DEVICE_HALFDUPLEX flag IS set), data are received after transmission
 * 'address', 'command' and 'dummy bits' are transmitted before data phase IF set in device's configuration
 *   and IF 'trans->length' and 'trans->rx_length' are NOT both 0
 * If configured, devices 'pre_cb' callback is called before and 'post_cb' after the transmission
 * If device was not previously selected, it will be selected before transmission and deselected after transmission.

*/

/*
 Replace this include with
 #include "driver/spi_master_lobo.h"
 if the driver is located in esp-isf/components
*/
#include "freertos/FreeRTOS.h"
#include <string.h>
#include "soc/spi_reg.h"
#include "soc/dport_reg.h"
#include "esp_log.h"
#include "freertos/semphr.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "spi_master_lobo.h"
#include "driver/periph_ctrl.h"

static spi_lobo_host_t *spihost[3] = {NULL};


static const char *SPI_TAG = "spi_lobo_master";
#define SPI_CHECK(a, str, ret_val) \
    if (!(a)) { \
        ESP_LOGE(SPI_TAG,"%s(%d): %s", __FUNCTION__, __LINE__, str); \
        return (ret_val); \
    }

/*
 Stores a bunch of per-spi-peripheral data.
*/
typedef struct {
    const uint8_t spiclk_out;       //GPIO mux output signals
    const uint8_t spid_out;
    const uint8_t spiq_out;
    const uint8_t spiwp_out;
    const uint8_t spihd_out;
    const uint8_t spid_in;          //GPIO mux input signals
    const uint8_t spiq_in;
    const uint8_t spiwp_in;
    const uint8_t spihd_in;
    const uint8_t spics_out[3];     // /CS GPIO output mux signals
    const uint8_t spiclk_native;    //IO pins of IO_MUX muxed signals
    const uint8_t spid_native;
    const uint8_t spiq_native;
    const uint8_t spiwp_native;
    const uint8_t spihd_native;
    const uint8_t spics0_native;
    const uint8_t irq;              //irq source for interrupt mux
    const uint8_t irq_dma;          //dma irq source for interrupt mux
    const periph_module_t module;   //peripheral module, for enabling clock etc
    spi_dev_t *hw;                  //Pointer to the hardware registers
} spi_signal_conn_t;

/*
 Bunch of constants for every SPI peripheral: GPIO signals, irqs, hw addr of registers etc
*/
static const spi_signal_conn_t io_signal[3]={
    {
        .spiclk_out=SPICLK_OUT_IDX,
        .spid_out=SPID_OUT_IDX,
        .spiq_out=SPIQ_OUT_IDX,
        .spiwp_out=SPIWP_OUT_IDX,
        .spihd_out=SPIHD_OUT_IDX,
        .spid_in=SPID_IN_IDX,
        .spiq_in=SPIQ_IN_IDX,
        .spiwp_in=SPIWP_IN_IDX,
        .spihd_in=SPIHD_IN_IDX,
        .spics_out={SPICS0_OUT_IDX, SPICS1_OUT_IDX, SPICS2_OUT_IDX},
        .spiclk_native=6,
        .spid_native=8,
        .spiq_native=7,
        .spiwp_native=10,
        .spihd_native=9,
        .spics0_native=11,
        .irq=ETS_SPI1_INTR_SOURCE,
        .irq_dma=ETS_SPI1_DMA_INTR_SOURCE,
        .module=PERIPH_SPI_MODULE,
        .hw=&SPI1
    }, {
        .spiclk_out=HSPICLK_OUT_IDX,
        .spid_out=HSPID_OUT_IDX,
        .spiq_out=HSPIQ_OUT_IDX,
        .spiwp_out=HSPIWP_OUT_IDX,
        .spihd_out=HSPIHD_OUT_IDX,
        .spid_in=HSPID_IN_IDX,
        .spiq_in=HSPIQ_IN_IDX,
        .spiwp_in=HSPIWP_IN_IDX,
        .spihd_in=HSPIHD_IN_IDX,
        .spics_out={HSPICS0_OUT_IDX, HSPICS1_OUT_IDX, HSPICS2_OUT_IDX},
        .spiclk_native=14,
        .spid_native=13,
        .spiq_native=12,
        .spiwp_native=2,
        .spihd_native=4,
        .spics0_native=15,
        .irq=ETS_SPI2_INTR_SOURCE,
        .irq_dma=ETS_SPI2_DMA_INTR_SOURCE,
        .module=PERIPH_HSPI_MODULE,
        .hw=&SPI2
    }, {
        .spiclk_out=VSPICLK_OUT_IDX,
        .spid_out=VSPID_OUT_IDX,
        .spiq_out=VSPIQ_OUT_IDX,
        .spiwp_out=VSPIWP_OUT_IDX,
        .spihd_out=VSPIHD_OUT_IDX,
        .spid_in=VSPID_IN_IDX,
        .spiq_in=VSPIQ_IN_IDX,
        .spiwp_in=VSPIWP_IN_IDX,
        .spihd_in=VSPIHD_IN_IDX,
        .spics_out={VSPICS0_OUT_IDX, VSPICS1_OUT_IDX, VSPICS2_OUT_IDX},
        .spiclk_native=18,
        .spid_native=23,
        .spiq_native=19,
        .spiwp_native=22,
        .spihd_native=21,
        .spics0_native=5,
        .irq=ETS_SPI3_INTR_SOURCE,
        .irq_dma=ETS_SPI3_DMA_INTR_SOURCE,
        .module=PERIPH_VSPI_MODULE,
        .hw=&SPI3
    }
};


//======================================================================================================

#define DMA_CHANNEL_ENABLED(dma_chan)    (BIT(dma_chan-1))

typedef void(*dmaworkaround_cb_t)(void *arg);

//Set up a list of dma descriptors. dmadesc is an array of descriptors. Data is the buffer to point to.
//--------------------------------------------------------------------------------------------
void spi_lobo_setup_dma_desc_links(lldesc_t *dmadesc, int len, const uint8_t *data, bool isrx)
{
    int n = 0;
    while (len) {
        int dmachunklen = len;
        if (dmachunklen > SPI_MAX_DMA_LEN) dmachunklen = SPI_MAX_DMA_LEN;
        if (isrx) {
            //Receive needs DMA length rounded to next 32-bit boundary
            dmadesc[n].size = (dmachunklen + 3) & (~3);
            dmadesc[n].length = (dmachunklen + 3) & (~3);
        } else {
            dmadesc[n].size = dmachunklen;
            dmadesc[n].length = dmachunklen;
        }
        dmadesc[n].buf = (uint8_t *)data;
        dmadesc[n].eof = 0;
        dmadesc[n].sosf = 0;
        dmadesc[n].owner = 1;
        dmadesc[n].qe.stqe_next = &dmadesc[n + 1];
        len -= dmachunklen;
        data += dmachunklen;
        n++;
    }
    dmadesc[n - 1].eof = 1; //Mark last DMA desc as end of stream.
    dmadesc[n - 1].qe.stqe_next = NULL;
}


/*
Code for workaround for DMA issue in ESP32 v0/v1 silicon
*/


static volatile int dmaworkaround_channels_busy[2] = {0, 0};
static dmaworkaround_cb_t dmaworkaround_cb;
static void *dmaworkaround_cb_arg;
static portMUX_TYPE dmaworkaround_mux = portMUX_INITIALIZER_UNLOCKED;
static int dmaworkaround_waiting_for_chan = 0;
static bool spi_periph_claimed[3] = {true, false, false};
static uint8_t spi_dma_chan_enabled = 0;
static portMUX_TYPE spi_dma_spinlock = portMUX_INITIALIZER_UNLOCKED;

//--------------------------------------------------------------------------------------------
bool IRAM_ATTR spi_lobo_dmaworkaround_req_reset(int dmachan, dmaworkaround_cb_t cb, void *arg)
{
    int otherchan = (dmachan == 1) ? 2 : 1;
    bool ret;
    portENTER_CRITICAL(&dmaworkaround_mux);
    if (dmaworkaround_channels_busy[otherchan-1]) {
        //Other channel is busy. Call back when it's done.
        dmaworkaround_cb = cb;
        dmaworkaround_cb_arg = arg;
        dmaworkaround_waiting_for_chan = otherchan;
        ret = false;
    } else {
        //Reset DMA
        DPORT_SET_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_SPI_DMA_RST);
        DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_SPI_DMA_RST);
        ret = true;
    }
    portEXIT_CRITICAL(&dmaworkaround_mux);
    return ret;
}

//-------------------------------------------------------
bool IRAM_ATTR spi_lobo_dmaworkaround_reset_in_progress()
{
    return (dmaworkaround_waiting_for_chan != 0);
}

//-----------------------------------------------------
void IRAM_ATTR spi_lobo_dmaworkaround_idle(int dmachan)
{
    portENTER_CRITICAL(&dmaworkaround_mux);
    dmaworkaround_channels_busy[dmachan-1] = 0;
    if (dmaworkaround_waiting_for_chan == dmachan) {
        //Reset DMA
        DPORT_SET_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_SPI_DMA_RST);
        DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_SPI_DMA_RST);
        dmaworkaround_waiting_for_chan = 0;
        //Call callback
        dmaworkaround_cb(dmaworkaround_cb_arg);

    }
    portEXIT_CRITICAL(&dmaworkaround_mux);
}

//----------------------------------------------------------------
void IRAM_ATTR spi_lobo_dmaworkaround_transfer_active(int dmachan)
{
    portENTER_CRITICAL(&dmaworkaround_mux);
    dmaworkaround_channels_busy[dmachan-1] = 1;
    portEXIT_CRITICAL(&dmaworkaround_mux);
}

//Returns true if this peripheral is successfully claimed, false if otherwise.
//-----------------------------------------------------
bool spi_lobo_periph_claim(spi_lobo_host_device_t host)
{
    bool ret = __sync_bool_compare_and_swap(&spi_periph_claimed[host], false, true);
    if (ret) periph_module_enable(io_signal[host].module);
    return ret;
}

//Returns true if this peripheral is successfully freed, false if otherwise.
//-----------------------------------------------
bool spi_lobo_periph_free(spi_lobo_host_device_t host)
{
    bool ret = __sync_bool_compare_and_swap(&spi_periph_claimed[host], true, false);
    if (ret) periph_module_disable(io_signal[host].module);
    return ret;
}

//-----------------------------------------
bool spi_lobo_dma_chan_claim (int dma_chan)
{
    bool ret = false;
    assert( dma_chan == 1 || dma_chan == 2 );

    portENTER_CRITICAL(&spi_dma_spinlock);
    if ( !(spi_dma_chan_enabled & DMA_CHANNEL_ENABLED(dma_chan)) ) {
        // get the channel only when it's not claimed yet.
        spi_dma_chan_enabled |= DMA_CHANNEL_ENABLED(dma_chan);
        ret = true;
    }
    periph_module_enable( PERIPH_SPI_DMA_MODULE );
    portEXIT_CRITICAL(&spi_dma_spinlock);

    return ret;
}

//---------------------------------------
bool spi_lobo_dma_chan_free(int dma_chan)
{
    assert( dma_chan == 1 || dma_chan == 2 );
    assert( spi_dma_chan_enabled & DMA_CHANNEL_ENABLED(dma_chan) );

    portENTER_CRITICAL(&spi_dma_spinlock);
    spi_dma_chan_enabled &= ~DMA_CHANNEL_ENABLED(dma_chan);
    if ( spi_dma_chan_enabled == 0 ) {
        //disable the DMA only when all the channels are freed.
        periph_module_disable( PERIPH_SPI_DMA_MODULE );
    }
    portEXIT_CRITICAL(&spi_dma_spinlock);

    return true;
}


//======================================================================================================


//----------------------------------------------------------------------------------------------------------------
static esp_err_t spi_lobo_bus_initialize(spi_lobo_host_device_t host, spi_lobo_bus_config_t *bus_config, int init)
{
    bool native=true, spi_chan_claimed, dma_chan_claimed;

    if (init > 0) {
        /* ToDo: remove this when we have flash operations cooperating with this */
        SPI_CHECK(host!=TFT_SPI_HOST, "SPI1 is not supported", ESP_ERR_NOT_SUPPORTED);

        SPI_CHECK(host>=TFT_SPI_HOST && host<=TFT_VSPI_HOST, "invalid host", ESP_ERR_INVALID_ARG);
        SPI_CHECK(spihost[host]==NULL, "host already in use", ESP_ERR_INVALID_STATE);
    }
    else {
        SPI_CHECK(spihost[host]!=NULL, "host not in use", ESP_ERR_INVALID_STATE);
    }
    
    SPI_CHECK(bus_config->mosi_io_num<0 || GPIO_IS_VALID_OUTPUT_GPIO(bus_config->mosi_io_num), "spid pin invalid", ESP_ERR_INVALID_ARG);
    SPI_CHECK(bus_config->sclk_io_num<0 || GPIO_IS_VALID_OUTPUT_GPIO(bus_config->sclk_io_num), "spiclk pin invalid", ESP_ERR_INVALID_ARG);
    SPI_CHECK(bus_config->miso_io_num<0 || GPIO_IS_VALID_GPIO(bus_config->miso_io_num), "spiq pin invalid", ESP_ERR_INVALID_ARG);
    SPI_CHECK(bus_config->quadwp_io_num<0 || GPIO_IS_VALID_OUTPUT_GPIO(bus_config->quadwp_io_num), "spiwp pin invalid", ESP_ERR_INVALID_ARG);
    SPI_CHECK(bus_config->quadhd_io_num<0 || GPIO_IS_VALID_OUTPUT_GPIO(bus_config->quadhd_io_num), "spihd pin invalid", ESP_ERR_INVALID_ARG);

    if (init > 0) {
        spi_chan_claimed=spi_lobo_periph_claim(host);
        SPI_CHECK(spi_chan_claimed, "host already in use", ESP_ERR_INVALID_STATE);

        //spihost[host]=malloc(sizeof(spi_lobo_host_t));
		spihost[host]=heap_caps_malloc(sizeof(spi_lobo_host_t), MALLOC_CAP_DMA);
		if (spihost[host]==NULL) return ESP_ERR_NO_MEM;
		memset(spihost[host], 0, sizeof(spi_lobo_host_t));
		// Create semaphore
		spihost[host]->spi_lobo_bus_mutex = xSemaphoreCreateMutex();
		if (!spihost[host]->spi_lobo_bus_mutex) return ESP_ERR_NO_MEM;
    }

    spihost[host]->cur_device = -1;
    memcpy(&spihost[host]->cur_bus_config, bus_config, sizeof(spi_lobo_bus_config_t));

    //Check if the selected pins correspond to the native pins of the peripheral
    if (bus_config->mosi_io_num >= 0 && bus_config->mosi_io_num!=io_signal[host].spid_native) native=false;
    if (bus_config->miso_io_num >= 0 && bus_config->miso_io_num!=io_signal[host].spiq_native) native=false;
    if (bus_config->sclk_io_num >= 0 && bus_config->sclk_io_num!=io_signal[host].spiclk_native) native=false;
    if (bus_config->quadwp_io_num >= 0 && bus_config->quadwp_io_num!=io_signal[host].spiwp_native) native=false;
    if (bus_config->quadhd_io_num >= 0 && bus_config->quadhd_io_num!=io_signal[host].spihd_native) native=false;
    
    spihost[host]->no_gpio_matrix=native;
    if (native) {
        //All SPI native pin selections resolve to 1, so we put that here instead of trying to figure
        //out which FUNC_GPIOx_xSPIxx to grab; they all are defined to 1 anyway.
        if (bus_config->mosi_io_num > 0) PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[bus_config->mosi_io_num], 1);
        if (bus_config->miso_io_num > 0) PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[bus_config->miso_io_num], 1);
        if (bus_config->quadwp_io_num > 0) PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[bus_config->quadwp_io_num], 1);
        if (bus_config->quadhd_io_num > 0) PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[bus_config->quadhd_io_num], 1);
        if (bus_config->sclk_io_num > 0) PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[bus_config->sclk_io_num], 1);
    } else {
        //Use GPIO 
        if (bus_config->mosi_io_num>0) {
            PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[bus_config->mosi_io_num], PIN_FUNC_GPIO);
            gpio_set_direction(bus_config->mosi_io_num, GPIO_MODE_OUTPUT);
            gpio_matrix_out(bus_config->mosi_io_num, io_signal[host].spid_out, false, false);
            gpio_matrix_in(bus_config->mosi_io_num, io_signal[host].spid_in, false);
        }
        if (bus_config->miso_io_num>0) {
            PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[bus_config->miso_io_num], PIN_FUNC_GPIO);
            gpio_set_direction(bus_config->miso_io_num, GPIO_MODE_INPUT);
            gpio_matrix_out(bus_config->miso_io_num, io_signal[host].spiq_out, false, false);
            gpio_matrix_in(bus_config->miso_io_num, io_signal[host].spiq_in, false);
        }
        if (bus_config->quadwp_io_num>0) {
            PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[bus_config->quadwp_io_num], PIN_FUNC_GPIO);
            gpio_set_direction(bus_config->quadwp_io_num, GPIO_MODE_OUTPUT);
            gpio_matrix_out(bus_config->quadwp_io_num, io_signal[host].spiwp_out, false, false);
            gpio_matrix_in(bus_config->quadwp_io_num, io_signal[host].spiwp_in, false);
        }
        if (bus_config->quadhd_io_num>0) {
            PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[bus_config->quadhd_io_num], PIN_FUNC_GPIO);
            gpio_set_direction(bus_config->quadhd_io_num, GPIO_MODE_OUTPUT);
            gpio_matrix_out(bus_config->quadhd_io_num, io_signal[host].spihd_out, false, false);
            gpio_matrix_in(bus_config->quadhd_io_num, io_signal[host].spihd_in, false);
        }
        if (bus_config->sclk_io_num>0) {
            PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[bus_config->sclk_io_num], PIN_FUNC_GPIO);
            gpio_set_direction(bus_config->sclk_io_num, GPIO_MODE_OUTPUT);
            gpio_matrix_out(bus_config->sclk_io_num, io_signal[host].spiclk_out, false, false);
        }
    }
	periph_module_enable(io_signal[host].module);
	spihost[host]->hw=io_signal[host].hw;

	if (init > 0) {
        dma_chan_claimed=spi_lobo_dma_chan_claim(init);
        if ( !dma_chan_claimed ) {
        	spi_lobo_periph_free( host );
            SPI_CHECK(dma_chan_claimed, "dma channel already in use", ESP_ERR_INVALID_STATE);
        }
	    spihost[host]->dma_chan = init;
        //See how many dma descriptors we need and allocate them
        int dma_desc_ct=(bus_config->max_transfer_sz+SPI_MAX_DMA_LEN-1)/SPI_MAX_DMA_LEN;
        if (dma_desc_ct==0) dma_desc_ct=1; //default to 4k when max is not given
        spihost[host]->max_transfer_sz = dma_desc_ct*SPI_MAX_DMA_LEN;

        spihost[host]->dmadesc_tx=heap_caps_malloc(sizeof(lldesc_t)*dma_desc_ct, MALLOC_CAP_DMA);
        spihost[host]->dmadesc_rx=heap_caps_malloc(sizeof(lldesc_t)*dma_desc_ct, MALLOC_CAP_DMA);
        if (!spihost[host]->dmadesc_tx || !spihost[host]->dmadesc_rx) goto nomem;

        //Tell common code DMA workaround that our DMA channel is idle. If needed, the code will do a DMA reset.
        spi_lobo_dmaworkaround_idle(spihost[host]->dma_chan);

        // Reset DMA
        spihost[host]->hw->dma_conf.val |= SPI_OUT_RST|SPI_IN_RST|SPI_AHBM_RST|SPI_AHBM_FIFO_RST;
        spihost[host]->hw->dma_out_link.start=0;
        spihost[host]->hw->dma_in_link.start=0;
        spihost[host]->hw->dma_conf.val &= ~(SPI_OUT_RST|SPI_IN_RST|SPI_AHBM_RST|SPI_AHBM_FIFO_RST);
        spihost[host]->hw->dma_conf.out_data_burst_en=1;

        //Reset timing
        spihost[host]->hw->ctrl2.val=0;

        //Disable unneeded ints
        spihost[host]->hw->slave.rd_buf_done=0;
        spihost[host]->hw->slave.wr_buf_done=0;
        spihost[host]->hw->slave.rd_sta_done=0;
        spihost[host]->hw->slave.wr_sta_done=0;
        spihost[host]->hw->slave.rd_buf_inten=0;
        spihost[host]->hw->slave.wr_buf_inten=0;
        spihost[host]->hw->slave.rd_sta_inten=0;
        spihost[host]->hw->slave.wr_sta_inten=0;

        //Force a transaction done interrupt. This interrupt won't fire yet because we initialized the SPI interrupt as
        //disabled.  This way, we can just enable the SPI interrupt and the interrupt handler will kick in, handling
        //any transactions that are queued.
        spihost[host]->hw->slave.trans_inten=1;
        spihost[host]->hw->slave.trans_done=1;

		//Select DMA channel.
		DPORT_SET_PERI_REG_BITS(DPORT_SPI_DMA_CHAN_SEL_REG, 3, init, (host * 2));
    }
    return ESP_OK;

nomem:
	if (spihost[host]) {
		free(spihost[host]->dmadesc_tx);
		free(spihost[host]->dmadesc_rx);
	}
	free(spihost[host]);
    spi_lobo_periph_free(host);
	return ESP_ERR_NO_MEM;
}

//---------------------------------------------------------------------------
static esp_err_t spi_lobo_bus_free(spi_lobo_host_device_t host, int dofree)
{
	if ((host == TFT_SPI_HOST) || (host > TFT_VSPI_HOST)) return ESP_ERR_NOT_SUPPORTED;  // invalid host

	if (spihost[host] == NULL) return ESP_ERR_INVALID_STATE;  // host not in use

    if (dofree) {
		for (int x=0; x<NO_DEV; x++) {
			if (spihost[host]->device[x] != NULL) return ESP_ERR_INVALID_STATE;  // not all devices freed
		}
    }
    if ( spihost[host]->dma_chan > 0 ) {
        spi_lobo_dma_chan_free ( spihost[host]->dma_chan );
    }
    spihost[host]->hw->slave.trans_inten=0;
    spihost[host]->hw->slave.trans_done=0;
    spi_lobo_periph_free(host);

    if (dofree) {
		vSemaphoreDelete(spihost[host]->spi_lobo_bus_mutex);
	    free(spihost[host]->dmadesc_tx);
	    free(spihost[host]->dmadesc_rx);
		free(spihost[host]);
		spihost[host] = NULL;
    }
    return ESP_OK;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
esp_err_t spi_lobo_bus_add_device(spi_lobo_host_device_t host, spi_lobo_bus_config_t *bus_config, spi_lobo_device_interface_config_t *dev_config, spi_lobo_device_handle_t *handle)
{
	if ((host == TFT_SPI_HOST) || (host > TFT_VSPI_HOST)) return ESP_ERR_NOT_SUPPORTED;  // invalid host
	
	if (spihost[host] == NULL) {
		esp_err_t ret = spi_lobo_bus_initialize(host, bus_config, 1);
		if (ret) return ret;
	}
	
	int freecs, maxdev;
    int apbclk=APB_CLK_FREQ;

	if (spihost[host] == NULL) return ESP_ERR_INVALID_STATE;

    if (dev_config->spics_io_num >= 0) {
		if (!GPIO_IS_VALID_OUTPUT_GPIO(dev_config->spics_io_num)) return  ESP_ERR_INVALID_ARG;
		if (dev_config->spics_ext_io_num > 0) dev_config->spics_ext_io_num = -1;
	}
	else {
		//if ((dev_config->spics_ext_io_num <= 0) || (!GPIO_IS_VALID_OUTPUT_GPIO(dev_config->spics_ext_io_num))) return ESP_ERR_INVALID_ARG;
	}

    //ToDo: Check if some other device uses the same 'spics_ext_io_num'

    if (dev_config->clock_speed_hz == 0) return ESP_ERR_INVALID_ARG;
	if (dev_config->spics_io_num > 0) maxdev = NO_CS;
	else maxdev = NO_DEV;

    for (freecs=0; freecs<maxdev; freecs++) {
        //See if this slot is free; reserve if it is by putting a dummy pointer in the slot. We use an atomic compare&swap to make this thread-safe.
        if (__sync_bool_compare_and_swap(&spihost[host]->device[freecs], NULL, (spi_lobo_device_t *)1)) break;
    }
    if (freecs == maxdev) return ESP_ERR_NOT_FOUND;

    // The hardware looks like it would support this, but actually setting cs_ena_pretrans when transferring in full
    // duplex mode does absolutely nothing on the ESP32.
    if ((dev_config->cs_ena_pretrans != 0) && (dev_config->flags & LB_SPI_DEVICE_HALFDUPLEX)) return ESP_ERR_INVALID_ARG;

    // Speeds >=40MHz over GPIO matrix needs a dummy cycle, but these don't work for full-duplex connections.
    if (((dev_config->flags & LB_SPI_DEVICE_HALFDUPLEX)==0) && (dev_config->clock_speed_hz > ((apbclk*2)/5)) && (!spihost[host]->no_gpio_matrix)) return ESP_ERR_INVALID_ARG;

    //Allocate memory for device
    spi_lobo_device_t *dev=malloc(sizeof(spi_lobo_device_t));
    if (dev==NULL) return ESP_ERR_NO_MEM;

    memset(dev, 0, sizeof(spi_lobo_device_t));
    spihost[host]->device[freecs]=dev;

    if (dev_config->duty_cycle_pos==0) dev_config->duty_cycle_pos=128;
    dev->host=spihost[host];
	dev->host_dev = host;

    //We want to save a copy of the dev config in the dev struct.
    memcpy(&dev->cfg, dev_config, sizeof(spi_lobo_device_interface_config_t));
    //We want to save a copy of the bus config in the dev struct.
    memcpy(&dev->bus_config, bus_config, sizeof(spi_lobo_bus_config_t));

    //Set CS pin, CS options
    if (dev_config->spics_io_num > 0) {
        if (spihost[host]->no_gpio_matrix &&dev_config->spics_io_num == io_signal[host].spics0_native && freecs==0) {
            //Again, the cs0s for all SPI peripherals map to pin mux source 1, so we use that instead of a define.
            PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[dev_config->spics_io_num], 1);
        } else {
            //Use GPIO matrix
            PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[dev_config->spics_io_num], PIN_FUNC_GPIO);
            gpio_set_direction(dev_config->spics_io_num, GPIO_MODE_OUTPUT);
            gpio_matrix_out(dev_config->spics_io_num, io_signal[host].spics_out[freecs], false, false);
        }
    }
    else if (dev_config->spics_ext_io_num >= 0) {
		gpio_set_direction(dev_config->spics_ext_io_num, GPIO_MODE_OUTPUT);
		gpio_set_level(dev_config->spics_ext_io_num, 1);
	}
    if (dev_config->flags & LB_SPI_DEVICE_CLK_AS_CS) {
        spihost[host]->hw->pin.master_ck_sel |= (1<<freecs);
    } else {
        spihost[host]->hw->pin.master_ck_sel &= (1<<freecs);
    }
    if (dev_config->flags & LB_SPI_DEVICE_POSITIVE_CS) {
        spihost[host]->hw->pin.master_cs_pol |= (1<<freecs);
    } else {
        spihost[host]->hw->pin.master_cs_pol &= (1<<freecs);
    }

    *handle = dev;
    return ESP_OK;
}

//-------------------------------------------------------------------
esp_err_t spi_lobo_bus_remove_device(spi_lobo_device_handle_t handle)
{
    int x;
    if (handle == NULL) return ESP_ERR_INVALID_ARG;

    //Remove device from list of csses and free memory
    for (x=0; x<NO_DEV; x++) {
        if (handle->host->device[x] == handle) handle->host->device[x]=NULL;
    }
	
	// Check if all devices are removed from this host and free the bus if yes
	for (x=0; x<NO_DEV; x++) {
		if (spihost[handle->host_dev]->device[x] !=NULL) break;
	}
	if (x == NO_DEV) {
		free(handle);
		spi_lobo_bus_free(handle->host_dev, 1);
	}
	else free(handle);

	return ESP_OK;
}

//-----------------------------------------------------------------
static int IRAM_ATTR spi_freq_for_pre_n(int fapb, int pre, int n) {
    return (fapb / (pre * n));
}

/*
 * Set the SPI clock to a certain frequency. Returns the effective frequency set, which may be slightly
 * different from the requested frequency.
 */
//-----------------------------------------------------------------------------------
static int IRAM_ATTR spi_set_clock(spi_dev_t *hw, int fapb, int hz, int duty_cycle) {
   int pre, n, h, l, eff_clk;

    //In hw, n, h and l are 1-64, pre is 1-8K. Value written to register is one lower than used value.
    if (hz>((fapb/4)*3)) {
        //Using Fapb directly will give us the best result here.
        hw->clock.clkcnt_l=0;
        hw->clock.clkcnt_h=0;
        hw->clock.clkcnt_n=0;
        hw->clock.clkdiv_pre=0;
        hw->clock.clk_equ_sysclk=1;
        eff_clk=fapb;
    } else {
        //For best duty cycle resolution, we want n to be as close to 32 as possible, but
        //we also need a pre/n combo that gets us as close as possible to the intended freq.
        //To do this, we bruteforce n and calculate the best pre to go along with that.
        //If there's a choice between pre/n combos that give the same result, use the one
        //with the higher n.
        int bestn=-1;
        int bestpre=-1;
        int besterr=0;
        int errval;
        for (n=1; n<=64; n++) {
            //Effectively, this does pre=round((fapb/n)/hz).
            pre=((fapb/n)+(hz/2))/hz;
            if (pre<=0) pre=1;
            if (pre>8192) pre=8192;
            errval=abs(spi_freq_for_pre_n(fapb, pre, n)-hz);
            if (bestn==-1 || errval<=besterr) {
                besterr=errval;
                bestn=n;
                bestpre=pre;
            }
        }

        n=bestn;
        pre=bestpre;
        l=n;
        //This effectively does round((duty_cycle*n)/256)
        h=(duty_cycle*n+127)/256;
        if (h<=0) h=1;

        hw->clock.clk_equ_sysclk=0;
        hw->clock.clkcnt_n=n-1;
        hw->clock.clkdiv_pre=pre-1;
        hw->clock.clkcnt_h=h-1;
        hw->clock.clkcnt_l=l-1;
        eff_clk=spi_freq_for_pre_n(fapb, pre, n);
    }
    return eff_clk;
}



//------------------------------------------------------------------------------------
esp_err_t IRAM_ATTR spi_lobo_device_select(spi_lobo_device_handle_t handle, int force)
{
	if (handle == NULL) return ESP_ERR_INVALID_ARG;

	if ((handle->cfg.selected == 1) && (!force)) return ESP_OK;  // already selected

	int i;
	spi_lobo_host_t *host=(spi_lobo_host_t*)handle->host;

	// find device's host bus
	for (i=0; i<NO_DEV; i++) {
		if (host->device[i] == handle) break;
	}
	if (i == NO_DEV) return ESP_ERR_INVALID_ARG;

	if (!(xSemaphoreTake(host->spi_lobo_bus_mutex, SPI_SEMAPHORE_WAIT))) return ESP_ERR_INVALID_STATE;

	// Check if previously used device's bus device is the same
	if (memcmp(&host->cur_bus_config, &handle->bus_config, sizeof(spi_lobo_bus_config_t)) != 0) {
		// device has different bus configuration, we need to reconfigure the bus
		esp_err_t err = spi_lobo_bus_free(1, 0);
		if (err) {
			xSemaphoreGive(host->spi_lobo_bus_mutex);
			return err;
		}
		err = spi_lobo_bus_initialize(i, &handle->bus_config, -1);
		if (err) {
			xSemaphoreGive(host->spi_lobo_bus_mutex);
			return err;
		}
	}

	//Reconfigure according to device settings, but only if the device changed or forced.
	if ((force) || (host->device[host->cur_device] != handle)) {
	    //Assumes a hardcoded 80MHz Fapb for now. ToDo: figure out something better once we have clock scaling working.
		int apbclk=APB_CLK_FREQ;

	    //Speeds >=40MHz over GPIO matrix needs a dummy cycle, but these don't work for full-duplex connections.
	    if (((handle->cfg.flags & LB_SPI_DEVICE_HALFDUPLEX) == 0) && (handle->cfg.clock_speed_hz > ((apbclk*2)/5)) && (!host->no_gpio_matrix)) {
	    	// set speed to 32 MHz
	    	handle->cfg.clock_speed_hz = (apbclk*2)/5;
	    }

		int effclk=spi_set_clock(host->hw, apbclk, handle->cfg.clock_speed_hz, handle->cfg.duty_cycle_pos);
		//Configure bit order
		host->hw->ctrl.rd_bit_order=(handle->cfg.flags & LB_SPI_DEVICE_RXBIT_LSBFIRST)?1:0;
		host->hw->ctrl.wr_bit_order=(handle->cfg.flags & LB_SPI_DEVICE_TXBIT_LSBFIRST)?1:0;
		
		//Configure polarity
        //SPI iface needs to be configured for a delay in some cases.
		int nodelay=0;
        int extra_dummy=0;
        if (host->no_gpio_matrix) {
            if (effclk >= apbclk/2) {
                nodelay=1;
            }
        } else {
            if (effclk >= apbclk/2) {
                nodelay=1;
                extra_dummy=1;          //Note: This only works on half-duplex connections. spi_lobo_bus_add_device checks for this.
            } else if (effclk >= apbclk/4) {
                nodelay=1;
            }
        }
		if (handle->cfg.mode==0) {
			host->hw->pin.ck_idle_edge=0;
			host->hw->user.ck_out_edge=0;
			host->hw->ctrl2.miso_delay_mode=nodelay?0:2;
		} else if (handle->cfg.mode==1) {
			host->hw->pin.ck_idle_edge=0;
			host->hw->user.ck_out_edge=1;
			host->hw->ctrl2.miso_delay_mode=nodelay?0:1;
		} else if (handle->cfg.mode==2) {
			host->hw->pin.ck_idle_edge=1;
			host->hw->user.ck_out_edge=1;
			host->hw->ctrl2.miso_delay_mode=nodelay?0:1;
		} else if (handle->cfg.mode==3) {
			host->hw->pin.ck_idle_edge=1;
			host->hw->user.ck_out_edge=0;
			host->hw->ctrl2.miso_delay_mode=nodelay?0:2;
		}

		//Configure bit sizes, load addr and command
		host->hw->user.usr_dummy=(handle->cfg.dummy_bits+extra_dummy)?1:0;
		host->hw->user.usr_addr=(handle->cfg.address_bits)?1:0;
		host->hw->user.usr_command=(handle->cfg.command_bits)?1:0;
		host->hw->user1.usr_addr_bitlen=handle->cfg.address_bits-1;
		host->hw->user1.usr_dummy_cyclelen=handle->cfg.dummy_bits+extra_dummy-1;
		host->hw->user2.usr_command_bitlen=handle->cfg.command_bits-1;
		//Configure misc stuff
		host->hw->user.doutdin=(handle->cfg.flags & LB_SPI_DEVICE_HALFDUPLEX)?0:1;
		host->hw->user.sio=(handle->cfg.flags & LB_SPI_DEVICE_3WIRE)?1:0;

		host->hw->ctrl2.setup_time=handle->cfg.cs_ena_pretrans-1;
		host->hw->user.cs_setup=handle->cfg.cs_ena_pretrans?1:0;
		host->hw->ctrl2.hold_time=handle->cfg.cs_ena_posttrans-1;
		host->hw->user.cs_hold=(handle->cfg.cs_ena_posttrans)?1:0;

		//Configure CS pin
		host->hw->pin.cs0_dis=(i==0)?0:1;
		host->hw->pin.cs1_dis=(i==1)?0:1;
		host->hw->pin.cs2_dis=(i==2)?0:1;
		
		host->cur_device = i;
	}

	if ((handle->cfg.spics_io_num < 0) && (handle->cfg.spics_ext_io_num > 0)) {
		gpio_set_level(handle->cfg.spics_ext_io_num, 0);
	}

	handle->cfg.selected = 1;

	return ESP_OK;
}

//---------------------------------------------------------------------------
esp_err_t IRAM_ATTR spi_lobo_device_deselect(spi_lobo_device_handle_t handle)
{
	if (handle == NULL) return ESP_ERR_INVALID_ARG;

	if (handle->cfg.selected == 0) return ESP_OK;  // already deselected

	int i;
	spi_lobo_host_t *host=(spi_lobo_host_t*)handle->host;

	for (i=0; i<NO_DEV; i++) {
		if (host->device[i] == handle) break;
	}
	if (i == NO_DEV) return ESP_ERR_INVALID_ARG;
	
	if (host->device[host->cur_device] == handle) {
		if ((handle->cfg.spics_io_num < 0) && (handle->cfg.spics_ext_io_num > 0)) {
			gpio_set_level(handle->cfg.spics_ext_io_num, 1);
		}
	}

	handle->cfg.selected = 0;
	xSemaphoreGive(host->spi_lobo_bus_mutex);

	return ESP_OK;
}

//--------------------------------------------------------------------------------
esp_err_t IRAM_ATTR spi_lobo_device_TakeSemaphore(spi_lobo_device_handle_t handle)
{
	if (!(xSemaphoreTake(handle->host->spi_lobo_bus_mutex, SPI_SEMAPHORE_WAIT))) return ESP_ERR_INVALID_STATE;
	else return ESP_OK;
}

//---------------------------------------------------------------------------
void IRAM_ATTR spi_lobo_device_GiveSemaphore(spi_lobo_device_handle_t handle)
{
	xSemaphoreTake(handle->host->spi_lobo_bus_mutex, portMAX_DELAY);
}

//----------------------------------------------------------
uint32_t spi_lobo_get_speed(spi_lobo_device_handle_t handle)
{
	spi_lobo_host_t *host=(spi_lobo_host_t*)handle->host;
	uint32_t speed = 0;
	if (spi_lobo_device_select(handle, 0) == ESP_OK) {
		if (host->hw->clock.clk_equ_sysclk == 1) speed = 80000000;
		else speed =  80000000/(host->hw->clock.clkdiv_pre+1)/(host->hw->clock.clkcnt_n+1);
	}
	spi_lobo_device_deselect(handle);
	return speed;
}

//--------------------------------------------------------------------------
uint32_t spi_lobo_set_speed(spi_lobo_device_handle_t handle, uint32_t speed)
{
	spi_lobo_host_t *host=(spi_lobo_host_t*)handle->host;
	uint32_t newspeed = 0;
	if (spi_lobo_device_select(handle, 0) == ESP_OK) {
		spi_lobo_device_deselect(handle);
		handle->cfg.clock_speed_hz = speed;
		if (spi_lobo_device_select(handle, 1) == ESP_OK) {
			if (host->hw->clock.clk_equ_sysclk == 1) newspeed = 80000000;
			else newspeed =  80000000/(host->hw->clock.clkdiv_pre+1)/(host->hw->clock.clkcnt_n+1);
		}
	}
	spi_lobo_device_deselect(handle);
	
	return newspeed;
}

//-------------------------------------------------------------
bool spi_lobo_uses_native_pins(spi_lobo_device_handle_t handle)
{
	return handle->host->no_gpio_matrix;
}

//-------------------------------------------------------------------
void spi_lobo_get_native_pins(int host, int *sdi, int *sdo, int *sck)
{
	*sdo = io_signal[host].spid_native;
	*sdi = io_signal[host].spiq_native;
	*sck = io_signal[host].spiclk_native;
}

/*
When using  'spi_lobo_transfer_data' function we can have several scenarios:

A: Send only      (trans->rxlength = 0)
B: Receive only   (trans->txlength = 0)
C: Send & receive (trans->txlength > 0 & trans->rxlength > 0)
D: No operation   (trans->txlength = 0 & trans->rxlength = 0)

*/
//----------------------------------------------------------------------------------------------------------
esp_err_t IRAM_ATTR spi_lobo_transfer_data(spi_lobo_device_handle_t handle, spi_lobo_transaction_t *trans) {
	if (!handle) return ESP_ERR_INVALID_ARG;

	// *** For now we can only handle 8-bit bytes transmission
	if (((trans->length % 8) != 0) || ((trans->rxlength % 8) != 0)) return ESP_ERR_INVALID_ARG;

	spi_lobo_host_t *host=(spi_lobo_host_t*)handle->host;
	esp_err_t ret;
	uint8_t do_deselect = 0;
    const uint8_t *txbuffer = NULL;
	uint8_t *rxbuffer = NULL;

	if (trans->flags & LB_SPI_TRANS_USE_TXDATA) {
        // Send data from 'trans->tx_data'
		txbuffer=(uint8_t*)&trans->tx_data[0];
	} else {
        // Send data from 'trans->tx_buffer'
		txbuffer=(uint8_t*)trans->tx_buffer;
	}
	if (trans->flags & LB_SPI_TRANS_USE_RXDATA) {
        // Receive data to 'trans->rx_data'
		rxbuffer=(uint8_t*)&trans->rx_data[0];
	} else {
        // Receive data to 'trans->rx_buffer'
		rxbuffer=(uint8_t*)trans->rx_buffer;
	}

	// ** Set transmit & receive length in bytes
	uint32_t txlen = trans->length / 8;
	uint32_t rxlen = trans->rxlength / 8;

	if (txbuffer == NULL) txlen = 0;
	if (rxbuffer == NULL) rxlen = 0;
	if ((rxlen == 0) && (txlen == 0)) {
        // ** NOTHING TO SEND or RECEIVE, return
        return ESP_ERR_INVALID_ARG;
    }

    // If using 'trans->tx_data' and/or 'trans->rx_data', maximum 4 bytes can be sent/received
	if ((txbuffer == &trans->tx_data[0]) && (txlen > 4)) return ESP_ERR_INVALID_ARG;
	if ((rxbuffer == &trans->rx_data[0]) && (rxlen > 4)) return ESP_ERR_INVALID_ARG;

	// --- Wait for SPI bus ready ---
	while (host->hw->cmd.usr);

    // ** If the device was not selected, select it
	if (handle->cfg.selected == 0) {
		ret = spi_lobo_device_select(handle, 0);
		if (ret) return ret;
		do_deselect = 1;     // We will deselect the device after the operation !
	}

	// ** Call pre-transmission callback, if any
	if (handle->cfg.pre_cb) handle->cfg.pre_cb(trans);

    // Test if operating in full duplex mode
	uint8_t duplex = 1;
	if (handle->cfg.flags & LB_SPI_DEVICE_HALFDUPLEX) duplex = 0; // Half duplex mode !

    uint32_t bits, rdbits;
	uint32_t wd;
	uint8_t bc, rdidx;
	uint32_t rdcount = rxlen;	// Total number of bytes to read
	uint32_t count = 0;			// number of bytes transmitted
	uint32_t rd_read = 0;		// Number of bytes read so far

	host->hw->user.usr_mosi_highpart = 0; // use the whole spi buffer

    // ** Check if address phase will be used
	host->hw->user2.usr_command_value=trans->command;
	if (handle->cfg.address_bits>32) {
		host->hw->addr=trans->address >> 32;
		host->hw->slv_wr_status=trans->address & 0xffffffff;
	} else {
		host->hw->addr=trans->address & 0xffffffff;
	}

	// Check if we have to transmit some data
	if (txlen > 0) {
		host->hw->user.usr_mosi = 1;
		uint8_t idx;
		bits = 0;				// remaining bits to send
		idx = 0;				// index to spi hw data_buf (16 32-bit words, 64 bytes, 512 bits)

        // ** Transmit 'txlen' bytes
		while (count < txlen) {
			wd = 0;
			for (bc=0;bc<32;bc+=8) {
				wd |= (uint32_t)txbuffer[count] << bc;
				count++;                    // Increment sent data count
				bits += 8;                  // Increment bits count
				if (count == txlen) break;  // If all transmit data pushed to hw spi buffer break from the loop
			}
			host->hw->data_buf[idx] = wd;
			idx++;
			if (idx == 16) {
				// hw SPI buffer full (all 64 bytes filled, START THE TRANSSACTION
				host->hw->mosi_dlen.usr_mosi_dbitlen=bits-1;            // Set mosi dbitlen

				if ((duplex) && (rdcount > 0)) {
                    // In full duplex mode we are receiving while sending !
					host->hw->miso_dlen.usr_miso_dbitlen = bits-1;      // Set miso dbitlen
					host->hw->user.usr_miso = 1;
				}
				else {
					host->hw->miso_dlen.usr_miso_dbitlen = 0;           // In half duplex mode nothing will be received
					host->hw->user.usr_miso = 0;
				}

				// ** Start the transaction ***
				host->hw->cmd.usr=1;
                // Wait the transaction to finish
				while (host->hw->cmd.usr);

				if ((duplex) && (rdcount > 0)) {
					// *** in full duplex mode transfer received data to input buffer ***
					rdidx = 0;
			    	while (bits > 0) {
						wd = host->hw->data_buf[rdidx];
						rdidx++;
						for (bc=0;bc<32;bc+=8) { // get max 4 bytes
							rxbuffer[rd_read++] = (uint8_t)((wd >> bc) & 0xFF);
							rdcount--;
							bits -= 8;
							if (rdcount == 0) {
								bits = 0;
								break; // Finished reading data
							}
						}
			    	}
				}
				bits = 0;   // nothing in hw spi buffer yet
				idx = 0;    // start from the beginning of the hw spi buffer
			}
		}
		// *** All transmit data are sent or pushed to hw spi buffer
		// bits > 0  IF THERE ARE SOME DATA STILL WAITING IN THE HW SPI TRANSMIT BUFFER
		if (bits > 0) {
			// ** WE HAVE SOME DATA IN THE HW SPI TRANSMIT BUFFER
			host->hw->mosi_dlen.usr_mosi_dbitlen = bits-1;          // Set mosi dbitlen

			if ((duplex) && (rdcount > 0)) {
                // In full duplex mode we are receiving while sending !
				host->hw->miso_dlen.usr_miso_dbitlen = bits-1;      // Set miso dbitlen
				host->hw->user.usr_miso = 1;
			}
			else {
				host->hw->miso_dlen.usr_miso_dbitlen = 0;           // In half duplex mode nothing will be received
				host->hw->user.usr_miso = 0;
			}

			// ** Start the transaction ***
			host->hw->cmd.usr=1;
            // Wait the transaction to finish
			while (host->hw->cmd.usr);

			if ((duplex) && (rdcount > 0)) {
                // *** in full duplex mode transfer received data to input buffer ***
				rdidx = 0;
		    	while (bits > 0) {
					wd = host->hw->data_buf[rdidx];
					rdidx++;
					for (bc=0;bc<32;bc+=8) { // get max 4 bytes
						rxbuffer[rd_read++] = (uint8_t)((wd >> bc) & 0xFF);
						rdcount--;
						bits -= 8;
						if (bits == 0) break;
						if (rdcount == 0) {
							bits = 0;
							break; // Finished reading data
						}
					}
		    	}
			}
		}
		//if (duplex) rdcount = 0;  // In duplex mode receive only as many bytes as was transmitted
	}

	// ------------------------------------------------------------------------
	// *** If rdcount = 0 we have nothing to receive and we exit the function
    //     This is true if no data receive was requested,
    //     or all the data was received in Full duplex mode during the transmission
	// ------------------------------------------------------------------------
	if (rdcount > 0) {
		// ----------------------------------------------------------------------------------------------------------------
		// *** rdcount > 0, we have to receive some data
		//     This is true if we operate in Half duplex mode when receiving after transmission is done,
		//     or not all data was received in Full duplex mode during the transmission (trans->rxlength > trans->txlength)
		// ----------------------------------------------------------------------------------------------------------------
		host->hw->user.usr_mosi = 0;  // do not send
		host->hw->user.usr_miso = 1;  // do receive
		while (rdcount > 0) {
			if (rdcount <= 64) rdbits = rdcount * 8;
			else rdbits = 64 * 8;

			// Load receive buffer
			host->hw->mosi_dlen.usr_mosi_dbitlen=0;
			host->hw->miso_dlen.usr_miso_dbitlen=rdbits-1;

			// ** Start the transaction ***
			host->hw->cmd.usr=1;
			// Wait the transaction to finish
			while (host->hw->cmd.usr);

			// *** transfer received data to input buffer ***
			rdidx = 0;
			while (rdbits > 0) {
				wd = host->hw->data_buf[rdidx];
				rdidx++;
				for (bc=0;bc<32;bc+=8) {
					rxbuffer[rd_read++] = (uint8_t)((wd >> bc) & 0xFF);
					rdcount--;
					rdbits -= 8;
					if (rdcount == 0) {
						rdbits = 0;
						break;
					}
				}
			}
		}
	}

	// ** Call post-transmission callback, if any
	if (handle->cfg.post_cb) handle->cfg.post_cb(trans);

	if (do_deselect) {
        // Spi device was selected in this function, we have to deselect it now
		ret = spi_lobo_device_deselect(handle);
		if (ret) return ret;
	}

	return ESP_OK;
}
