/*
 * Copyright 2023 Girish Palya
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>

#include "app_version.h"

LOG_MODULE_REGISTER(main, CONFIG_APP_LOG_LEVEL);

#define GPIO_NODE DT_NODELABEL(xiao_test)

#if !DT_NODE_EXISTS(GPIO_NODE)
#error "Overlay for gpio test is not defined."
#endif

#if !DT_NODE_EXISTS(DT_PATH(zephyr_user)) || !DT_NODE_HAS_PROP(DT_PATH(zephyr_user), io_channels)
#error "No suitable devicetree overlay specified"
#endif

#define DT_SPEC_AND_COMMA(node_id, prop, idx) \
    ADC_DT_SPEC_GET_BY_IDX(node_id, idx),

/* Data of ADC io-channels specified in devicetree. */
static const struct adc_dt_spec adc_channels[] = {
    DT_FOREACH_PROP_ELEM(DT_PATH(zephyr_user), io_channels,
        DT_SPEC_AND_COMMA)
};

#define XIAO_GPIO_PIN(i, _) \
    static const struct gpio_dt_spec D##i = GPIO_DT_SPEC_GET_BY_IDX(GPIO_NODE, gpios, i)
LISTIFY(11, XIAO_GPIO_PIN, (;));

#define IS_GPIO_READY(i, _) !gpio_is_ready_dt(&D##i)

/* ------------------------------------------------------------ */

void adc_read_channels(void)
{
    int16_t buf;
    int err;
    struct adc_sequence sequence = {
        .buffer = &buf,
        /* buffer size in bytes, not number of samples */
        .buffer_size = sizeof(buf),
    };

    printk("ADC reading:\n");
    for (size_t i = 0U; i < ARRAY_SIZE(adc_channels); i++) {
        int32_t val_mv;

        printk("- %s, channel %d: ",
            adc_channels[i].dev->name,
            adc_channels[i].channel_id);

        (void)adc_sequence_init_dt(&adc_channels[i], &sequence);

        err = adc_read(adc_channels[i].dev, &sequence);
        if (err < 0) {
            printk("Could not read (%d)\n", err);
            continue;
        } else {
            printk("%" PRId16, buf);
        }

        /* conversion to mV may not be supported, skip if not */
        val_mv = buf;
        err = adc_raw_to_millivolts_dt(&adc_channels[i],
            &val_mv);
        if (err < 0) {
            printk(" (value in mV not available)\n");
        } else {
            printk(" = %" PRId32 " mV\n", val_mv);
        }
    }
}

/* ------------------------------------------------------------ */

void adc_configure(void)
{
    int err;
    /* Configure channels individually prior to sampling. */
    for (size_t i = 0U; i < ARRAY_SIZE(adc_channels); i++) {
        if (!device_is_ready(adc_channels[i].dev)) {
            printk("ADC controller device not ready\n");
            return;
        }

        err = adc_channel_setup_dt(&adc_channels[i]);
        if (err < 0) {
            printk("Could not setup channel #%d (%d)\n", i, err);
            return;
        }
    }
}

/* ------------------------------------------------------------ */

void gpio_configure(void)
{

    if (LISTIFY(11, IS_GPIO_READY, (||))) {
        LOG_ERR("GPIOs not ready");
        return;
    }
    /* Configure pins. D0, D2 as output and D10 as input. */
    if ((gpio_pin_configure_dt(&D0, GPIO_OUTPUT_INACTIVE) < 0) || (gpio_pin_configure_dt(&D2, GPIO_OUTPUT_INACTIVE) < 0)) {
        LOG_ERR("GPIOs configuration failed");
        return;
    }
    if (gpio_pin_configure_dt(&D10, GPIO_INPUT) < 0) {
        LOG_ERR("GPIOs configuration failed");
        return;
    }
    /* Set D0 pin high */
    if (gpio_pin_set_dt(&D0, 1) < 0) {
        LOG_ERR("GPIO set high failed");
        return;
    }
}

/* ------------------------------------------------------------ */

int gpio_pulse(void)
{
    // uint32_t period_us = .1; // .1 gives 1us delay, 1us gives 2us delay, 3 us
    // gives 4us delay, implement your own
    static const int32_t period_us_a = .1;
    static const int32_t period_us_b = .1;
    // static const uint32_t period_ms = 2;
    static int pin_status = 0, new_status;
    static uint32_t counter = 0;

    /* Create square pulse on D2 */
    if (gpio_pin_set_dt(&D2, 1) < 0) {
        LOG_ERR("GPIO set high failed");
        return -1;
    }
    k_busy_wait(period_us_a);
    // k_sleep(K_MSEC(period_ms));

    new_status = gpio_pin_get_dt(&D10);
    if (new_status != pin_status) {
        printk("%d: pin level is %d\n", counter++, new_status);
        pin_status = new_status;
    }

    if (gpio_pin_set_dt(&D2, 0) < 0) {
        LOG_ERR("GPIO set low failed");
        return -1;
    }
    k_busy_wait(period_us_b);
    // k_sleep(K_MSEC(period_ms));
    return 0;
}

/* ------------------------------------------------------------ */

void main(void)
{
    printk("Xiao BLE Test Application %s\n", APP_VERSION_STR);
    adc_configure();
    gpio_configure();
    while (1) {
        // if (gpio_pulse() < 0) {
        //     return;
        // }
        adc_read_channels();
        k_sleep(K_MSEC(500));
    }
}
