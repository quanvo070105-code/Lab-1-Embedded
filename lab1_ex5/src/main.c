/*
 * Lab 1 - Exercise 5: Traffic Light System Simulation
 * - thread_led_blinky   : LED3 1 s ON / 1 s OFF ("system alive" indicator)
 * - thread_traffic_light: Y0 = RED, Y1 = GREEN, both ON = AMBER
 *       RED (only Y0) 7 s -> GREEN (only Y1) 5 s -> AMBER (Y0 + Y1) 2 s -> RED
 * The three phases are stored in a constant array and the thread iterates
 * over it, so no phase-specific code block is repeated. On every phase change
 * the phase name and duration are printed to the console (USART1, 115200).
 * The two threads are asynchronous: they share no variables.
 */
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <stdio.h>

#define STACK_SIZE  1024
#define PRIORITY    7

static const struct gpio_dt_spec led3 = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios); /* PE4 */
static const struct gpio_dt_spec y0   = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios); /* PE5 - RED   */
static const struct gpio_dt_spec y1   = GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios); /* PE6 - GREEN */

struct phase {
    const char *name;
    int y0;
    int y1;
    int ms;
};

/* Traffic-light phases, in order */
static const struct phase phases[] = {
    { "RED",   1, 0, 7000 },
    { "GREEN", 0, 1, 5000 },
    { "AMBER", 1, 1, 2000 },
};

#define NUM_PHASES  ARRAY_SIZE(phases)

/* Thread 1: LED3 1 s ON / 1 s OFF - "system alive" */
void thread_led_blinky(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    if (!gpio_is_ready_dt(&led3)) {
        return;
    }
    gpio_pin_configure_dt(&led3, GPIO_OUTPUT_INACTIVE);

    while (1) {
        gpio_pin_set_dt(&led3, 1);      /* LED3 ON  */
        k_msleep(1000);

        gpio_pin_set_dt(&led3, 0);      /* LED3 OFF */
        k_msleep(1000);
    }
}

/* Thread 2: traffic light driven by the constant phase table */
void thread_traffic_light(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    if (!gpio_is_ready_dt(&y0) || !gpio_is_ready_dt(&y1)) {
        return;
    }
    gpio_pin_configure_dt(&y0, GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure_dt(&y1, GPIO_OUTPUT_INACTIVE);

    while (1) {
        for (size_t i = 0; i < NUM_PHASES; i++) {
            const struct phase *p = &phases[i];

            gpio_pin_set_dt(&y0, p->y0);
            gpio_pin_set_dt(&y1, p->y1);
            printf("[LIGHT] %s - %d s\n", p->name, p->ms / 1000);

            k_msleep(p->ms);
        }
    }
}

K_THREAD_DEFINE(led_blinky_id, STACK_SIZE, thread_led_blinky,
                NULL, NULL, NULL, PRIORITY, 0, 0);
K_THREAD_DEFINE(traffic_light_id, STACK_SIZE, thread_traffic_light,
                NULL, NULL, NULL, PRIORITY, 0, 0);

int main(void)
{
    /* main() does not control any light - the two threads do the work */
    return 0;
}
