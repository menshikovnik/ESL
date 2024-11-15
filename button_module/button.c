#include "button.h"
#include "nrf_gpio.h"
#include "nrfx_gpiote.h"
#include "app_timer.h"
#include "boards.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#define BUTTON_PIN NRF_GPIO_PIN_MAP(1, 6)
#define DEBOUNCE_INTERVAL APP_TIMER_TICKS(50)
#define DOUBLE_CLICK_INTERVAL APP_TIMER_TICKS(250)

APP_TIMER_DEF(debounce_timer);
APP_TIMER_DEF(double_click_timer);

static button_event_handler_t  m_event_handler = NULL;

/**@brief Function to check if the button is pressed. */
static bool is_button_pressed(void)
{
    return nrf_gpio_pin_read(BUTTON_PIN) == 0;
}

/**@brief GPIOTE event handler for the button. */
static void button_gpiote_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    if (is_button_pressed()) {
        NRF_LOG_INFO("Button press detected");
        app_timer_start(debounce_timer, DEBOUNCE_INTERVAL, NULL);
    }
}

/**@brief Debounce timer timeout handler. */
static void button_timer_handler(void *p_context)
{
    static bool first_click_detected = false;

    if (is_button_pressed()) {
        if (first_click_detected) { //second click
            app_timer_stop(double_click_timer);
            first_click_detected = false;

            if (m_event_handler) {
                m_event_handler(BUTTON_EVENT_DOUBLE_CLICK);
            }
            NRF_LOG_INFO("Double click detected");
        }
        else //first click
        {
            first_click_detected = true;
            app_timer_start(double_click_timer, DOUBLE_CLICK_INTERVAL, NULL);
            NRF_LOG_INFO("single click detected");
        }
    }
    else {
        first_click_detected = false;
    }
}

void button_init(button_event_handler_t event_handler)
{
    ret_code_t err_code;

    m_event_handler = event_handler;

    app_timer_create(&debounce_timer, APP_TIMER_MODE_SINGLE_SHOT, button_timer_handler);
    app_timer_create(&double_click_timer, APP_TIMER_MODE_SINGLE_SHOT, button_timer_handler);

    nrfx_gpiote_init();

    nrfx_gpiote_in_config_t button_config = NRFX_GPIOTE_CONFIG_IN_SENSE_TOGGLE(false);
    button_config.pull = NRF_GPIO_PIN_PULLUP;
    err_code = nrfx_gpiote_in_init(BUTTON_PIN, &button_config, button_gpiote_handler);
    APP_ERROR_CHECK(err_code);

    nrfx_gpiote_in_event_enable(BUTTON_PIN, true);

    NRF_LOG_INFO("button module initialized");
}
