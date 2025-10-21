#include <stdio.h>
#include "esp_sleep.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/rtc_io.h"
#include "main.h"


void app_main(void) {
    gpio_config(&pir_conf);

    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };

    adc_oneshot_new_unit(&init_config1, &adc1_handle);

    adc_oneshot_chan_cfg_t channel_config = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT
    };

    adc_oneshot_config_channel(adc1_handle, VBAT, &channel_config);

    ledc_timer_config(&high_timer_config_1);
    ledc_timer_config(&high_timer_config_2);
    ledc_timer_config(&high_timer_config_3);
    ledc_timer_config(&high_timer_config_4);
    ledc_timer_config(&low_timer_config_1);
    ledc_timer_config(&low_timer_config_2);
    ledc_timer_config(&low_timer_config_3);

    ledc_channel_config(&led_1_pwm);
    ledc_channel_config(&led_2_pwm);
    ledc_channel_config(&led_3_pwm);
    ledc_channel_config(&led_4_pwm);
    ledc_channel_config(&led_5_pwm);
    ledc_channel_config(&led_6_pwm);
    ledc_channel_config(&led_7_pwm);
    ledc_channel_config(&led_8_pwm);
    ledc_channel_config(&led_9_pwm);
    /*
    ledc_channel_config(&led_10_pwm);
    ledc_channel_config(&led_11_pwm);
    ledc_channel_config(&led_12_pwm);
    ledc_channel_config(&led_13_pwm);
    ledc_channel_config(&led_14_pwm);
    */
    ledc_channel_config_t led_confs[9] = {led_1_pwm, led_2_pwm, led_3_pwm, led_4_pwm, led_5_pwm, led_6_pwm, led_7_pwm, led_8_pwm, led_9_pwm};

    esp_sleep_enable_ext1_wakeup_io((1ULL << PIR_1) | (1ULL << PIR_2), ESP_EXT1_WAKEUP_ANY_HIGH);

    TaskHandle_t xHandle = NULL;

    ESP_LOGI("Main", "Setting up Task");
    ESP_LOGI("Main", "%d", esp_sleep_get_wakeup_cause());

    static int adc_value[2][10];
    adc_oneshot_read(adc1_handle, VBAT, &adc_value[0][0]);

    ESP_LOGI("Main", "Vbat: %f", (adc_value[0][0] * 3300) / 4095 * 1.285);

    uint8_t pir_bottom = gpio_get_level(PIR_1);
    uint8_t pir_top = gpio_get_level(PIR_2);

    if (pir_bottom == 1) {
        ESP_LOGI(TAG, "PIR Bottom");
        for (int i = 0; i < 9; i++) {
            gpio_hold_dis(led_confs[i].gpio_num);
            xTaskCreate(fade_on, "Fade On", 1024, (void*)&led_confs[i], 10, &xHandle);
            vTaskDelay(pdMS_TO_TICKS(100));
        }

        vTaskDelay(pdMS_TO_TICKS(LIGHT_ON * 1000));
    } else if (pir_top == 1) {
        ESP_LOGI(TAG, "PIR Top");
        for (int i = 8; i >= 0; i--) {
            gpio_hold_dis(led_confs[i].gpio_num);
            xTaskCreate(fade_on, "Fade On", 1024, (void*)&led_confs[i], 10, &xHandle);
            vTaskDelay(pdMS_TO_TICKS(100));
        }

        vTaskDelay(pdMS_TO_TICKS(LIGHT_ON * 1000));
    }

    for (int i = 8; i >= 0; i--) {
        xTaskCreate(fade_off, "Fade off", 1024, (void*)&led_confs[i], 10, &xHandle);
    }

    vTaskDelay(pdMS_TO_TICKS(1000));
    esp_deep_sleep_start();
}

void fade_on(void *pvParam) {
    ledc_channel_config_t *pwm_conf = pvParam;
    ledc_mode_t speed_mode = pwm_conf->speed_mode;
    ledc_channel_t channel = pwm_conf->channel;

    for (int i = 0; i < BRIGHTNESS; i++) {
        ledc_set_duty(speed_mode, channel, (uint32_t)(255 * (i / 100.0)));
        ledc_update_duty(speed_mode, channel);
        vTaskDelay(pdMS_TO_TICKS(BRIGHTNESS_DELAY));
    }
    vTaskDelete(NULL);
}

void fade_off(void* pvParam) {
    ledc_channel_config_t *pwm_conf = pvParam;
    ledc_mode_t speed_mode = pwm_conf->speed_mode;
    ledc_channel_t channel = pwm_conf->channel;

    for (int i = BRIGHTNESS; i >= 0; i--) {
        ledc_set_duty(speed_mode, channel, (uint32_t)(255 * (i / 100.0)));
        ledc_update_duty(speed_mode, channel);
        vTaskDelay(pdMS_TO_TICKS(BRIGHTNESS_DELAY));
    }
    gpio_hold_en(pwm_conf->gpio_num);
    vTaskDelete(NULL);
}