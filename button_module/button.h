#ifndef BUTTON_H
#define BUTTON_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    BUTTON_EVENT_DOUBLE_CLICK,
} button_event_t;

typedef void (*button_event_handler_t)(button_event_t event);

/**@brief Function to initialize the button module.
 *
 * @param[in] event_handler  Function to be called when a button event occurs.
 */
void button_init(button_event_handler_t event_handler);

#endif // BUTTON_H
