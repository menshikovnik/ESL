//
// Created by Nick Menshikov on 26.10.2024.
//

#include <stdlib.h>

#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "nrf_log.h"
#include "nrfx_systick.h"
#include "nrfx_gpiote.h"

#define DEVICE_ID 6586

#define PWM_FREQ 1000
#define PERIOD_US (1000000 / PWM_FREQ)
#define MAX_DUTY_CYCLE 100      
#define DELAY_AFTER_BLINK_MS 1000

#define DELAY_AFTER_LED_BLINK_MS 1000
#define DELAY_AFTER_LED_OFF_MS 1000

#define LED_PIN_1 NRF_GPIO_PIN_MAP(0, 6)
#define LED_PIN_2_R NRF_GPIO_PIN_MAP(0, 8)
#define LED_PIN_2_G NRF_GPIO_PIN_MAP(1, 9)
#define LED_PIN_2_B NRF_GPIO_PIN_MAP(0, 12)
#define SW_PIN NRF_GPIO_PIN_MAP(1, 6)

volatile bool button_pressed = false;
int digits[4];
int seq_size;
int curr_led = 0;

void turn_off_all_leds(void);

void gpio_init(void);

void play_sequence(int pin);

void sequence_init(int* led_sequence);

void turn_off_led(int pin);

void turn_on_led(int pin);

void calculate_size_of_sequence(void);

void BUTTON_IRQHandler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action);

void pwm_led_fade(int pin);

void pwm_cycle(int pin, int duty_cycle);


int main(void)
{
    gpio_init();
    nrfx_systick_init();
    calculate_size_of_sequence();
    int led_sequence[seq_size];
    sequence_init(led_sequence);
    turn_off_all_leds();
    while (true) {
        while (button_pressed)
        {
            play_sequence(led_sequence[curr_led]);
        }
    }
}

void gpio_init(void)
{
    nrf_gpio_cfg_output(LED_PIN_1);
    nrf_gpio_cfg_output(LED_PIN_2_R);
    nrf_gpio_cfg_output(LED_PIN_2_G);
    nrf_gpio_cfg_output(LED_PIN_2_B);

    nrfx_gpiote_init();
    nrfx_gpiote_in_config_t button_cfg = NRFX_GPIOTE_CONFIG_IN_SENSE_TOGGLE(false);
    button_cfg.pull = NRF_GPIO_PIN_PULLUP;
    nrfx_gpiote_in_init(SW_PIN, &button_cfg, BUTTON_IRQHandler);
    nrfx_gpiote_in_event_enable(SW_PIN, true);
}

void pwm_cycle(int pin, int duty_cycle)
{
    nrfx_systick_state_t init_time;
    uint32_t t_on_us = (duty_cycle * PERIOD_US) / 100;
    uint32_t t_off_us = PERIOD_US - t_on_us;

    nrf_gpio_pin_write(pin, 0);
    nrfx_systick_get(&init_time);
    while (!nrfx_systick_test(&init_time, t_on_us))
    {
    }

    nrf_gpio_pin_write(pin, 1);
    nrfx_systick_get(&init_time);
    while (!nrfx_systick_test(&init_time, t_off_us))
    {
    }
}

void fade_led(int pin)
{
    for (int duty_cycle = 0; duty_cycle <= MAX_DUTY_CYCLE; ++duty_cycle)
    {
        pwm_cycle(pin, duty_cycle);
    }

    for (int duty_cycle = MAX_DUTY_CYCLE; duty_cycle >= 0; --duty_cycle)
    {
        pwm_cycle(pin, duty_cycle);
    }
}


void BUTTON_IRQHandler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    button_pressed = !button_pressed;
}

void turn_off_all_leds(void)
{
    nrf_gpio_pin_write(LED_PIN_1, 1);
    nrf_gpio_pin_write(LED_PIN_2_R, 1);
    nrf_gpio_pin_write(LED_PIN_2_G, 1);
    nrf_gpio_pin_write(LED_PIN_2_B, 1);
}

void play_sequence(const int pin)
{
    if (curr_led >= seq_size) {
        curr_led = 0;
    }

    fade_led(pin);
    nrfx_systick_delay_ms(DELAY_AFTER_BLINK_MS);

    ++curr_led;
}

void sequence_init(int *led_sequence)
{
    int idx = 0;

    for (int i = 0; i < digits[0]; ++i) {
        led_sequence[idx++] = LED_PIN_1;
    }
    for (int i = 0; i < digits[1]; ++i) {
        led_sequence[idx++] = LED_PIN_2_R;
    }
    for (int i = 0; i < digits[2]; ++i) {
        led_sequence[idx++] = LED_PIN_2_G;
    }
    for (int i = 0; i < digits[3]; ++i) {
        led_sequence[idx++] = LED_PIN_2_B;
    }
}

void turn_off_led(const int pin)
{
    nrf_gpio_pin_write(pin, 1);
}

void turn_on_led(const int pin)
{
    nrf_gpio_pin_write(pin, 0);
}

void calculate_size_of_sequence(void)
{
    int id = DEVICE_ID;
    for (int i = 3; i >= 0; --i) {
        digits[i] = id % 10;
        id /= 10;
    }
    seq_size = digits[0] + digits[1] + digits[2] + digits[3];
}
