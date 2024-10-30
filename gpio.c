//
// Created by Nick Menshikov on 26.10.2024.
//

#include <stdlib.h>

#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "nrf_log.h"

#define DEVICE_ID 6586
#define LED_PIN_1 NRF_GPIO_PIN_MAP(0, 6)
#define LED_PIN_2_R NRF_GPIO_PIN_MAP(0, 8)
#define LED_PIN_2_G NRF_GPIO_PIN_MAP(1, 9)
#define LED_PIN_2_B NRF_GPIO_PIN_MAP(0, 12)
#define SW_PIN NRF_GPIO_PIN_MAP(1, 6)

int *led_sequence;
int seq_size;
int curr_led = 0;

void turn_off_all_leds();

void gpio_init();

void play_sequence();

void sequence_init();

int main(void)
{
    gpio_init();
    sequence_init();
    turn_off_all_leds();
    while (true) {
        if (nrf_gpio_pin_read(SW_PIN) == 0) {
            play_sequence();
        }
    }
}

void gpio_init()
{
    nrf_gpio_cfg_output(LED_PIN_1);
    nrf_gpio_cfg_output(LED_PIN_2_R);
    nrf_gpio_cfg_output(LED_PIN_2_G);
    nrf_gpio_cfg_output(LED_PIN_2_B);

    nrf_gpio_cfg_input(SW_PIN, NRF_GPIO_PIN_PULLUP);
}

void turn_off_all_leds()
{
    nrf_gpio_pin_write(LED_PIN_1, 1);
    nrf_gpio_pin_write(LED_PIN_2_R, 1);
    nrf_gpio_pin_write(LED_PIN_2_G, 1);
    nrf_gpio_pin_write(LED_PIN_2_B, 1);
}

void play_sequence()
{
    if (curr_led >= seq_size) {
        curr_led = 0;
    }

    nrf_gpio_pin_write(led_sequence[curr_led], 0);

    for (int i = 0; i < 10; ++i) {
        if (nrf_gpio_pin_read(SW_PIN) == 1) {
            return;
        }
        nrf_delay_ms(100);
    }

    nrf_gpio_pin_write(led_sequence[curr_led], 1);

    nrf_delay_ms(1000);

    ++curr_led;
}

void sequence_init()
{
    int digits[4];
    int id = DEVICE_ID;
    for (int i = 3; i >= 0; --i) {
        digits[i] = id % 10;
        id /= 10;
    }

    seq_size = digits[0] + digits[1] + digits[2] + digits[3];
    led_sequence = malloc(seq_size * sizeof(int));
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
