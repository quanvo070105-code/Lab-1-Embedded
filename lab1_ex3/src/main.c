/*
 * Lab 1 - Exercise 3: Independent Blinking Patterns
 * - thread_led_blinky      : LED3 1.5 s ON / 0.5 s OFF (period 2 s)
 * - thread_output_sequence : Y0/Y1 cycle through 3 states
 *       Only Y0 ON (1 s) -> Only Y1 ON (2 s) -> Y0 + Y1 ON (2 s) -> repeat
 * The two threads are completely independent: no shared variables,
 * each one owns its own pins and keeps its own timing with k_msleep().
 */
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#define STACK_SIZE  1024
#define PRIORITY    7

static const struct gpio_dt_spec led3 = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios); /* PE4 */
static const struct gpio_dt_spec y0   = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios); /* PE5 */
static const struct gpio_dt_spec y1   = GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios); /* PE6 */

/* Thread 1: LED3 1.5 s ON / 0.5 s OFF */
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
        k_msleep(1500);

        gpio_pin_set_dt(&led3, 0);      /* LED3 OFF */
        k_msleep(500);
    }
}

/* Thread 2: Y0/Y1 three-state sequence (period 5 s) */
void thread_output_sequence(void *p1, void *p2, void *p3)
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
        /* State 1: only Y0 ON for 1 s */
        gpio_pin_set_dt(&y0, 1);
        gpio_pin_set_dt(&y1, 0);
        k_msleep(1000);

        /* State 2: only Y1 ON for 2 s */
        gpio_pin_set_dt(&y0, 0);
        gpio_pin_set_dt(&y1, 1);
        k_msleep(2000);

        /* State 3: both Y0 and Y1 ON for 2 s */
        gpio_pin_set_dt(&y0, 1);
        gpio_pin_set_dt(&y1, 1);
        k_msleep(2000);
    }
}

K_THREAD_DEFINE(led_blinky_id, STACK_SIZE, thread_led_blinky,
                NULL, NULL, NULL, PRIORITY, 0, 0);
K_THREAD_DEFINE(output_seq_id, STACK_SIZE, thread_output_sequence,
                NULL, NULL, NULL, PRIORITY, 0, 0);

int main(void)
{
    /* main() does not control any light - the two threads do the work */
    return 0;
}