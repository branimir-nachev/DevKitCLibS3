#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_system.h>
#include <string.h>
#include <esp_err.h>
#include <mcp9808.h>
#include <tsl2591.h>

/* float is used in printf(). you need non-default configuration in
 * sdkconfig for ESP8266, which is enabled by default for this
 * example. see sdkconfig.defaults.esp8266
 */

#ifndef APP_CPU_NUM
#define APP_CPU_NUM PRO_CPU_NUM
#endif

void tsl2591_test(void *pvParameters)
{
    tsl2591_t dev = { 0 };

    ESP_ERROR_CHECK(tsl2591_init_desc(&dev, I2C_NUM_0, CONFIG_EXAMPLE_I2C_MASTER_SDA, CONFIG_EXAMPLE_I2C_MASTER_SCL));
    ESP_ERROR_CHECK(tsl2591_init(&dev));

    float lux;
    esp_err_t res;
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
        // Get the lighting in Lux
        if ((res = tsl2591_get_lux(&dev, &lux)) != ESP_OK)
            printf("Could not read lux value: %d\n", res);
        else
            printf("Lux: %f\n", lux);
    }
}

void task(void *pvParameters)
{
    float temperature;
    esp_err_t res;

    i2c_dev_t dev;
    memset(&dev, 0, sizeof(i2c_dev_t));

    ESP_ERROR_CHECK(mcp9808_init_desc(&dev, CONFIG_EXAMPLE_I2C_ADDR, I2C_NUM_0, CONFIG_EXAMPLE_I2C_MASTER_SDA, CONFIG_EXAMPLE_I2C_MASTER_SCL));
    ESP_ERROR_CHECK(mcp9808_init(&dev));

    while (1)
    {
        // Get the values and do something with them.
        if ((res = mcp9808_get_temperature(&dev, &temperature, NULL, NULL, NULL)) == ESP_OK)
            printf("Temperature: %.2f °C\n", temperature);
        else
            printf("Could not get results: %d (%s)", res, esp_err_to_name(res));

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main()
{
    ESP_ERROR_CHECK(i2cdev_init());

    xTaskCreatePinnedToCore(task, "mcp9808_test", configMINIMAL_STACK_SIZE * 8, NULL, 5, NULL, APP_CPU_NUM);
    xTaskCreatePinnedToCore(tsl2591_test, "tsl2591_test", configMINIMAL_STACK_SIZE * 8, NULL, 5, NULL, APP_CPU_NUM);
}
