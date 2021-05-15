#define _GNU_SOURCE
#include "display.h"
#include "mh-z19c.h"
#include "pms5003.h"
#include "usart.h"
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/usart.h>
#include <stdio.h>
#include <stdlib.h>
#include <eglib/display.h>
#include <eglib/display/frame_buffer.h>

//
// PMS5003
//

#define PMS5003_RCC_RESET RCC_GPIOA
#define PMS5003_PORT_RESET GPIOA
#define PMS5003_GPIO_RESET GPIO8

#define PMS5003_RCC_SET RCC_GPIOB
#define PMS5003_PORT_SET GPIOB
#define PMS5003_GPIO_SET GPIO15

#define PMS5003_USART_RCC_USART_PORT RCC_GPIOA
#define PMS5003_USART_RCC_USART RCC_USART1
#define PMS5003_USART_RCC_USART_TX RCC_GPIOA
#define PMS5003_USART_PORT_USART_TX GPIOA
#define PMS5003_USART_GPIO_USART_TX GPIO9
#define PMS5003_USART_RCC_USART_RX RCC_GPIOA
#define PMS5003_USART_PORT_USART_RX GPIOA
#define PMS5003_USART_GPIO_USART_RX GPIO10
#define PMS5003_USART USART1
#define PMS5003_USART_BAUDRATE 9600
#define PMS5003_USART_DATABITS 8
#define PMS5003_USART_STOPBITS USART_STOPBITS_1
#define PMS5003_USART_MODE USART_MODE_TX_RX
#define PMS5003_USART_PARITY USART_PARITY_NONE
#define PMS5003_USART_FLOW_CONTROL USART_FLOWCONTROL_NONE

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

void setup_pms5003(void);

void setup_pms5003(void) {
	enum pms5003_error pm5003_ret;

	display_message("PMS5003");

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
	delay_us(150);

	// RESET
	gpio_set(PMS5003_PORT_RESET, PMS5003_GPIO_RESET);
	// Required as PMS5003 takes a while to start accepting requests after reset
	delay_ms(1500);

	// Passive mode
	if((pm5003_ret = pms5003_set_data_mode(PMS5003_DATA_MODE_PASSIVE, write_serial_pms5003, read_serial_pms5003))) {
		display_error("PMS5003", pms5003_strerror(pm5003_ret));
		while(true);
	}
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
#define MH_Z19C_GPIO_HD GPIO10

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

void setup_mh_z19c(void);

void setup_mh_z19c(void) {
	enum mh_z19c_error mh_z19c_ret;

	display_message("MH-Z19C");

	// Required as MH-Z19C takes a while to boot
	// FIXME
	// delay(180);

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
		display_error("MH-Z19C", mh_z19c_strerror(mh_z19c_ret));
}

//
// Main
//

int main(void) {
	rcc_clock_setup_pll(&rcc_hse_25mhz_3v3[RCC_CLOCK_3V3_84MHZ]);

	display_setup();

	setup_pms5003();

	setup_mh_z19c();

	display_message("Ready!");

	while(true) {
		struct pms5003_measurement measurement;
		enum pms5003_error pm5003_ret;
		uint16_t co2_concentration;
		enum mh_z19c_error mh_z19c_ret;

		if((pm5003_ret = pms5003_get_passive_measurement(&measurement, write_serial_pms5003, read_serial_pms5003))) {
			display_error("PMS5003", pms5003_strerror(pm5003_ret));
			continue;
		}

		mh_z19c_ret = mh_z19c_read(
			&co2_concentration,
			write_serial_mh_z19c,
			read_serial_mh_z19c
		);
		if(mh_z19c_ret) {
			display_error("MH-Z19C", mh_z19c_strerror(mh_z19c_ret));
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
			0, eglib_GetHeight(&eglib) / 3 * 2,
			eglib_GetWidth(&eglib), eglib_GetHeight(&eglib) / 3,
			&background,
			"CO₂", co2_concentration, "ppm"
		);

		eglib_FrameBuffer_Send(&eglib);
	}
}