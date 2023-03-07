/*
 *  Author: LoBo (loboris@gmail.com, loboris.github)
 *
 *  Module supporting SPI TFT displays based on ILI9341 & ILI9488 controllers
 *
 * HIGH SPEED LOW LEVEL DISPLAY FUNCTIONS
 * USING DIRECT or DMA SPI TRANSFER MODEs
 *
*/

#include <string.h>
#include "tftspi.h"
#include "freertos/task.h"
#include "soc/spi_reg.h"
#include "driver/gpio.h"


// ====================================================
// ==== Global variables, default values ==============

// Converts colors to grayscale if set to 1
uint8_t tft_gray_scale = 0;
// Spi clock for reading data from display memory in Hz
uint32_t tft_max_rdclock = 8000000;

// Default display dimensions
int tft_width = DEFAULT_TFT_DISPLAY_WIDTH;
int tft_height = DEFAULT_TFT_DISPLAY_HEIGHT;

// Display type, DISP_TYPE_ILI9488 or DISP_TYPE_ILI9341
uint8_t tft_disp_type = DEFAULT_DISP_TYPE;

// Spi device handles for display and touch screen
spi_lobo_device_handle_t tft_disp_spi = NULL;
spi_lobo_device_handle_t tft_ts_spi = NULL;

// ====================================================


static color_t *trans_cline = NULL;
static uint8_t _dma_sending = 0;

// RGB to GRAYSCALE constants
// 0.2989  0.5870  0.1140
#define GS_FACT_R 0.2989
#define GS_FACT_G 0.4870
#define GS_FACT_B 0.2140



// ==== Functions =====================

//------------------------------------------------------
esp_err_t IRAM_ATTR wait_trans_finish(uint8_t free_line)
{
	// Wait for SPI bus ready
	while (tft_disp_spi->host->hw->cmd.usr);
	if ((free_line) && (trans_cline)) {
		free(trans_cline);
		trans_cline = NULL;
	}
	if (_dma_sending) {
	    //Tell common code DMA workaround that our DMA channel is idle. If needed, the code will do a DMA reset.
	    if (tft_disp_spi->host->dma_chan) spi_lobo_dmaworkaround_idle(tft_disp_spi->host->dma_chan);

	    // Reset DMA
		tft_disp_spi->host->hw->dma_conf.val |= SPI_OUT_RST|SPI_IN_RST|SPI_AHBM_RST|SPI_AHBM_FIFO_RST;
		tft_disp_spi->host->hw->dma_out_link.start=0;
		tft_disp_spi->host->hw->dma_in_link.start=0;
		tft_disp_spi->host->hw->dma_conf.val &= ~(SPI_OUT_RST|SPI_IN_RST|SPI_AHBM_RST|SPI_AHBM_FIFO_RST);
		tft_disp_spi->host->hw->dma_conf.out_data_burst_en=1;
		_dma_sending = 0;
	}
    return ESP_OK;
}

//-------------------------------
esp_err_t IRAM_ATTR disp_select()
{
	wait_trans_finish(1);
	return spi_lobo_device_select(tft_disp_spi, 0);
}

//---------------------------------
esp_err_t IRAM_ATTR disp_deselect()
{
	wait_trans_finish(1);
	return spi_lobo_device_deselect(tft_disp_spi);
}

//---------------------------------------------------------------------------------------------------
static void IRAM_ATTR _spi_transfer_start(spi_lobo_device_handle_t spi_dev, int wrbits, int rdbits) {
	// Load send buffer
	spi_dev->host->hw->user.usr_mosi_highpart = 0;
	spi_dev->host->hw->mosi_dlen.usr_mosi_dbitlen = wrbits-1;
	spi_dev->host->hw->user.usr_mosi = 1;
    if (rdbits) {
        spi_dev->host->hw->miso_dlen.usr_miso_dbitlen = rdbits;
        spi_dev->host->hw->user.usr_miso = 1;
    }
    else {
        spi_dev->host->hw->miso_dlen.usr_miso_dbitlen = 0;
        spi_dev->host->hw->user.usr_miso = 0;
    }
	// Start transfer
	spi_dev->host->hw->cmd.usr = 1;
    // Wait for SPI bus ready
	while (spi_dev->host->hw->cmd.usr);
}

// Send 1 byte display command, display must be selected
//------------------------------------------------
void IRAM_ATTR disp_spi_transfer_cmd(int8_t cmd) {
	// Wait for SPI bus ready
	while (tft_disp_spi->host->hw->cmd.usr);

	// Set DC to 0 (command mode);
    gpio_set_level(PIN_NUM_DC, 0);

    tft_disp_spi->host->hw->data_buf[0] = (uint32_t)cmd;
    _spi_transfer_start(tft_disp_spi, 8, 0);
}

// Send command with data to display, display must be selected
//----------------------------------------------------------------------------------
void IRAM_ATTR disp_spi_transfer_cmd_data(int8_t cmd, uint8_t *data, uint32_t len) {
	// Wait for SPI bus ready
	while (tft_disp_spi->host->hw->cmd.usr);

    // Set DC to 0 (command mode);
    gpio_set_level(PIN_NUM_DC, 0);

    tft_disp_spi->host->hw->data_buf[0] = (uint32_t)cmd;
    _spi_transfer_start(tft_disp_spi, 8, 0);

	if ((len == 0) || (data == NULL)) return;

    // Set DC to 1 (data mode);
	gpio_set_level(PIN_NUM_DC, 1);

	uint8_t idx=0, bidx=0;
	uint32_t bits=0;
	uint32_t count=0;
	uint32_t wd = 0;
	while (count < len) {
		// get data byte from buffer
		wd |= (uint32_t)data[count] << bidx;
    	count++;
    	bits += 8;
		bidx += 8;
    	if (count == len) {
    		tft_disp_spi->host->hw->data_buf[idx] = wd;
    		break;
    	}
		if (bidx == 32) {
			tft_disp_spi->host->hw->data_buf[idx] = wd;
			idx++;
			bidx = 0;
			wd = 0;
		}
    	if (idx == 16) {
    		// SPI buffer full, send data
			_spi_transfer_start(tft_disp_spi, bits, 0);

			bits = 0;
    		idx = 0;
			bidx = 0;
    	}
    }
    if (bits > 0) _spi_transfer_start(tft_disp_spi, bits, 0);
}

// Set the address window for display write & read commands, display must be selected
//---------------------------------------------------------------------------------------------------
static void IRAM_ATTR disp_spi_transfer_addrwin(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2) {
	uint32_t wd;

    taskDISABLE_INTERRUPTS();
	// Wait for SPI bus ready
	while (tft_disp_spi->host->hw->cmd.usr);
    gpio_set_level(PIN_NUM_DC, 0);

	tft_disp_spi->host->hw->data_buf[0] = (uint32_t)TFT_CASET;
	tft_disp_spi->host->hw->user.usr_mosi_highpart = 0;
	tft_disp_spi->host->hw->mosi_dlen.usr_mosi_dbitlen = 7;
	tft_disp_spi->host->hw->user.usr_mosi = 1;
	tft_disp_spi->host->hw->miso_dlen.usr_miso_dbitlen = 0;
	tft_disp_spi->host->hw->user.usr_miso = 0;

	tft_disp_spi->host->hw->cmd.usr = 1; // Start transfer

	wd = (uint32_t)(x1>>8);
	wd |= (uint32_t)(x1&0xff) << 8;
	wd |= (uint32_t)(x2>>8) << 16;
	wd |= (uint32_t)(x2&0xff) << 24;

	while (tft_disp_spi->host->hw->cmd.usr); // wait transfer end
	gpio_set_level(PIN_NUM_DC, 1);
	tft_disp_spi->host->hw->data_buf[0] = wd;
	tft_disp_spi->host->hw->mosi_dlen.usr_mosi_dbitlen = 31;
	tft_disp_spi->host->hw->cmd.usr = 1; // Start transfer

    while (tft_disp_spi->host->hw->cmd.usr);
    gpio_set_level(PIN_NUM_DC, 0);
    tft_disp_spi->host->hw->data_buf[0] = (uint32_t)TFT_PASET;
	tft_disp_spi->host->hw->mosi_dlen.usr_mosi_dbitlen = 7;
	tft_disp_spi->host->hw->cmd.usr = 1; // Start transfer

	wd = (uint32_t)(y1>>8);
	wd |= (uint32_t)(y1&0xff) << 8;
	wd |= (uint32_t)(y2>>8) << 16;
	wd |= (uint32_t)(y2&0xff) << 24;

	while (tft_disp_spi->host->hw->cmd.usr);
	gpio_set_level(PIN_NUM_DC, 1);

	tft_disp_spi->host->hw->data_buf[0] = wd;
	tft_disp_spi->host->hw->mosi_dlen.usr_mosi_dbitlen = 31;
	tft_disp_spi->host->hw->cmd.usr = 1; // Start transfer
	while (tft_disp_spi->host->hw->cmd.usr);
    taskENABLE_INTERRUPTS();
}

// Convert color to gray scale
//----------------------------------------------
static color_t IRAM_ATTR color2gs(color_t color)
{
	color_t _color;
    float gs_clr = GS_FACT_R * color.r + GS_FACT_G * color.g + GS_FACT_B * color.b;
    if (gs_clr > 255) gs_clr = 255;

    _color.r = (uint8_t)gs_clr;
    _color.g = (uint8_t)gs_clr;
    _color.b = (uint8_t)gs_clr;

    return _color;
}

// Set display pixel at given coordinates to given color
//------------------------------------------------------------------------
void IRAM_ATTR drawPixel(int16_t x, int16_t y, color_t color, uint8_t sel)
{
	if (!(tft_disp_spi->cfg.flags & LB_SPI_DEVICE_HALFDUPLEX)) return;

	if (sel) {
		if (disp_select()) return;
	}
	else wait_trans_finish(1);

	uint32_t wd = 0;
    color_t _color = color;
	if (tft_gray_scale) _color = color2gs(color);

    taskDISABLE_INTERRUPTS();
	disp_spi_transfer_addrwin(x, x+1, y, y+1);

	// Send RAM WRITE command
    gpio_set_level(PIN_NUM_DC, 0);
    tft_disp_spi->host->hw->data_buf[0] = (uint32_t)TFT_RAMWR;
	tft_disp_spi->host->hw->mosi_dlen.usr_mosi_dbitlen = 7;
	tft_disp_spi->host->hw->cmd.usr = 1;		// Start transfer
	while (tft_disp_spi->host->hw->cmd.usr);	// Wait for SPI bus ready

	wd = (uint32_t)_color.r;
	wd |= (uint32_t)_color.g << 8;
	wd |= (uint32_t)_color.b << 16;

    // Set DC to 1 (data mode);
	gpio_set_level(PIN_NUM_DC, 1);

	tft_disp_spi->host->hw->data_buf[0] = wd;
	tft_disp_spi->host->hw->mosi_dlen.usr_mosi_dbitlen = 23;
	tft_disp_spi->host->hw->cmd.usr = 1;		// Start transfer
	while (tft_disp_spi->host->hw->cmd.usr);	// Wait for SPI bus ready

    taskENABLE_INTERRUPTS();
   if (sel) disp_deselect();
}

//-----------------------------------------------------------
static void IRAM_ATTR _dma_send(uint8_t *data, uint32_t size)
{
    //Fill DMA descriptors
    spi_lobo_dmaworkaround_transfer_active(tft_disp_spi->host->dma_chan); //mark channel as active
    spi_lobo_setup_dma_desc_links(tft_disp_spi->host->dmadesc_tx, size, data, false);
    tft_disp_spi->host->hw->user.usr_mosi_highpart=0;
    tft_disp_spi->host->hw->dma_out_link.addr=(int)(&tft_disp_spi->host->dmadesc_tx[0]) & 0xFFFFF;
    tft_disp_spi->host->hw->dma_out_link.start=1;
    tft_disp_spi->host->hw->user.usr_mosi_highpart=0;

	tft_disp_spi->host->hw->mosi_dlen.usr_mosi_dbitlen = (size * 8) - 1;

	_dma_sending = 1;
	// Start transfer
	tft_disp_spi->host->hw->cmd.usr = 1;
}

//---------------------------------------------------------------------------
static void IRAM_ATTR _direct_send(color_t *color, uint32_t len, uint8_t rep)
{
	uint32_t cidx = 0;	// color buffer index
	uint32_t wd = 0;
	int idx = 0;
	int bits = 0;
	int wbits = 0;

    taskDISABLE_INTERRUPTS();
	color_t _color = color[0];
	if ((rep) && (tft_gray_scale)) _color = color2gs(color[0]);

	while (len) {
		// ** Get color data from color buffer **
		if (rep == 0) {
			if (tft_gray_scale) _color = color2gs(color[cidx]);
			else _color = color[cidx];
		}

		wd |= (uint32_t)_color.r << wbits;
		wbits += 8;
		if (wbits == 32) {
			bits += wbits;
			wbits = 0;
			tft_disp_spi->host->hw->data_buf[idx++] = wd;
			wd = 0;
		}
		wd |= (uint32_t)_color.g << wbits;
		wbits += 8;
		if (wbits == 32) {
			bits += wbits;
			wbits = 0;
			tft_disp_spi->host->hw->data_buf[idx++] = wd;
			wd = 0;
		}
		wd |= (uint32_t)_color.b << wbits;
		wbits += 8;
		if (wbits == 32) {
			bits += wbits;
			wbits = 0;
			tft_disp_spi->host->hw->data_buf[idx++] = wd;
			wd = 0;
		}
    	len--;					// Decrement colors counter
        if (rep == 0) cidx++;	// if not repeating color, increment color buffer index
    }
	if (bits) {
		while (tft_disp_spi->host->hw->cmd.usr);						// Wait for SPI bus ready
		tft_disp_spi->host->hw->mosi_dlen.usr_mosi_dbitlen = bits-1;	// set number of bits to be sent
        tft_disp_spi->host->hw->cmd.usr = 1;							// Start transfer
	}
    taskENABLE_INTERRUPTS();
}

// ================================================================
// === Main function to send data to display ======================
// If  rep==true:  repeat sending color data to display 'len' times
// If rep==false:  send 'len' color data from color buffer to display
// ** Device must already be selected and address window set **
// ================================================================
//----------------------------------------------------------------------------------------------
static void IRAM_ATTR _TFT_pushColorRep(color_t *color, uint32_t len, uint8_t rep, uint8_t wait)
{
	if (len == 0) return;
	if (!(tft_disp_spi->cfg.flags & LB_SPI_DEVICE_HALFDUPLEX)) return;

	// Send RAM WRITE command
    gpio_set_level(PIN_NUM_DC, 0);
    tft_disp_spi->host->hw->data_buf[0] = (uint32_t)TFT_RAMWR;
	tft_disp_spi->host->hw->mosi_dlen.usr_mosi_dbitlen = 7;
	tft_disp_spi->host->hw->cmd.usr = 1;		// Start transfer
	while (tft_disp_spi->host->hw->cmd.usr);	// Wait for SPI bus ready

	gpio_set_level(PIN_NUM_DC, 1);								// Set DC to 1 (data mode);

	if ((len*24) <= 512) {

		_direct_send(color, len, rep);

	}
	else if (rep == 0)  {
		// ==== use DMA transfer ====
		// ** Prepare data
		if (tft_gray_scale) {
			for (int n=0; n<len; n++) {
				color[n] = color2gs(color[n]);
			}
	    }

	    _dma_send((uint8_t *)color, len*3);
	}
	else {
		// ==== Repeat color, more than 512 bits total ====

		color_t _color;
		uint32_t buf_colors;
		int buf_bytes, to_send;

		/*
		to_send = len;
		while (to_send > 0) {
			wait_trans_finish(0);
			_direct_send(color, ((to_send > 21) ? 21 : to_send), rep);
			to_send -= 21;
		}
		*/

		buf_colors = ((len > (tft_width*2)) ? (tft_width*2) : len);
		buf_bytes = buf_colors * 3;

		// Prepare color buffer of maximum 2 color lines
		trans_cline = heap_caps_malloc(buf_bytes, MALLOC_CAP_DMA);
		if (trans_cline == NULL) return;

		// Prepare fill color
		if (tft_gray_scale) _color = color2gs(color[0]);
		else _color = color[0];

		// Fill color buffer with fill color
		for (uint32_t i=0; i<buf_colors; i++) {
			trans_cline[i] = _color;
		}

		// Send 'len' colors
		to_send = len;
		while (to_send > 0) {
			wait_trans_finish(0);
			_dma_send((uint8_t *)trans_cline, ((to_send > buf_colors) ? buf_bytes : (to_send*3)));
			to_send -= buf_colors;
		}
	}

	if (wait) wait_trans_finish(1);
}

// Write 'len' color data to TFT 'window' (x1,y2),(x2,y2)
//-------------------------------------------------------------------------------------------
void IRAM_ATTR TFT_pushColorRep(int x1, int y1, int x2, int y2, color_t color, uint32_t len)
{
	if (disp_select() != ESP_OK) return;

	// ** Send address window **
	disp_spi_transfer_addrwin(x1, x2, y1, y2);

	_TFT_pushColorRep(&color, len, 1, 1);

	disp_deselect();
}

// Write 'len' color data to TFT 'window' (x1,y2),(x2,y2) from given buffer
// ** Device must already be selected **
//-----------------------------------------------------------------------------------
void IRAM_ATTR send_data(int x1, int y1, int x2, int y2, uint32_t len, color_t *buf)
{
	// ** Send address window **
	disp_spi_transfer_addrwin(x1, x2, y1, y2);
	_TFT_pushColorRep(buf, len, 0, 0);
}

// Reads 'len' pixels/colors from the TFT's GRAM 'window'
// 'buf' is an array of bytes with 1st byte reserved for reading 1 dummy byte
// and the rest is actually an array of color_t values
//--------------------------------------------------------------------------------------------
int IRAM_ATTR read_data(int x1, int y1, int x2, int y2, int len, uint8_t *buf, uint8_t set_sp)
{
	spi_lobo_transaction_t t;
	uint32_t current_clock = 0;

    memset(&t, 0, sizeof(t));  //Zero out the transaction
	memset(buf, 0, len*sizeof(color_t));

	if (set_sp) {
		if (disp_deselect() != ESP_OK) return -1;
		// Change spi clock if needed
		current_clock = spi_lobo_get_speed(tft_disp_spi);
		if (tft_max_rdclock < current_clock) spi_lobo_set_speed(tft_disp_spi, tft_max_rdclock);
	}

	if (disp_select() != ESP_OK) return -2;

	// ** Send address window **
	disp_spi_transfer_addrwin(x1, x2, y1, y2);

    // ** GET pixels/colors **
	disp_spi_transfer_cmd(TFT_RAMRD);

    t.length=0;                //Send nothing
    t.tx_buffer=NULL;
    t.rxlength=8*((len*3)+1);  //Receive size in bits
    t.rx_buffer=buf;
    //t.user = (void*)1;

	esp_err_t res = spi_lobo_transfer_data(tft_disp_spi, &t); // Receive using direct mode

	disp_deselect();

	if (set_sp) {
		// Restore spi clock if needed
		if (tft_max_rdclock < current_clock) spi_lobo_set_speed(tft_disp_spi, current_clock);
	}

    return res;
}

// Reads one pixel/color from the TFT's GRAM at position (x,y)
//-----------------------------------------------
color_t IRAM_ATTR readPixel(int16_t x, int16_t y)
{
    uint8_t color_buf[sizeof(color_t)+1] = {0};

    read_data(x, y, x+1, y+1, 1, color_buf, 1);

    color_t color;
	color.r = color_buf[1];
	color.g = color_buf[2];
	color.b = color_buf[3];
	return color;
}

// get 16-bit data from touch controller for specified type
// ** Touch device must already be selected **
//----------------------------------------
int IRAM_ATTR touch_get_data(uint8_t type)
{
    /*
    esp_err_t ret;
    spi_lobo_transaction_t t;
    memset(&t, 0, sizeof(t));            //Zero out the transaction
    uint8_t rxdata[2] = {0};

    // send command byte & receive 2 byte response
    t.rxlength=8*2;
    t.rx_buffer=&rxdata;
    t.command = type;

    ret = spi_lobo_transfer_data(tft_ts_spi, &t);    // Transmit using direct mode

    if (ret != ESP_OK) return -1;
    return (((int)(rxdata[0] << 8) | (int)(rxdata[1])) >> 4);
    */
    spi_lobo_device_select(tft_ts_spi, 0);

    tft_ts_spi->host->hw->data_buf[0] = type;
    _spi_transfer_start(tft_ts_spi, 24, 24);
    uint16_t res = (uint16_t)(tft_ts_spi->host->hw->data_buf[0] >> 8);

    spi_lobo_device_deselect(tft_ts_spi);

    return res;
}

// ==== STMPE610 ===============================================================


// ----- STMPE610 --------------------------------------------------------------------------

// Send 1 byte display command, display must be selected
//---------------------------------------------------------
static void IRAM_ATTR stmpe610_write_reg(uint8_t reg, uint8_t val) {

    spi_lobo_device_select(tft_ts_spi, 0);

    tft_ts_spi->host->hw->data_buf[0] = (val << 8) | reg;
    _spi_transfer_start(tft_ts_spi, 16, 0);

    spi_lobo_device_deselect(tft_ts_spi);
}

//-----------------------------------------------
static uint8_t IRAM_ATTR stmpe610_read_byte(uint8_t reg) {
    spi_lobo_device_select(tft_ts_spi, 0);

    tft_ts_spi->host->hw->data_buf[0] = (reg << 8) | (reg | 0x80);
    _spi_transfer_start(tft_ts_spi, 16, 16);
    uint8_t res = tft_ts_spi->host->hw->data_buf[0] >> 8;

    spi_lobo_device_deselect(tft_ts_spi);
    return res;
}

//-----------------------------------------
static uint16_t IRAM_ATTR stmpe610_read_word(uint8_t reg) {
    spi_lobo_device_select(tft_ts_spi, 0);

    tft_ts_spi->host->hw->data_buf[0] = ((((reg+1) << 8) | ((reg+1) | 0x80)) << 16) | (reg << 8) | (reg | 0x80);
    _spi_transfer_start(tft_ts_spi, 32, 32);
    uint16_t res = (uint16_t)(tft_ts_spi->host->hw->data_buf[0] & 0xFF00);
    res |= (uint16_t)(tft_ts_spi->host->hw->data_buf[0] >> 24);

    spi_lobo_device_deselect(tft_ts_spi);
    return res;
}

//-----------------------
uint32_t stmpe610_getID()
{
    uint16_t tid = stmpe610_read_word(0);
    uint8_t tver = stmpe610_read_byte(2);
    return (tid << 8) | tver;
}

//==================
void stmpe610_Init()
{
    stmpe610_write_reg(STMPE610_REG_SYS_CTRL1, 0x02);        // Software chip reset
    vTaskDelay(10 / portTICK_RATE_MS);

    stmpe610_write_reg(STMPE610_REG_SYS_CTRL2, 0x04);        // Temperature sensor clock off, GPIO clock off, touch clock on, ADC clock on

    stmpe610_write_reg(STMPE610_REG_INT_EN, 0x00);           // Don't Interrupt on INT pin

    stmpe610_write_reg(STMPE610_REG_ADC_CTRL1, 0x48);        // ADC conversion time = 80 clock ticks, 12-bit ADC, internal voltage refernce
    vTaskDelay(2 / portTICK_RATE_MS);
    stmpe610_write_reg(STMPE610_REG_ADC_CTRL2, 0x01);        // ADC speed 3.25MHz
    stmpe610_write_reg(STMPE610_REG_GPIO_AF, 0x00);          // GPIO alternate function - OFF
    stmpe610_write_reg(STMPE610_REG_TSC_CFG, 0xE3);          // Averaging 8, touch detect delay 1ms, panel driver settling time 1ms
    stmpe610_write_reg(STMPE610_REG_FIFO_TH, 0x01);          // FIFO threshold = 1
    stmpe610_write_reg(STMPE610_REG_FIFO_STA, 0x01);         // FIFO reset enable
    stmpe610_write_reg(STMPE610_REG_FIFO_STA, 0x00);         // FIFO reset disable
    stmpe610_write_reg(STMPE610_REG_TSC_FRACT_XYZ, 0x07);    // Z axis data format
    stmpe610_write_reg(STMPE610_REG_TSC_I_DRIVE, 0x01);      // max 50mA touchscreen line current
    stmpe610_write_reg(STMPE610_REG_TSC_CTRL, 0x30);         // X&Y&Z, 16 reading window
    stmpe610_write_reg(STMPE610_REG_TSC_CTRL, 0x31);         // X&Y&Z, 16 reading window, TSC enable
    stmpe610_write_reg(STMPE610_REG_INT_STA, 0xFF);          // Clear all interrupts
    stmpe610_write_reg(STMPE610_REG_INT_CTRL, 0x00);         // Level interrupt, disable interrupts
}

//===========================================================
int stmpe610_get_touch(uint16_t *x, uint16_t *y, uint16_t *z)
{
	if (!(stmpe610_read_byte(STMPE610_REG_TSC_CTRL) & 0x80)) return 0;

    // Get touch data
    uint8_t fifo_size = stmpe610_read_byte(STMPE610_REG_FIFO_SIZE);
    while (fifo_size < 2) {
    	if (!(stmpe610_read_byte(STMPE610_REG_TSC_CTRL) & 0x80)) return 0;
        fifo_size = stmpe610_read_byte(STMPE610_REG_FIFO_SIZE);
    }
    while (fifo_size > 120) {
    	if (!(stmpe610_read_byte(STMPE610_REG_TSC_CTRL) & 0x80)) return 0;
        *x = stmpe610_read_word(STMPE610_REG_TSC_DATA_X);
        *y = stmpe610_read_word(STMPE610_REG_TSC_DATA_Y);
        *z = stmpe610_read_byte(STMPE610_REG_TSC_DATA_Z);
        fifo_size = stmpe610_read_byte(STMPE610_REG_FIFO_SIZE);
    }
    for (uint8_t i=0; i < (fifo_size-1); i++) {
        *x = stmpe610_read_word(STMPE610_REG_TSC_DATA_X);
        *y = stmpe610_read_word(STMPE610_REG_TSC_DATA_Y);
        *z = stmpe610_read_byte(STMPE610_REG_TSC_DATA_Z);
    }

    *x = 4096 - *x;
    /*
    // Clear the rest of the fifo
    {
        stmpe610_write_reg(STMPE610_REG_FIFO_STA, 0x01);		// FIFO reset enable
        stmpe610_write_reg(STMPE610_REG_FIFO_STA, 0x00);		// FIFO reset disable
    }
    */
	return 1;
}

// ==== STMPE610 ===========================================================================


// Find maximum spi clock for successful read from display RAM
// ** Must be used AFTER the display is initialized **
//======================
uint32_t find_rd_speed()
{
	esp_err_t ret;
	color_t color;
	uint32_t max_speed = 1000000;
    uint32_t change_speed, cur_speed;
    int line_check;
    color_t *color_line = NULL;
    uint8_t *line_rdbuf = NULL;
    uint8_t gs = tft_gray_scale;

    tft_gray_scale = 0;
    cur_speed = spi_lobo_get_speed(tft_disp_spi);

	color_line = malloc(tft_width*3);
    if (color_line == NULL) goto exit;

    line_rdbuf = malloc((tft_width*3)+1);
	if (line_rdbuf == NULL) goto exit;

	color_t *rdline = (color_t *)(line_rdbuf+1);

	// Fill test line with colors
	color = (color_t){0xEC,0xA8,0x74};
	for (int x=0; x<tft_width; x++) {
		color_line[x] = color;
	}

	// Find maximum read spi clock
	for (uint32_t speed=2000000; speed<=cur_speed; speed += 1000000) {
		change_speed = spi_lobo_set_speed(tft_disp_spi, speed);
		if (change_speed == 0) goto exit;

		memset(line_rdbuf, 0, tft_width*sizeof(color_t)+1);

		if (disp_select()) goto exit;
		// Write color line
		send_data(0, tft_height/2, tft_width-1, tft_height/2, tft_width, color_line);
		if (disp_deselect()) goto exit;

		// Read color line
		ret = read_data(0, tft_height/2, tft_width-1, tft_height/2, tft_width, line_rdbuf, 0);

		// Compare
		line_check = 0;
		if (ret == ESP_OK) {
			for (int y=0; y<tft_width; y++) {
				if ((color_line[y].r & 0xFC) != (rdline[y].r & 0xFC)) line_check = 1;
				else if ((color_line[y].g & 0xFC) != (rdline[y].g & 0xFC)) line_check = 1;
				else if ((color_line[y].b & 0xFC) != (rdline[y].b & 0xFC)) line_check =  1;
				if (line_check) break;
			}
		}
		else line_check = ret;

		if (line_check) break;
		max_speed = speed;
	}

exit:
    tft_gray_scale = gs;
	if (line_rdbuf) free(line_rdbuf);
	if (color_line) free(color_line);

	// restore spi clk
	change_speed = spi_lobo_set_speed(tft_disp_spi, cur_speed);

	return max_speed;
}

//---------------------------------------------------------------------------
// Companion code to the initialization table.
// Reads and issues a series of LCD commands stored in byte array
//---------------------------------------------------------------------------
static void commandList(spi_lobo_device_handle_t spi, const uint8_t *addr) {
  uint8_t  numCommands, numArgs, cmd;
  uint16_t ms;

  numCommands = *addr++;				// Number of commands to follow
  while(numCommands--) {				// For each command...
    cmd = *addr++;						// save command
    numArgs  = *addr++;					// Number of args to follow
    ms       = numArgs & TFT_CMD_DELAY;	// If high bit set, delay follows args
    numArgs &= ~TFT_CMD_DELAY;			// Mask out delay bit

	disp_spi_transfer_cmd_data(cmd, (uint8_t *)addr, numArgs);

	addr += numArgs;

    if(ms) {
      ms = *addr++;              // Read post-command delay time (ms)
      if(ms == 255) ms = 500;    // If 255, delay for 500 ms
	  vTaskDelay(ms / portTICK_RATE_MS);
    }
  }
}

//==================================
void _tft_setRotation(uint8_t rot) {
	uint8_t rotation = rot & 3; // can't be higher than 3
	uint8_t send = 1;
	uint8_t madctl = 0;
	uint16_t tmp;

    if ((rotation & 1)) {
        // in landscape modes must be width > height
        if (tft_width < tft_height) {
            tmp = tft_width;
            tft_width  = tft_height;
            tft_height = tmp;
        }
    }
    else {
        // in portrait modes must be width < height
        if (tft_width > tft_height) {
            tmp = tft_width;
            tft_width  = tft_height;
            tft_height = tmp;
        }
    }
    #if TFT_INVERT_ROTATION
    switch (rotation) {
        case PORTRAIT:
        madctl = (MADCTL_MV | TFT_RGB_BGR);
        break;
        case LANDSCAPE:
        madctl = (MADCTL_MX | TFT_RGB_BGR);
        break;
        case PORTRAIT_FLIP:
        madctl = (MADCTL_MV | TFT_RGB_BGR);
        break;
        case LANDSCAPE_FLIP:
        madctl = (MADCTL_MY | TFT_RGB_BGR);
        break;
    }
    #elif TFT_INVERT_ROTATION1
    switch (rotation) {
        case PORTRAIT:
        madctl = (MADCTL_MY | MADCTL_MX | TFT_RGB_BGR);
        break;
        case LANDSCAPE:
        madctl = (MADCTL_MY | MADCTL_MV | TFT_RGB_BGR);
        break;
        case PORTRAIT_FLIP:
        madctl = (TFT_RGB_BGR);
        break;
        case LANDSCAPE_FLIP:
        madctl = (MADCTL_MX | MADCTL_MV | TFT_RGB_BGR);
        break;
    }
    #elif TFT_INVERT_ROTATION2
    switch (rotation) {
        case PORTRAIT:
        madctl = (MADCTL_MX | MADCTL_MV | TFT_RGB_BGR);
        break;
        case LANDSCAPE:
        madctl = (TFT_RGB_BGR);
        break;
        case PORTRAIT_FLIP:
        madctl = (MADCTL_MY | MADCTL_MV | TFT_RGB_BGR);
        break;
        case LANDSCAPE_FLIP:
        madctl = (MADCTL_MY | MADCTL_MX | TFT_RGB_BGR);
        break;
    }
    #else
    switch (rotation) {
        case PORTRAIT:
        madctl = (MADCTL_MX | TFT_RGB_BGR);
        break;
        case LANDSCAPE:
        madctl = (MADCTL_MV | TFT_RGB_BGR);
        break;
        case PORTRAIT_FLIP:
        madctl = (MADCTL_MY | TFT_RGB_BGR);
        break;
        case LANDSCAPE_FLIP:
        madctl = (MADCTL_MX | MADCTL_MY | MADCTL_MV | TFT_RGB_BGR);
        break;
    }
    #endif
	if (send) {
		if (disp_select() == ESP_OK) {
			disp_spi_transfer_cmd_data(TFT_MADCTL, &madctl, 1);
			disp_deselect();
		}
	}

}

//=================
void TFT_PinsInit()
{
    // Route all used pins to GPIO control
    gpio_pad_select_gpio(PIN_NUM_CS);
    gpio_pad_select_gpio(PIN_NUM_MISO);
    gpio_pad_select_gpio(PIN_NUM_MOSI);
    gpio_pad_select_gpio(PIN_NUM_CLK);
    gpio_pad_select_gpio(PIN_NUM_DC);

    gpio_set_direction(PIN_NUM_MISO, GPIO_MODE_INPUT);
    gpio_set_pull_mode(PIN_NUM_MISO, GPIO_PULLUP_ONLY);
    gpio_set_direction(PIN_NUM_CS, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_NUM_MOSI, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_NUM_CLK, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_NUM_DC, GPIO_MODE_OUTPUT);
    gpio_set_level(PIN_NUM_DC, 0);
#if USE_TOUCH
    gpio_pad_select_gpio(PIN_NUM_TCS);
    gpio_set_direction(PIN_NUM_TCS, GPIO_MODE_OUTPUT);
#endif
#if PIN_NUM_BCKL
    gpio_pad_select_gpio(PIN_NUM_BCKL);
    gpio_set_direction(PIN_NUM_BCKL, GPIO_MODE_OUTPUT);
    gpio_set_level(PIN_NUM_BCKL, PIN_BCKL_OFF);
#endif

#if PIN_NUM_RST
    gpio_pad_select_gpio(PIN_NUM_RST);
    gpio_set_direction(PIN_NUM_RST, GPIO_MODE_OUTPUT);
    gpio_set_level(PIN_NUM_RST, 0);
#endif
}

// Initialize the display
// ====================
void TFT_display_init()
{
    esp_err_t ret;

#if PIN_NUM_RST
    //Reset the display
    gpio_set_level(PIN_NUM_RST, 0);
    vTaskDelay(20 / portTICK_RATE_MS);
    gpio_set_level(PIN_NUM_RST, 1);
    vTaskDelay(150 / portTICK_RATE_MS);
#endif

    ret = disp_select();
    assert(ret==ESP_OK);
    //Send all the initialization commands
	if (tft_disp_type == DISP_TYPE_ILI9341) {
		commandList(tft_disp_spi, ILI9341_init);
	}
	else if (tft_disp_type == DISP_TYPE_ILI9488) {
		commandList(tft_disp_spi, ILI9488_init);
	}
	else if (tft_disp_type == DISP_TYPE_ST7789V) {
		commandList(tft_disp_spi, ST7789V_init);
	}
	else if (tft_disp_type == DISP_TYPE_ST7735) {
		commandList(tft_disp_spi, STP7735_init);
	}
	else if (tft_disp_type == DISP_TYPE_ST7735R) {
		commandList(tft_disp_spi, STP7735R_init);
		commandList(tft_disp_spi, Rcmd2green);
		commandList(tft_disp_spi, Rcmd3);
	}
	else if (tft_disp_type == DISP_TYPE_ST7735B) {
		commandList(tft_disp_spi, STP7735R_init);
		commandList(tft_disp_spi, Rcmd2red);
		commandList(tft_disp_spi, Rcmd3);
	    uint8_t dt = 0xC0;
		disp_spi_transfer_cmd_data(TFT_MADCTL, &dt, 1);
	}
	else assert(0);

    ret = disp_deselect();
	assert(ret==ESP_OK);

	// Clear screen
    _tft_setRotation(PORTRAIT);
	TFT_pushColorRep(TFT_STATIC_WIDTH_OFFSET, TFT_STATIC_HEIGHT_OFFSET, tft_width + TFT_STATIC_WIDTH_OFFSET -1, tft_height + TFT_STATIC_HEIGHT_OFFSET -1, (color_t){0,0,0}, (uint32_t)(tft_height*tft_width));

	///Enable backlight
#if PIN_NUM_BCKL
    gpio_set_level(PIN_NUM_BCKL, PIN_BCKL_ON);
#endif
}


