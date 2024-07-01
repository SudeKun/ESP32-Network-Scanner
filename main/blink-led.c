#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_netif_net_stack.h"
#include "lwip/ip4_addr.h"
#include "lwip/etharp.h"
#include "lwip/ip_addr.h"


#include "wifi.c"


#define TAG "MAIN"
#define BLINK_LED 2



void app_main(void)
{
    char *ourTaskName = pcTaskGetName(NULL);
    ESP_LOGI(ourTaskName, "Hello, starting up!!\n");

    // GPIO pin initialize
    gpio_reset_pin(BLINK_LED);
    gpio_set_direction(BLINK_LED, GPIO_MODE_OUTPUT);

    // connect to wifi
    connect_wifi();

    // getting netif
    esp_netif_t* esp_netif=NULL;
    esp_netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");

    struct netif *netif = (struct netif *)esp_netif_get_netif_impl(esp_netif);
    ESP_LOGI(TAG, "Got netif for lwip");

    char char_target_ipp[16] = "192.168.8.";

    while(1){
        int deviceCount = 0;

        // loop from .0 ~ .255
        for(int i=0; i<256; i++){
            // target IP
            char char_target_ip[32];
            sprintf(char_target_ip, "%s%d", char_target_ipp, i);
            ip4_addr_t target_ip;
            ip4addr_aton(char_target_ip, &target_ip);

            gpio_set_level(BLINK_LED, 1);
            // send ARP request
            etharp_request(netif, &target_ip);
            ESP_LOGI(TAG, "Success sending ARP to %s", char_target_ip);

            // wait for request
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            gpio_set_level(BLINK_LED, 0);

            // check if received ARP
            ip4_addr_t *ipaddr_ret = NULL;
            struct eth_addr *eth_ret = NULL;
            char mac[20];
            if(etharp_find_addr(NULL, &target_ip, &eth_ret, &ipaddr_ret) != -1){
                sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X",eth_ret->addr[0],eth_ret->addr[1],eth_ret->addr[2],eth_ret->addr[3],eth_ret->addr[4],eth_ret->addr[5]);
                ESP_LOGI(TAG, "%s's MAC address is %s", char_target_ip, mac);
                deviceCount++;
            }

        }
        ESP_LOGI(TAG, "%d devices are on local network", deviceCount);

        vTaskDelay( 10*60*1000 / portTICK_PERIOD_MS);
    }
}
