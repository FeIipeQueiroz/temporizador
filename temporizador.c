#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

#define LED_BLUE 11
#define LED_RED 12
#define LED_GREEN 13
#define BUTTON 5

typedef enum {
    ALL_ON,
    TWO_ON,
    ONE_ON,
    ALL_OFF
} LedState;

LedState currentState = ALL_OFF;
bool timerActive = false;

int64_t turn_off_callback(alarm_id_t id, void *user_data) {
    switch (currentState) {
        case ALL_ON:
            gpio_put(LED_BLUE, 0);
            currentState = TWO_ON;
            break;
        case TWO_ON:
            gpio_put(LED_RED, 0);
            currentState = ONE_ON;
            break;
        case ONE_ON:
            gpio_put(LED_GREEN, 0);
            currentState = ALL_OFF;
            timerActive = false;
            break;
        default:
            break;
    }

    if (currentState != ALL_OFF) {
        add_alarm_in_ms(3000, turn_off_callback, NULL, false);
    }

    return 0;
}

void button_callback(uint gpio, uint32_t events) {
    if (!timerActive && gpio == BUTTON) {
        gpio_put(LED_BLUE, 1);
        gpio_put(LED_RED, 1);
        gpio_put(LED_GREEN, 1);
        currentState = ALL_ON;
        timerActive = true;
        add_alarm_in_ms(3000, turn_off_callback, NULL, false);
    }
}

int main() {
    stdio_init_all();

    gpio_init(LED_BLUE);
    gpio_init(LED_RED);
    gpio_init(LED_GREEN);
    gpio_set_dir(LED_BLUE, GPIO_OUT);
    gpio_set_dir(LED_RED, GPIO_OUT);
    gpio_set_dir(LED_GREEN, GPIO_OUT);

    gpio_init(BUTTON);
    gpio_set_dir(BUTTON, GPIO_IN);
    gpio_pull_up(BUTTON);

    gpio_set_irq_enabled_with_callback(BUTTON, GPIO_IRQ_EDGE_FALL, true, &button_callback);

    while (true) {
        tight_loop_contents();
    }

    return 0;
}