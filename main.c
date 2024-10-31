//
// Created by Nick Menshikov on 26.10.2024.
//

#include <stdlib.h>

#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "nrf_log.h"

#define DEVICE_ID 6586
#define DELAY_AFTER_LED_BLINK_MS 1000
#define DELAY_AFTER_LED_OFF_MS 1000
#define LED_PIN_1 NRF_GPIO_PIN_MAP(0, 6)
#define LED_PIN_2_R NRF_GPIO_PIN_MAP(0, 8)
#define LED_PIN_2_G NRF_GPIO_PIN_MAP(1, 9)
#define LED_PIN_2_B NRF_GPIO_PIN_MAP(0, 12)
#define SW_PIN NRF_GPIO_PIN_MAP(1, 6)

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

bool is_button_pressed(void);


int main(void)
{
    gpio_init();
    calculate_size_of_sequence();
    int led_sequence[seq_size];
    sequence_init(led_sequence);
    turn_off_all_leds();
    while (true) {
        if (is_button_pressed()) {
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

    nrf_gpio_cfg_input(SW_PIN, NRF_GPIO_PIN_PULLUP);
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

    turn_on_led(pin);

    for (int i = 0; i < DELAY_AFTER_LED_BLINK_MS / 100; ++i) {
        if (!is_button_pressed()) {
            return;
        }
        nrf_delay_ms(DELAY_AFTER_LED_BLINK_MS / 10);
    }

    turn_off_led(pin);

    nrf_delay_ms(DELAY_AFTER_LED_BLINK_MS);

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

bool is_button_pressed()
{
    return nrf_gpio_pin_read(SW_PIN) == 0;
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
