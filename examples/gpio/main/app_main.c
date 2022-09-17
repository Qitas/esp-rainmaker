/* GPIO Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <nvs_flash.h>

#include <esp_rmaker_core.h>
#include <esp_rmaker_standard_types.h>

#include <app_wifi.h>
#include <app_insights.h>
#include <ws2812_led.h>
#include "app_priv.h"
#include "esp_wifi.h"

static const char *TAG = "app_main";
static const char *DeviceName = "AE-TEST-2";
/* Callback to handle commands received from the RainMaker cloud */
static esp_err_t write_cb(const esp_rmaker_device_t *device, const esp_rmaker_param_t *param,
            const esp_rmaker_param_val_t val, void *priv_data, esp_rmaker_write_ctx_t *ctx)
{
    // if (ctx) {
    //     ESP_LOGI(TAG, "Received write request via : %s", esp_rmaker_device_cb_src_to_str(ctx->src));
    // }
    // char *device_name = esp_rmaker_device_get_name(device);
    // char *param_name = esp_rmaker_param_get_name(param);
    // if (strcmp(param_name, ESP_RMAKER_DEF_POWER_NAME) == 0) {
    //     ESP_LOGI(TAG, "Received value = %s for %s - %s",
    //             val.val.b? "true" : "false", device_name, param_name);
    //     app_fan_set_power(val.val.b);
    // } else if (strcmp(param_name, ESP_RMAKER_DEF_SPEED_NAME) == 0) {
    //     ESP_LOGI(TAG, "Received value = %d for %s - %s",
    //             val.val.i, device_name, param_name);
    //     app_fan_set_speed(val.val.i);
    // } else {
    //     /* Silently ignoring invalid params */
    //     return ESP_OK;
    // }
    if (ctx) {
        ESP_LOGI(TAG, "Received write request via : %s", esp_rmaker_device_cb_src_to_str(ctx->src));
    }
    if (app_driver_set_gpio(esp_rmaker_param_get_name(param), val.val.b) == ESP_OK) {
        esp_rmaker_param_update_and_report(param, val);
    }
    return ESP_OK;
}

void app_main()
{
    // uint32_t cnt =0;
    uint32_t min_heap = esp_get_free_heap_size();
   
    ESP_LOGI("start free heap"," %luKB",min_heap/1024);   

    /* Initialize Application specific hardware drivers and
     * set initial state.
     */
    app_driver_init();

    /* Initialize NVS. */
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );
    ws2812_led_init();
    ws2812_led_set_rgb(200,0,0);
    /* Initialize Wi-Fi. Note that, this should be called before esp_rmaker_node_init()
     */
    app_wifi_init();
    
    /* Initialize the ESP RainMaker Agent.
     * Note that this should be called after app_wifi_init() but before app_wifi_start()
     * */
    esp_rmaker_config_t rainmaker_cfg = {
        .enable_time_sync = false,
    };
    esp_rmaker_node_t *node = esp_rmaker_node_init(&rainmaker_cfg, "ESP RainMaker Device", DeviceName);
    if (!node) {
        ESP_LOGE(TAG, "Could not initialise node. Aborting!!!");
        vTaskDelay(5000/portTICK_PERIOD_MS);
        abort();
    }

    /* Create a device and add the relevant parameters to it */
    esp_rmaker_device_t *gpio_device = esp_rmaker_device_create(DeviceName, NULL, NULL);
    esp_rmaker_device_add_cb(gpio_device, write_cb, NULL);

    esp_rmaker_param_t *red_param = esp_rmaker_param_create("Red", NULL, esp_rmaker_bool(false), PROP_FLAG_READ | PROP_FLAG_WRITE);
    esp_rmaker_param_add_ui_type(red_param, ESP_RMAKER_UI_TOGGLE);
    esp_rmaker_device_add_param(gpio_device, red_param);

    esp_rmaker_param_t *green_param = esp_rmaker_param_create("Green", NULL, esp_rmaker_bool(false), PROP_FLAG_READ | PROP_FLAG_WRITE);
    esp_rmaker_param_add_ui_type(green_param, ESP_RMAKER_UI_TOGGLE);
    esp_rmaker_device_add_param(gpio_device, green_param);

    esp_rmaker_param_t *blue_param = esp_rmaker_param_create("Blue", NULL, esp_rmaker_bool(false), PROP_FLAG_READ | PROP_FLAG_WRITE);
    esp_rmaker_param_add_ui_type(blue_param, ESP_RMAKER_UI_TOGGLE);
    esp_rmaker_device_add_param(gpio_device, blue_param);

    esp_rmaker_node_add_device(node, gpio_device);

    /* Enable OTA */
    esp_rmaker_ota_enable_default();

    /* Enable Insights. Requires CONFIG_ESP_INSIGHTS_ENABLED=y */
    // app_insights_enable();

    /* Start the ESP RainMaker Agent */
    esp_rmaker_start();

    /* Start the Wi-Fi.
     * If the node is provisioned, it will start connection attempts,
     * else, it will start Wi-Fi provisioning. The function will return
     * after a connection has been successfully established
     */
    err = app_wifi_start(POP_TYPE_NONE);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Could not start Wifi. Aborting!!!");
        vTaskDelay(5000/portTICK_PERIOD_MS);
        abort();
    }
    ws2812_led_set_rgb(0,0,0);
    wifi_ap_record_t ap_info;
    esp_wifi_sta_get_ap_info(&ap_info);
    int8_t rssi = ap_info.rssi;  
    // char *infoBuffer = malloc(256);
    while(1)
    {
        uint32_t heap = esp_get_minimum_free_heap_size();
        if(min_heap > heap){
            min_heap = heap;
            ESP_LOGW("min free heap","%luKB ",min_heap/1024);
        }
        vTaskDelay(pdMS_TO_TICKS(500));
        esp_wifi_sta_get_ap_info(&ap_info);
        if(rssi != ap_info.rssi){
            ESP_LOGW("RSSI","%d -> %d",ap_info.rssi,rssi);
            rssi = ap_info.rssi;
        }
        // cnt++;
        // if(cnt%100==0){
        //     vTaskGetRunTimeStats(infoBuffer);
        //     ESP_LOGI("TimeStats","\n%s",infoBuffer);
        // }
    }
}
