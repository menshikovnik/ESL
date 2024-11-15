#include <stdbool.h>
#include <stdint.h>
#include "boards.h"
#include "nrf_gpio.h"
#include "nrfx_systick.h"
#include "app_timer.h"
#include "nrf_drv_clock.h"

#include "nordic_common.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "button.h"

#define DEVICE_ID 6586
#define PWM_FREQUENCY_HZ 1000
#define PWM_PERIOD_US (1000000 / PWM_FREQUENCY_HZ)
#define DUTY_CYCLE_PERCENT_MAX 100
#define DUTY_CYCLE_STEP_PERCENT 0.1

#define LED_PIN_1 NRF_GPIO_PIN_MAP(0, 6)
#define LED_PIN_2_R NRF_GPIO_PIN_MAP(0, 8)
#define LED_PIN_2_G NRF_GPIO_PIN_MAP(1, 9)
#define LED_PIN_2_B NRF_GPIO_PIN_MAP(0, 12)
#define SW_PIN NRF_GPIO_PIN_MAP(1, 6)

int curr_led;
int digits[4];
int seq_size;
volatile bool double_click = false;

void leds_init(void);
void init_logs(void);
void lfclk_request(void);
void fade_led(int pin);
void pwm_cycle(int pin, double duty_cycle);
void turn_on_led(int pin);
void turn_off_led(int pin);
void turn_off_all_leds(void);
void init_led_arr(int *led_arr);
void button_event_handler(button_event_t event);
void calculate_size_of_sequence(void);
void sequence_init(int* led_sequence);

int main(void)
{
    init_logs();
    NRF_LOG_INFO("start");
    lfclk_request();

    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    leds_init();
    button_init(button_event_handler);

    nrfx_systick_init();

    calculate_size_of_sequence();
    int led_sequence[seq_size];
    sequence_init(led_sequence);

    turn_off_all_leds();

    while (true)
    {
        if (curr_led == seq_size) {
            curr_led = 0;
        }
        if (double_click)
        {
            fade_led(led_sequence[curr_led]);
            ++curr_led;
        }
    }
}

void button_event_handler(button_event_t event)
{
    if (event == BUTTON_EVENT_DOUBLE_CLICK) {
        double_click = !double_click;
        NRF_LOG_INFO("double click event received, toggling states");
    }
}

void fade_led(int pin)
{
    for (double duty_cycle = 0; duty_cycle <= DUTY_CYCLE_PERCENT_MAX; duty_cycle += DUTY_CYCLE_STEP_PERCENT)
    {
        while (true)
        {
            pwm_cycle(pin, duty_cycle);
            if (double_click) {
                break;
            }
        }
    }

    for (double duty_cycle = DUTY_CYCLE_PERCENT_MAX; duty_cycle >= 0; duty_cycle -= DUTY_CYCLE_STEP_PERCENT)
    {
        while (true)
        {
            pwm_cycle(pin, duty_cycle);
            if (double_click) {
               break; 
            }
        }
    }
}

void pwm_cycle(int pin, double duty_cycle)
{
    nrfx_systick_state_t init_time;
    uint32_t t_on_us  = (duty_cycle * PWM_PERIOD_US) / DUTY_CYCLE_PERCENT_MAX;
    uint32_t t_off_us = PWM_PERIOD_US - t_on_us;

    turn_on_led(pin);
    nrfx_systick_get(&init_time);
    while (!nrfx_systick_test(&init_time, t_on_us))
    {
    }

    turn_off_led(pin);
    nrfx_systick_get(&init_time);
    while (!nrfx_systick_test(&init_time, t_off_us))
    {
    }
}

void turn_on_led(int pin)
{
    nrf_gpio_pin_write(pin, 0);
    NRF_LOG_DEBUG("turning on LED at pin %d", pin);
}

void turn_off_led(int pin)
{
    nrf_gpio_pin_write(pin, 1);
    NRF_LOG_DEBUG("turning off LED at pin %d", pin);
}

void turn_off_all_leds(void)
{
    nrf_gpio_pin_write(LED_PIN_1, 1);
    nrf_gpio_pin_write(LED_PIN_2_R, 1);
    nrf_gpio_pin_write(LED_PIN_2_G, 1);
    nrf_gpio_pin_write(LED_PIN_2_B, 1);
}

void lfclk_request(void)
{
    ret_code_t err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);
    nrf_drv_clock_lfclk_request(NULL);
}

void init_logs(void)
{
    ret_code_t ret = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(ret);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
    NRF_LOG_INFO("log system initialized.");
}

void leds_init(void)
{
    nrf_gpio_cfg_output(LED_PIN_1);
    nrf_gpio_cfg_output(LED_PIN_2_R);
    nrf_gpio_cfg_output(LED_PIN_2_G);
    nrf_gpio_cfg_output(LED_PIN_2_B);
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

void calculate_size_of_sequence(void)
{
    int id = DEVICE_ID;
    for (int i = 3; i >= 0; --i) {
        digits[i] = id % 10;
        id /= 10;
    }
    seq_size = digits[0] + digits[1] + digits[2] + digits[3];
}
