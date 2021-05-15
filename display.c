#define _GNU_SOURCE
#include "display.h"
#include <eglib/display/frame_buffer.h>
#include <eglib/display/st7789.h>
#include <eglib/hal/four_wire_spi/libopencm3_stm32f4.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/spi.h>
#include <stdio.h>
#include <stdlib.h>

eglib_t eglib;
static eglib_t *eglib_st7789;

void display_setup() {
	static four_wire_spi_libopencm3_stm32f4_config_t  four_wire_spi_libopencm3_stm32f4_config = {
		// rst
		.rcc_rst = RCC_GPIOB,
		.port_rst = GPIOB,
		.gpio_rst = GPIO1,
		// cd
		.rcc_dc = RCC_GPIOB,
		.port_dc = GPIOB,
		.gpio_dc = GPIO0,
		// cs
		.rcc_cs = RCC_GPIOA,
		.port_cs = GPIOA,
		.gpio_cs = GPIO6,
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
	static st7789_config_t st7789_config = {
		.width = 240,
		.height = 240,
		.color = ST7789_COLOR_16_BIT,
		.page_address = ST7789_PAGE_ADDRESS_TOP_TO_BOTTOM,
		.colum_address = ST7789_COLUMN_ADDRESS_LEFT_TO_RIGHT,
		.page_column_order = ST7789_PAGE_COLUMN_ORDER_NORMAL,
		.vertical_refresh = ST7789_VERTICAL_REFRESH_TOP_TO_BOTTOM,
		.horizontal_refresh = ST7789_HORIZONTAL_REFRESH_LEFT_TO_RIGHT,
	};
	static frame_buffer_config_t frame_buffer_config;

	eglib_st7789 = eglib_Init_FrameBuffer(
		&eglib, &frame_buffer_config,
		&four_wire_spi_libopencm3_stm32f4, &four_wire_spi_libopencm3_stm32f4_config,
		&st7789, &st7789_config
	);

	eglib_AddUnicodeBlockToFont(
		&font_FreeFont_FreeSans_22px,
		&unicode_block_FreeFont_FreeSans_22px_Latin1Supplement
	);
	eglib_AddUnicodeBlockToFont(
		&font_FreeFont_FreeSans_22px,
		&unicode_block_FreeFont_FreeSans_22px_SuperscriptsAndSubscripts
	);
	eglib_AddUnicodeBlockToFont(
		&font_FreeFont_FreeSans_28px,
		&unicode_block_FreeFont_FreeSans_22px_SuperscriptsAndSubscripts
	);

	display_message("ST7789");
}

void display_message(const char *msg) {
	eglib_SetIndexColor(&eglib, 0, 255, 255, 255);
	eglib_ClearScreen(&eglib);
	eglib_SetIndexColor(&eglib, 0, 0, 0, 0);
	eglib_SetFont(&eglib, &font_FreeFont_FreeSans_28px);
	eglib_DrawTextCentered(&eglib, eglib_GetWidth(&eglib)/2, eglib_GetHeight(&eglib)/2, msg);
	eglib_FrameBuffer_Send(&eglib);
}

void display_error(const char *title, const char *msg) {
	eglib_SetIndexColor(&eglib, 0, 255, 0, 0);
	eglib_ClearScreen(&eglib);
	eglib_SetIndexColor(&eglib, 0, 0, 0, 0);
	eglib_SetFont(&eglib, &font_FreeFont_FreeSans_28px);
	eglib_DrawTextCentered(&eglib, eglib_GetWidth(&eglib)/2, eglib_GetHeight(&eglib)/2 - 16, title);

	eglib_SetFont(&eglib, &font_FreeFont_FreeSans_16px);
	eglib_DrawTextCentered(&eglib, eglib_GetWidth(&eglib)/2, eglib_GetHeight(&eglib)/2, msg);
	eglib_FrameBuffer_Send(&eglib);
}

void draw_measurement(
	coordinate_t x, coordinate_t y,
	coordinate_t width, coordinate_t height,
	color_t *background,
	const char *title, uint16_t value, const char *unit
) {
	char *buff;
	coordinate_t text_width;
	coordinate_t x_offset;
	coordinate_t y_offset;

	eglib_SetIndexColor(&eglib, 0, background->r, background->g, background->b);
	eglib_DrawBox(&eglib, x, y, width, height);

	if(asprintf(&buff, "%d", value) == -1) {
		display_error("draw_measurement()", "Low memory!");
		while(true);
	}

	text_width = 0;
	eglib_SetFont(&eglib, &font_FreeFont_FreeSans_22px);
	text_width += eglib_GetTextWidth(&eglib, title);
	eglib_SetFont(&eglib, &font_FreeFont_FreeSans_48px);
	text_width += eglib_GetTextWidth(&eglib, buff);
	eglib_SetFont(&eglib, &font_FreeFont_FreeSans_22px);
	text_width += eglib_GetTextWidth(&eglib, unit);
	x_offset = x + (width - text_width) / 2;

	eglib_SetIndexColor(&eglib, 0, 0, 0, 0);

	y_offset = y + height / 2 + font_FreeFont_FreeSans_54px.ascent / 2;

	eglib_SetFont(&eglib, &font_FreeFont_FreeSans_22px);
	eglib_DrawText(&eglib, x_offset, y_offset, title);
	x_offset += eglib_GetTextWidth(&eglib, title);

	eglib_SetFont(&eglib, &font_FreeFont_FreeSans_48px);
	eglib_DrawText(&eglib, x_offset, y_offset, buff);
	x_offset += eglib_GetTextWidth(&eglib, buff);

	eglib_SetFont(&eglib, &font_FreeFont_FreeSans_22px);
	eglib_DrawText(&eglib, x_offset, y_offset, unit);

	free(buff);
}

void delay_us(uint32_t us) {
	eglib_DelayUs(eglib_st7789, us);
}

void delay_ms(uint32_t ms) {
	eglib_DelayMs(eglib_st7789, ms);
}

void delay(uint32_t s) {
	eglib_DelayS(eglib_st7789, s);
}