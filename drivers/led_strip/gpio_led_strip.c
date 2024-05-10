/*
 * Copyright (c) 2022 The ZMK Contributors
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_gpio_led_strip

#include <zephyr/drivers/led_strip.h>
#include <zephyr/drivers/led.h>

#define LOG_LEVEL CONFIG_LED_STRIP_LOG_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(gpio_led_strip);

#include <zephyr/kernel.h>
#include <zephyr/device.h>

#include <zephyr/sys/util.h>

struct led_child {
    const struct device *device;
};

#define LED_CHILD(inst)                                                                            \
    { .device = DEVICE_DT_GET(DT_PHANDLE(inst, led)), }

static const struct led_child led_children[] = {DT_INST_FOREACH_CHILD_SEP(0, LED_CHILD, (, ))};

struct gpio_led_strip_config {
    const struct led_child *leds;
    uint32_t pixels_cnt;
};

static int gpio_led_strip_update_rgb(const struct device *dev, struct led_rgb *pixels,
                                     size_t num_pixels) {
    const struct gpio_led_strip_config *config = dev->config;

    int ret = 0;
    for (int i = 0; i < MIN(config->pixels_cnt, num_pixels); i++) {
        // fast convert to 100
        uint16_t r_scaled = pixels[i].r << 8 & 0xFF00;
        r_scaled = (r_scaled >> 2) + (r_scaled >> 3) + (r_scaled >> 6) + (r_scaled >> 8);
        uint16_t g_scaled = pixels[i].g << 8 & 0xFF00;
        g_scaled = (g_scaled >> 2) + (g_scaled >> 3) + (g_scaled >> 6) + (g_scaled >> 8);
        uint16_t b_scaled = pixels[i].b << 8 & 0xFF00;
        b_scaled = (b_scaled >> 2) + (b_scaled >> 3) + (b_scaled >> 6) + (b_scaled >> 8);

        ret = led_set_brightness(config->leds[i].device, 0, (r_scaled >> 8));
        if (ret != 0) {
            LOG_ERR("Failed updating child red led %s", config->leds[i].device->name);
            return ret;
        }
        ret = led_set_brightness(config->leds[i].device, 1, (g_scaled >> 8));
        if (ret != 0) {
            LOG_ERR("Failed updating child grene led %s", config->leds[i].device->name);
            return ret;
        }
        ret = led_set_brightness(config->leds[i].device, 2, (b_scaled >> 8));
        if (ret != 0) {
            LOG_ERR("Failed updating child blue led %s", config->leds[i].device->name);
            return ret;
        }
    }

    return 0;
}

static int gpio_led_strip_update_channels(const struct device *dev, uint8_t *channels,
                                          size_t num_channels) {
    LOG_ERR("update_channels not implemented");
    return -ENOTSUP;
}

static int gpio_led_strip_init(const struct device *dev) {
    const struct gpio_led_strip_config *config = dev->config;

    for (int i = 0; i < config->pixels_cnt; i++) {
        LOG_DBG("Bond led %d: %s", i, config->leds[i].device->name);
    }

    return 0;
}

static const struct led_strip_driver_api gpio_led_strip_api = {
    .update_rgb = gpio_led_strip_update_rgb,
    .update_channels = gpio_led_strip_update_channels,
};

static const struct gpio_led_strip_config gpio_led_strip_config = {
    .leds = led_children,
    .pixels_cnt = DT_INST_PROP(0, chain_length),
};

DEVICE_DT_INST_DEFINE(0, &gpio_led_strip_init, NULL, NULL, &gpio_led_strip_config, POST_KERNEL,
                      CONFIG_GPIO_LED_STRIP_INIT_PRIORITY, &gpio_led_strip_api);