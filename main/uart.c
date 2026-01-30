#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_log.h"

#include "string.h"

#include "uart.h"
#include "uart_manager.h"

#define BUF_SIZE 2048

#define UART_PORT 1

#if UART_PORT == 0
#define UART_NUM    UART_NUM_0
#define UART_RX_PIN GPIO_NUM_44
#define UART_TX_PIN GPIO_NUM_43
#elif UART_PORT == 1
#define UART_NUM    UART_NUM_1
#define UART_RX_PIN GPIO_NUM_19
#define UART_TX_PIN GPIO_NUM_20
#else
#error "Invalid UART_PORT value. Valid options: 0 or 1"
#endif

static const char* TAG = "UART";

int uart_write(void* data, uint32_t bytes, uint32_t timeout)
{
    ESP_LOGD(TAG, "tx %lu bytes:", bytes);
    ESP_LOG_BUFFER_HEXDUMP(TAG, data, bytes, ESP_LOG_DEBUG);
    esp_err_t r = uart_manager_write(UART_NUM, data, bytes);
    return (r == ESP_OK) ? (int)bytes : -1;
}

int uart_read(void* data, uint32_t bytes, uint32_t timeout)
{
    int read = uart_read_bytes(UART_NUM, data, bytes, timeout);
    if (read > 0) {
        ESP_LOGD(TAG, "rx %d bytes:", read);
        ESP_LOG_BUFFER_HEXDUMP(TAG, data, read, ESP_LOG_DEBUG);
    }
    return read;
}

int uart_init()
{
    esp_err_t r = uart_manager_install(UART_NUM, UART_RX_PIN, UART_TX_PIN, 115200, NULL);
    return (r == ESP_OK) ? ESP_OK : ESP_FAIL;
}

int uart_release()
{
    uart_wait_tx_idle_polling(UART_NUM);
    return uart_manager_uninstall(UART_NUM);
}

int uart_flush_buffers() { return uart_flush(UART_NUM); }
