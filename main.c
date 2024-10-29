#include "boards.h"
#include "nrf_delay.h"
#include <stdbool.h>

#define DEVICE_ID 6586
#define LED_ON_TIME_MS 400
#define LED_OFF_TIME_MS 400
#define PAUSE_BETWEEN_LEDS 2000
#define PAUSE_AFTER_ID_MS 3000

void led_blink(int i);

int main(void) 
{
  bsp_board_init(BSP_INIT_LEDS);

  int digits[4];
  int id = DEVICE_ID;

  for (int i = 3; i >= 0; i--) {
    digits[i] = id % 10;
    id /= 10;
  }

  while (true) {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < digits[i]; j++) {
        led_blink(i);
      }
      nrf_delay_ms(PAUSE_BETWEEN_LEDS);
    }

    bsp_board_leds_on();
    nrf_delay_ms(PAUSE_BETWEEN_LEDS);
    bsp_board_leds_off();
    nrf_delay_ms(PAUSE_AFTER_ID_MS);
  }
}

void led_blink(const int i)
{
  bsp_board_led_invert(i);
  nrf_delay_ms(LED_ON_TIME_MS);
  bsp_board_led_invert(i);
  nrf_delay_ms(LED_OFF_TIME_MS);
}
