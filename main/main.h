#include <stdio.h>
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

// PIR Top: IO34
// PIR Bottom: IO2
// VBAT: IO35
// SCL: IO22
// SDA: IO21
// LED_1: IO32
// LED_2: IO33
// LED_3: IO25
// LED_4: IO14
// LED_5: IO27
// LED_6: IO26
// LED_7: IO13
// LED_8: IO23
// LED_9: IO19
// LED_10: IO18
// LED_11: IO5
// LED_12: IO17
// LED_13: IO16
// LED_14: IO4

#define TAG "Main"

#define BRIGHTNESS 10
#define BRIGHTNESS_DELAY 25 
#define PIR_1 GPIO_NUM_2
#define PIR_2 GPIO_NUM_34
#define VBAT ADC_CHANNEL_7

#define LIGHT_ON 60

const int LEDS[9] = {GPIO_NUM_32, GPIO_NUM_33, GPIO_NUM_25, GPIO_NUM_18, GPIO_NUM_27, GPIO_NUM_26, GPIO_NUM_13, GPIO_NUM_23, GPIO_NUM_19};

gpio_config_t pir_conf = {
    .pin_bit_mask = (1ULL << PIR_1) | (1ULL << PIR_2),
    .mode = GPIO_MODE_INPUT,
    .pull_up_en = GPIO_PULLUP_DISABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .intr_type = GPIO_INTR_DISABLE
};
ledc_timer_config_t high_timer_config_1 = {
    .speed_mode = LEDC_HIGH_SPEED_MODE,
    .duty_resolution = LEDC_TIMER_8_BIT,
    .timer_num = LEDC_TIMER_0,
    .freq_hz = 1000
};

ledc_timer_config_t high_timer_config_2 = {
    .speed_mode = LEDC_HIGH_SPEED_MODE,
    .duty_resolution = LEDC_TIMER_8_BIT,
    .timer_num = LEDC_TIMER_1,
    .freq_hz = 1000
};

ledc_timer_config_t high_timer_config_3 = {
    .speed_mode = LEDC_HIGH_SPEED_MODE,
    .duty_resolution = LEDC_TIMER_8_BIT,
    .timer_num = LEDC_TIMER_2,
    .freq_hz = 1000
};

ledc_timer_config_t high_timer_config_4 = {
    .speed_mode = LEDC_HIGH_SPEED_MODE,
    .duty_resolution = LEDC_TIMER_8_BIT,
    .timer_num = LEDC_TIMER_3,
    .freq_hz = 1000
};

ledc_timer_config_t low_timer_config_1 = {
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .duty_resolution = LEDC_TIMER_8_BIT,
    .timer_num = LEDC_TIMER_0,
    .freq_hz = 1000
};

ledc_timer_config_t low_timer_config_2 = {
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .duty_resolution = LEDC_TIMER_8_BIT,
    .timer_num = LEDC_TIMER_1,
    .freq_hz = 1000
};

ledc_timer_config_t low_timer_config_3 = {
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .duty_resolution = LEDC_TIMER_8_BIT,
    .timer_num = LEDC_TIMER_2,
    .freq_hz = 1000
};


ledc_channel_config_t led_1_pwm = {
    .gpio_num = LEDS[0],
    .speed_mode = LEDC_HIGH_SPEED_MODE,
    .channel = LEDC_CHANNEL_0,
    .duty = 0,
};

ledc_channel_config_t led_2_pwm = {
    .gpio_num = LEDS[1],
    .speed_mode = LEDC_HIGH_SPEED_MODE,
    .channel = LEDC_CHANNEL_1,
    .duty = 0,
};

ledc_channel_config_t led_3_pwm = {
    .gpio_num = LEDS[2],
    .speed_mode = LEDC_HIGH_SPEED_MODE,
    .channel = LEDC_CHANNEL_2,
    .duty = 0,
};

ledc_channel_config_t led_4_pwm = {
    .gpio_num = LEDS[3],
    .speed_mode = LEDC_HIGH_SPEED_MODE,
    .channel = LEDC_CHANNEL_3,
    .duty = 0,
};

ledc_channel_config_t led_5_pwm = {
    .gpio_num = LEDS[4],
    .speed_mode = LEDC_HIGH_SPEED_MODE,
    .channel = LEDC_CHANNEL_4,
    .duty = 0,
};

ledc_channel_config_t led_6_pwm = {
    .gpio_num = LEDS[5],
    .speed_mode = LEDC_HIGH_SPEED_MODE,
    .channel = LEDC_CHANNEL_5,
    .duty = 0,
};

ledc_channel_config_t led_7_pwm = {
    .gpio_num = LEDS[6],
    .speed_mode = LEDC_HIGH_SPEED_MODE,
    .channel = LEDC_CHANNEL_6,
    .duty = 0,
};

ledc_channel_config_t led_8_pwm = {
    .gpio_num = LEDS[7],
    .speed_mode = LEDC_HIGH_SPEED_MODE,
    .channel = LEDC_CHANNEL_7,
    .duty = 0,
};

ledc_channel_config_t led_9_pwm = {
    .gpio_num = LEDS[8],
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .channel = LEDC_CHANNEL_0,
    .duty = 0,
};

/*
ledc_channel_config_t led_10_pwm = {
    .gpio_num = LEDS[9],
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .channel = LEDC_CHANNEL_1,
    .duty = 0,
};

ledc_channel_config_t led_11_pwm = {
    .gpio_num = LEDS[10],
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .channel = LEDC_CHANNEL_2,
    .duty = 0,
};

ledc_channel_config_t led_12_pwm = {
    .gpio_num = LEDS[11],
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .channel = LEDC_CHANNEL_3,
    .duty = 0,
};

ledc_channel_config_t led_13_pwm = {
    .gpio_num = LEDS[12],
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .channel = LEDC_CHANNEL_4,
    .duty = 0,
};

ledc_channel_config_t led_14_pwm = {
    .gpio_num = LEDS[13],
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .channel = LEDC_CHANNEL_5,
    .duty = 0,
};
*/

void fade_on(void *pvParam);
void fade_off(void* pvParam);