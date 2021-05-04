#include <eglib.h>
#include <eglib/hal/four_wire_spi/libopencm3_stm32f4.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/spi.h>
#include <eglib/display/st7789.h>

#define LED_RCC RCC_GPIOC
#define LED_PORT GPIOC
#define LED_GPIO GPIO13

int main(void) {
	eglib_t eglib;
	four_wire_spi_libopencm3_stm32f4_config_t  four_wire_spi_libopencm3_stm32f4_config = {
		// rst
		.rcc_rst = RCC_GPIOA,
		.port_rst = GPIOA,
		.gpio_rst = GPIO4,
		// cd
		.rcc_dc = RCC_GPIOA,
		.port_dc = GPIOA,
		.gpio_dc = GPIO3,
		// cs
		.rcc_cs = RCC_GPIOA,
		.port_cs = GPIOA,
		.gpio_cs = GPIO2,
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

	rcc_clock_setup_pll(&rcc_hse_25mhz_3v3[RCC_CLOCK_3V3_84MHZ]);

	rcc_periph_clock_enable(LED_RCC);
	gpio_mode_setup(LED_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_GPIO);
	gpio_clear(LED_PORT, LED_GPIO);

	eglib_Init(
		&eglib,
		&four_wire_spi_libopencm3_stm32f4, &four_wire_spi_libopencm3_stm32f4_config,
		&st7789, &st7789_config
	);
}