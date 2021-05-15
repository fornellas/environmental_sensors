#ifndef DISPLAY_H
#define DISPLAY_H

#include <eglib.h>

extern eglib_t eglib;

void display_setup(void);

void display_message(const char *msg);

void display_error(const char *title, const char *msg);

void draw_measurement(
	coordinate_t x, coordinate_t y,
	coordinate_t width, coordinate_t height,
	color_t *background,
	const char *title, uint16_t value, const char *unit
);

void delay_us(uint32_t us);
void delay_ms(uint32_t ms);
void delay(uint32_t s);

#endif