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
#error "Overlay for gpio test is not properly defined."
#endif

void main(void) {

    printk("Xiao BLE Test Application %s\n", APP_VERSION_STR);

    const struct gpio_dt_spec D0 =
        GPIO_DT_SPEC_GET_BY_IDX(GPIO_NODE, gpios, 0);
    const struct gpio_dt_spec D1 =
        GPIO_DT_SPEC_GET_BY_IDX(GPIO_NODE, gpios, 1);

    if (!gpio_is_ready_dt(&D0) || !gpio_is_ready_dt(&D1)) {
        LOG_ERR("GPIOs not ready");
        return;
    }

    /* Configure pins. */
    if ((gpio_pin_configure_dt(&D0, GPIO_OUTPUT_ACTIVE) < 0) ||
        (gpio_pin_configure_dt(&D1, GPIO_OUTPUT_ACTIVE) < 0)) {
        LOG_ERR("GPIOs configuration failed");
        return;
    }

    while (1) {
        /*
        if (gpio_pin_set_dt(&D0, 1) < 0) {
            LOG_ERR("GPIO set high failed");
            return;
        }
        if (gpio_pin_set_dt(&D1, 1) < 0) {
            LOG_ERR("GPIO set high failed");
            return;
        } 
        */
        k_sleep(K_MSEC(5000));
    }
}
