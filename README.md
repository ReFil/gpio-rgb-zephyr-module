# LED led-strip Driver

This is a zephyr module that abstracts multiple sets of 3 Zephyr LEDs (GPIO or PWM (although PWM is untested as of yet)) behind the LED-strip interface. 
Please note when using GPIO each channel can only be off or on, so there's a maximum of 8 colours.

Each LED needs to be configured as so

```
led_0: led_0 {
        compatible = "gpio-leds";
        red_led: led_0r {
            gpios = <&gpio0 16 GPIO_ACTIVE_LOW>;
        };
        green_led: led_0g {
            gpios = <&gpio0 17 GPIO_ACTIVE_LOW>;
        };
        blue_led: led_0b {
            gpios = <&gpio0 18 GPIO_ACTIVE_LOW>;
        };
    };
```

Common cathode and common anode LEDS are supported, just change the direction of GPIO_ACTIVE_LOW to GPIO_ACTIVE_HIGH for common cathode.

Then each LED needs to be tied together in the main led-strip node

```
    led_strip_1: gpio_led_strip {
        compatible = "zmk,gpio-led-strip";
        chain-length = <2>;
        led-0 {
            led = <&led_0>;
        };
        led-1 {
            led = <&led_1>;
        };
    };
```

`chain-length` should match the number of child led nodes you have configured