/* Test: LED3, Y0 and Y1 all permanently ON */
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

static const struct gpio_dt_spec led3 = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static const struct gpio_dt_spec y0   = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios);
static const struct gpio_dt_spec y1   = GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios);

int main(void)
{
    gpio_pin_configure_dt(&led3, GPIO_OUTPUT_ACTIVE);
    gpio_pin_configure_dt(&y0,   GPIO_OUTPUT_ACTIVE);
    gpio_pin_configure_dt(&y1,   GPIO_OUTPUT_ACTIVE);
    while (1) {
        k_msleep(1000);
    }
    return 0;
}
