#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "dht11.h"

#define DHTPIN  27

_Noreturn void app_main(void) {
    gpio_num_t dht_pin = DHTPIN;
    gpio_set_level(dht_pin, HIGH);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    while (true) {
        STATE state = read_dht(dht_pin);
        switch (state) {
            case SUCCESS:
                ESP_LOGI(TAG, "temp %f  hum %f %%",get_temperatureC(), get_humidity());
                vTaskDelay(2000 / portTICK_PERIOD_MS);
                break;
            case ERROR_TIMEOUT:
                ESP_LOGE(TAG, "ERROR BY TIMEOUT");
                break;
            case ERROR_CHECKSUM:
                ESP_LOGE(TAG, "ERROR BY CHECKSUM");
        }
    }
}
