#include "dht11.h"

uint8_t data [5];
int64_t last_read_time;
int64_t diff_time = 5000000; // 5 мкс

STATE checksum (uint8_t *d) {
    uint8_t checksum = 0;
    const uint8_t *end = d + 4;
    while (d < end)
        checksum += *d++;
    if (*(++d) == checksum) return SUCCESS;
    else return ERROR_CHECKSUM;
}

STATE wait_for_level(uint32_t level, uint32_t timeout) {
    uint32_t count = 0;

    while (gpio_get_level(DHTPIN) != level) {
        count++;
        if (count > timeout) {
            return ERROR_TIMEOUT;
        }
        ets_delay_us(1);
    }
    return SUCCESS;
}

STATE init_dht(gpio_num_t dht_pin) {
    for(uint8_t i = 0; i < 5; data[i++] = 0);

    gpio_set_direction(dht_pin, GPIO_MODE_OUTPUT);
    gpio_set_level(dht_pin, LOW);
    ets_delay_us(19000); //низкий уровень в течении 18-20 мс
    gpio_set_level(dht_pin, HIGH);
    ets_delay_us(30); // высокий уровень в течении 20-40 мкс
    gpio_set_direction(dht_pin, GPIO_MODE_INPUT);

//      Ожидание от датчика низкий уровень сигнала
    STATE state = wait_for_level(LOW, 80);
    return state;
}

STATE read_dht(gpio_num_t dht_pin) {
    if(esp_timer_get_time() - last_read_time < diff_time) return SUCCESS;
    last_read_time = esp_timer_get_time();

    STATE state;
    state = init_dht(dht_pin);
    if (state != SUCCESS) return state;

    for(uint8_t i = 0; i < 5; i ++){
        for(uint8_t j = 0; j < 8; j++) {
            state = wait_for_level(HIGH, 50);
            if (state != SUCCESS) return state;
            ets_delay_us(30);

            if(gpio_get_level(dht_pin) == 1) data[i] |= (1 << (8-j));
            state = wait_for_level(LOW, 100);
            if (state != SUCCESS) return state;
        }
    }

    // Отпускаем ШД
    gpio_set_direction(dht_pin, GPIO_MODE_OUTPUT);
    gpio_set_level(dht_pin, HIGH);

    return SUCCESS;
}

float get_humidity() {
    return data[0] + data[1] * 0.1f;
}

float get_temperatureC() {
    float f = data[2];
    if (data[3] & 0x80) {
        f = -1 - f;
    }
    return f + (data[3] & 0x0f) * 0.1f;
}

float convertCtoF(float c) { return c * 1.8 + 32; }

float convertFtoC(float f) { return (f - 32) * 0.55555; }