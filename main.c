#define _GNU_SOURCE
#include <eglib.h>
#include <eglib/display/frame_buffer.h>
#include <eglib/display/st7789.h>
#include <eglib/hal/four_wire_spi/libopencm3_stm32f4.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/spi.h>
#include <stdio.h>
#include "usart.h"
#include "pms5003.h"
#include "mh-z19c.h"
#include <libopencm3/stm32/usart.h>
#include <stdio.h>
#include <stdlib.h>

//
// PMS5003
//

#define PMS5003_USART_RCC_USART_PORT RCC_GPIOA
#define PMS5003_USART_RCC_USART RCC_USART1
#define PMS5003_USART_RCC_USART_TX RCC_GPIOA
#define PMS5003_USART_PORT_USART_TX GPIOA
#define PMS5003_USART_GPIO_USART_TX GPIO15
#define PMS5003_USART_RCC_USART_RX RCC_GPIOB
#define PMS5003_USART_PORT_USART_RX GPIOB
#define PMS5003_USART_GPIO_USART_RX GPIO3
#define PMS5003_USART USART1
#define PMS5003_USART_BAUDRATE 9600
#define PMS5003_USART_DATABITS 8
#define PMS5003_USART_STOPBITS USART_STOPBITS_1
#define PMS5003_USART_MODE USART_MODE_TX_RX
#define PMS5003_USART_PARITY USART_PARITY_NONE
#define PMS5003_USART_FLOW_CONTROL USART_FLOWCONTROL_NONE
#define PMS5003_RCC_SET RCC_GPIOB
#define PMS5003_PORT_SET GPIOB
#define PMS5003_GPIO_SET GPIO0
#define PMS5003_RCC_RESET RCC_GPIOB
#define PMS5003_PORT_RESET GPIOB
#define PMS5003_GPIO_RESET GPIO1

int read_serial_pms5003(uint8_t *c);

int read_serial_pms5003(uint8_t *c) {
	*c = usart_recv_blocking(PMS5003_USART);
	return 0;
}

int write_serial_pms5003(uint8_t c);

int write_serial_pms5003(uint8_t c) {
	usart_send_blocking(PMS5003_USART, c);
	return 0;
}

//
// MH-Z19C
//

#define MH_Z19C_USART_RCC_USART_PORT RCC_GPIOA
#define MH_Z19C_USART_RCC_USART RCC_USART2
#define MH_Z19C_USART_RCC_USART_TX RCC_GPIOA
#define MH_Z19C_USART_PORT_USART_TX GPIOA
#define MH_Z19C_USART_GPIO_USART_TX GPIO2
#define MH_Z19C_USART_RCC_USART_RX RCC_GPIOA
#define MH_Z19C_USART_PORT_USART_RX GPIOA
#define MH_Z19C_USART_GPIO_USART_RX GPIO3
#define MH_Z19C_USART USART2
#define MH_Z19C_USART_BAUDRATE 9600
#define MH_Z19C_USART_DATABITS 8
#define MH_Z19C_USART_STOPBITS USART_STOPBITS_1
#define MH_Z19C_USART_MODE USART_MODE_TX_RX
#define MH_Z19C_USART_PARITY USART_PARITY_NONE
#define MH_Z19C_USART_FLOW_CONTROL USART_FLOWCONTROL_NONE
#define MH_Z19C_RCC_HD RCC_GPIOB
#define MH_Z19C_PORT_HD GPIOB
#define MH_Z19C_GPIO_HD GPIO2

int read_serial_mh_z19c(uint8_t *c);

int read_serial_mh_z19c(uint8_t *c) {
	*c = usart_recv_blocking(MH_Z19C_USART);
	return 0;
}

int write_serial_mh_z19c(uint8_t c);

int write_serial_mh_z19c(uint8_t c) {
	usart_send_blocking(MH_Z19C_USART, c);
	return 0;
}

//
// Display
//

void display_message(eglib_t *eglib, const char *msg);

void display_message(eglib_t *eglib, const char *msg) {
	eglib_SetIndexColor(eglib, 0, 255, 255, 255);
	eglib_ClearScreen(eglib);
	eglib_SetIndexColor(eglib, 0, 0, 0, 0);
	eglib_SetFont(eglib, &font_FreeFont_FreeSans_36px);
	eglib_DrawTextCentered(eglib, eglib_GetWidth(eglib)/2, eglib_GetHeight(eglib)/2, msg);
	eglib_FrameBuffer_Send(eglib);
}

void display_error(eglib_t *eglib, const char *title, const char *msg);

void display_error(eglib_t *eglib, const char *title, const char *msg) {
	eglib_SetIndexColor(eglib, 0, 255, 0, 0);
	eglib_ClearScreen(eglib);
	eglib_SetIndexColor(eglib, 0, 0, 0, 0);
	eglib_SetFont(eglib, &font_FreeFont_FreeSans_36px);
	eglib_DrawTextCentered(eglib, eglib_GetWidth(eglib)/2, eglib_GetHeight(eglib)/2 - 16, title);

	eglib_SetFont(eglib, &font_FreeFont_FreeSans_16px);
	eglib_DrawTextCentered(eglib, eglib_GetWidth(eglib)/2, eglib_GetHeight(eglib)/2, msg);
	eglib_FrameBuffer_Send(eglib);
}

void draw_measurement(
	eglib_t *eglib,
	coordinate_t x, coordinate_t y,
	coordinate_t width, coordinate_t height,
	color_t *background,
	const char *title, uint16_t value, const char *unit
);

void draw_measurement(
	eglib_t *eglib,
	coordinate_t x, coordinate_t y,
	coordinate_t width, coordinate_t height,
	color_t *background,
	const char *title, uint16_t value, const char *unit
) {
	char *buff;
	coordinate_t text_width;
	coordinate_t x_offset;
	coordinate_t y_offset;

	eglib_SetIndexColor(eglib, 0, background->r, background->g, background->b);
	eglib_DrawBox(eglib, x, y, width, height);

	if(asprintf(&buff, "%d", value) == -1) {
		display_error(eglib, "draw_measurement()", "Low memory!");
		while(true);
	}

	text_width = 0;
	eglib_SetFont(eglib, &font_FreeFont_FreeSans_22px);
	text_width += eglib_GetTextWidth(eglib, title);
	eglib_SetFont(eglib, &font_FreeFont_FreeSans_48px);
	text_width += eglib_GetTextWidth(eglib, buff);
	eglib_SetFont(eglib, &font_FreeFont_FreeSans_22px);
	text_width += eglib_GetTextWidth(eglib, unit);
	x_offset = x + (width - text_width) / 2;

	eglib_SetIndexColor(eglib, 0, 0, 0, 0);

	y_offset = y + height / 2 + font_FreeFont_FreeSans_54px.ascent / 2;

	eglib_SetFont(eglib, &font_FreeFont_FreeSans_22px);
	eglib_DrawText(eglib, x_offset, y_offset, title);
	x_offset += eglib_GetTextWidth(eglib, title);

	eglib_SetFont(eglib, &font_FreeFont_FreeSans_48px);
	eglib_DrawText(eglib, x_offset, y_offset, buff);
	x_offset += eglib_GetTextWidth(eglib, buff);

	eglib_SetFont(eglib, &font_FreeFont_FreeSans_22px);
	eglib_DrawText(eglib, x_offset, y_offset, unit);

	free(buff);
}

//
// Main
//

int main(void) {
	eglib_t eglib;
	eglib_t *eglib_real;
	(void)eglib_real;

	four_wire_spi_libopencm3_stm32f4_config_t  four_wire_spi_libopencm3_stm32f4_config = {
		// rst
		.rcc_rst = RCC_GPIOA,
		.port_rst = GPIOA,
		.gpio_rst = GPIO4,
		// cd
		.rcc_dc = RCC_GPIOA,
		.port_dc = GPIOA,
		.gpio_dc = GPIO0,
		// cs
		.rcc_cs = RCC_GPIOA,
		.port_cs = GPIOA,
		.gpio_cs = GPIO1,
		// spi
		.rcc_spi = RCC_SPI1,
		.spi = SPI1,
		// sck
		.rcc_sck = RCC_GPIOA,
		.port_sck = GPIOA,
		.gpio_sck = GPIO5,
		// mosi
		.rcc_mosi = RCC_GPIOA,
		.port_mosi = GPIOA,
		.gpio_mosi = GPIO7,
	};

	st7789_config_t st7789_config = {
		.width = 240,
		.height = 240,
		.color = ST7789_COLOR_16_BIT,
		.page_address = ST7789_PAGE_ADDRESS_TOP_TO_BOTTOM,
		.colum_address = ST7789_COLUMN_ADDRESS_LEFT_TO_RIGHT,
		.page_column_order = ST7789_PAGE_COLUMN_ORDER_NORMAL,
		.vertical_refresh = ST7789_VERTICAL_REFRESH_TOP_TO_BOTTOM,
		.horizontal_refresh = ST7789_HORIZONTAL_REFRESH_LEFT_TO_RIGHT,
	};
	enum mh_z19c_error mh_z19c_ret;
	enum pms5003_error pm5003_ret;

	//
	// Clock
	//

	rcc_clock_setup_pll(&rcc_hse_25mhz_3v3[RCC_CLOCK_3V3_84MHZ]);

	//
	// eglib
	//

	frame_buffer_config_t frame_buffer_config;
	eglib_real = eglib_Init_FrameBuffer(
		&eglib, &frame_buffer_config,
		&four_wire_spi_libopencm3_stm32f4, &four_wire_spi_libopencm3_stm32f4_config,
		&st7789, &st7789_config
	);
	display_message(&eglib, "");

	eglib_AddUnicodeBlockToFont(
		&font_FreeFont_FreeSans_22px,
		&unicode_block_FreeFont_FreeSans_22px_Latin1Supplement
	);

	eglib_AddUnicodeBlockToFont(
		&font_FreeFont_FreeSans_22px,
		&unicode_block_FreeFont_FreeSans_22px_SuperscriptsAndSubscripts
	);

	//
	// PMS5003
	//

	display_message(&eglib, "PMS5003");

	// SET
	rcc_periph_clock_enable(PMS5003_RCC_SET);
	gpio_mode_setup(
		PMS5003_PORT_SET,
		GPIO_MODE_OUTPUT,
		GPIO_PUPD_NONE,
		PMS5003_GPIO_SET
	);
	gpio_set(PMS5003_PORT_SET, PMS5003_GPIO_SET);

	// RESET
	rcc_periph_clock_enable(PMS5003_RCC_RESET);
	gpio_mode_setup(
		PMS5003_PORT_RESET,
		GPIO_MODE_OUTPUT,
		GPIO_PUPD_NONE,
		PMS5003_GPIO_RESET
	);
	gpio_clear(PMS5003_PORT_RESET, PMS5003_GPIO_RESET);

	// USART
	usart_setup(
		PMS5003_USART_RCC_USART_PORT,
		PMS5003_USART_RCC_USART,
		PMS5003_USART_RCC_USART_TX,
		PMS5003_USART_PORT_USART_TX,
		PMS5003_USART_GPIO_USART_TX,
		PMS5003_USART_RCC_USART_RX,
		PMS5003_USART_PORT_USART_RX,
		PMS5003_USART_GPIO_USART_RX,
		PMS5003_USART,
		PMS5003_USART_BAUDRATE,
		PMS5003_USART_DATABITS,
		PMS5003_USART_STOPBITS,
		PMS5003_USART_MODE,
		PMS5003_USART_PARITY,
		PMS5003_USART_FLOW_CONTROL
	);
	// Required as uC TX takes ~110us to go from low to high
	eglib_DelayUs(eglib_real, 150);

	// RESET
	gpio_set(PMS5003_PORT_RESET, PMS5003_GPIO_RESET);
	// Required as PMS5003 takes a while to start accepting requests after reset
	eglib_DelayMs(eglib_real, 1500);

	// Passive mode
	if((pm5003_ret = pms5003_set_data_mode(PMS5003_DATA_MODE_PASSIVE, write_serial_pms5003, read_serial_pms5003))) {
		display_error(&eglib, "PMS5003", pms5003_strerror(pm5003_ret));
		while(true);
	}

	//
	// MH-Z19C
	//

	display_message(&eglib, "MH-Z19C");

	// Required as MH-Z19C takes a while to boot
	eglib_DelayS(eglib_real, 180);

	// USART
	usart_setup(
		MH_Z19C_USART_RCC_USART_PORT,
		MH_Z19C_USART_RCC_USART,
		MH_Z19C_USART_RCC_USART_TX,
		MH_Z19C_USART_PORT_USART_TX,
		MH_Z19C_USART_GPIO_USART_TX,
		MH_Z19C_USART_RCC_USART_RX,
		MH_Z19C_USART_PORT_USART_RX,
		MH_Z19C_USART_GPIO_USART_RX,
		MH_Z19C_USART,
		MH_Z19C_USART_BAUDRATE,
		MH_Z19C_USART_DATABITS,
		MH_Z19C_USART_STOPBITS,
		MH_Z19C_USART_MODE,
		MH_Z19C_USART_PARITY,
		MH_Z19C_USART_FLOW_CONTROL
	);

	// HD
	rcc_periph_clock_enable(MH_Z19C_RCC_HD);
	gpio_mode_setup(
		MH_Z19C_PORT_HD,
		GPIO_MODE_OUTPUT,
		GPIO_PUPD_NONE,
		MH_Z19C_GPIO_HD
	);
	gpio_set(MH_Z19C_PORT_HD, MH_Z19C_GPIO_HD);

	// Zero Point Calibration
	while((mh_z19c_ret = mh_z19c_self_calibration_for_zero_point(false, write_serial_mh_z19c, read_serial_mh_z19c)))
		display_error(&eglib, "MH-Z19C", mh_z19c_strerror(mh_z19c_ret));

	display_message(&eglib, "Ready!");

	while(true) {
		struct pms5003_measurement measurement;
		uint16_t co2_concentration;

		// if((pm5003_ret = pms5003_get_active_measurement(&measurement, read_serial_pms5003))) {
		if((pm5003_ret = pms5003_get_passive_measurement(&measurement, write_serial_pms5003, read_serial_pms5003))) {
			display_error(&eglib, "PMS5003", pms5003_strerror(pm5003_ret));
			continue;
		}

		mh_z19c_ret = mh_z19c_read(
			&co2_concentration,
			write_serial_mh_z19c,
			read_serial_mh_z19c
		);
		if(mh_z19c_ret) {
			display_error(&eglib, "MH-Z19C", mh_z19c_strerror(mh_z19c_ret));
			continue;
		}


		uint16_t value;
		color_t background;

		value = measurement.pm2_5_cf1;

		if(value <= 11)
			background = (color_t){191, 215, 48};
		else if(value <= 23)
			background = (color_t){101, 179, 69};
		else if(value <= 35)
			background = (color_t){50, 132, 50};
		else if(value <= 41)
			background = (color_t){242, 190, 26};
		else if(value <= 47)
			background = (color_t){247, 147, 30};
		else if(value <= 53)
			background = (color_t){242, 101, 34};
		else if(value <= 58)
			background = (color_t){237, 28, 36};
		else if(value <= 64)
			background = (color_t){177, 17, 23};
		else if(value <= 70)
			background = (color_t){116, 54, 24};
		else
			background = (color_t){180, 63, 151};
		draw_measurement(
			&eglib,
			0, 0,
			eglib_GetWidth(&eglib), eglib_GetHeight(&eglib)  / 3,
			&background,
			"PM₂.₅", value, "µg/m³"
		);

		value = measurement.pm10_cf1;

		if(value <= 16)
			background = (color_t){191, 215, 48};
		else if(value <= 33)
			background = (color_t){101, 179, 69};
		else if(value <= 50)
			background = (color_t){50, 132, 50};
		else if(value <= 58)
			background = (color_t){242, 190, 26};
		else if(value <= 66)
			background = (color_t){247, 147, 30};
		else if(value <= 75)
			background = (color_t){242, 101, 34};
		else if(value <= 83)
			background = (color_t){237, 28, 36};
		else if(value <= 91)
			background = (color_t){177, 17, 23};
		else if(value <= 100)
			background = (color_t){116, 54, 24};
		else
			background = (color_t){180, 63, 151};
		draw_measurement(
			&eglib,
			0, eglib_GetHeight(&eglib)  / 3,
			eglib_GetWidth(&eglib), eglib_GetHeight(&eglib)  / 3,
			&background,
			"PM₁₀", value, "µg/m³"
		);


		// 250-400ppm: Normal background concentration in outdoor ambient air
		if(co2_concentration < 600)
			background = (color_t){191, 215, 48};
		else if(co2_concentration < 800)
			background = (color_t){101, 179, 69};
		// 400-1000ppm: Concentrations typical of occupied indoor spaces with good air exchange
		else if(co2_concentration < 1000)
			background = (color_t){50, 132, 50};
		else if(co2_concentration < 1333)
			background = (color_t){242, 190, 26};
		else if(co2_concentration < 1666)
			background = (color_t){247, 147, 30};
		// 1000-2000ppm: Complaints of drowsiness and poor air.
		else if(co2_concentration < 2000)
			background = (color_t){242, 101, 34};
		else if(co2_concentration < 3000)
			background = (color_t){237, 28, 36};
		else if(co2_concentration < 4000)
			background = (color_t){177, 17, 23};
		// 2000-5000 ppm: Headaches, sleepiness and stagnant, stale, stuffy air. Poor concentration, loss of attention, increased heart rate and slight nausea may also be present.
		else if(co2_concentration < 5000)
			background = (color_t){116, 54, 24};
		// 5000ppm: Workplace exposure limit (as 8-hour TWA) in most jurisdictions.
		else
			background = (color_t){180, 63, 151};
		draw_measurement(
			&eglib,
			0, eglib_GetHeight(&eglib) / 3 * 2,
			eglib_GetWidth(&eglib), eglib_GetHeight(&eglib) / 3,
			&background,
			"CO₂", co2_concentration, "ppm"
		);

		eglib_FrameBuffer_Send(&eglib);
	}
}