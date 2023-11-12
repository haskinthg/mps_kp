#ifndef DHT11_INC

#include "driver/gpio.h"
#include "rom/ets_sys.h"
#include "esp_timer.h"
#define DHTPIN  27
#define LOW 0
#define HIGH 1

static const char *TAG = "DHT11_MODULE";

enum STATE {
    SUCCESS = 0,
    ERROR_TIMEOUT = 1,
    ERROR_CHECKSUM = 2
};

typedef enum STATE STATE;
STATE read_dht(gpio_num_t dht_pin);
float get_humidity();
float get_temperatureC();
float convertCtoF(float c);
float convertFtoC(float f);

#define DHT11_INC
#endif
