#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

#define LED_RCC RCC_GPIOC
#define LED_PORT GPIOC
#define LED_GPIO GPIO13

int main(void) {
	rcc_clock_setup_pll(&rcc_hse_25mhz_3v3[RCC_CLOCK_3V3_84MHZ]);
	rcc_periph_clock_enable(LED_RCC);
	gpio_mode_setup(LED_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_GPIO);
	gpio_clear(LED_PORT, LED_GPIO);
}