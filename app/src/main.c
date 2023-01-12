/*
 * Copyright 2023 Girish Palya
 * SPDX-License-Identifier: MIT
 */

/*
 * Bindings for seeed are in dts/bindings/gpio/seeed-xiao-header.yaml
 * and boards/arm/xiao_ble/
 */

#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "app_version.h"

LOG_MODULE_REGISTER(main, CONFIG_APP_LOG_LEVEL);

#define GPIO_NODE DT_NODELABEL(xiao_test)

#if !DT_NODE_EXISTS(GPIO_NODE)
#error "Overlay for gpio test is not defined."
#endif

#define XIAO_GPIO_PIN(i, _) \
    const struct gpio_dt_spec D##i = GPIO_DT_SPEC_GET_BY_IDX(GPIO_NODE, gpios, i)
#define IS_GPIO_READY(i, _) !gpio_is_ready_dt(&D##i)
#define GPIO_PIN_CONFIGURE(i, _) \
    (gpio_pin_configure_dt(&D##i, GPIO_OUTPUT_INACTIVE) < 0)

void main(void)
{

    printk("Xiao BLE Test Application %s\n", APP_VERSION_STR);

    LISTIFY(11, XIAO_GPIO_PIN, (;));

    if (LISTIFY(11, IS_GPIO_READY, (||))) {
        LOG_ERR("GPIOs not ready");
        return;
    }

    /* Configure pins. */
    if (LISTIFY(11, GPIO_PIN_CONFIGURE, (||))) {
        LOG_ERR("GPIOs configuration failed");
        return;
    }

    /* Set D0 and D1 pins high */
    if (gpio_pin_set_dt(&D0, 1) < 0) {
        LOG_ERR("GPIO set high failed");
        return;
    }
    if (gpio_pin_set_dt(&D1, 1) < 0) {
        LOG_ERR("GPIO set high failed");
        return;
    }

    while (1) {
        k_sleep(K_MSEC(5000));
    }
}
