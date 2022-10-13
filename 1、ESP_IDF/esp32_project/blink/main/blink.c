#include <stdio.h>
#include "driver/gpio.h"
#include "esp_log.h"
//freertos配置头文件
#include "freertos/FreeRTOS.h"
//任务处理API
#include "freertos/task.h"
//esp-idf，esp物联网开发框架头文件，不需要更改，自动生成
#include "sdkconfig.h"

#define BLINK_GPIO_NUM 2

#define START_STK_SIZE 128 //任务堆栈大小
uint8_t led_state = 0;
static const char *TAG = "FREERTOS_LED";

static void config_led(void)
{
    //配置之前，复位GPIO口值
    gpio_reset_pin(BLINK_GPIO_NUM);
    //配置GPIO口输出模式
    gpio_set_direction(BLINK_GPIO_NUM,GPIO_MODE_DEF_OUTPUT);
}

void LED_blink_task(void *pvParameters)
{
    while(1)
    {
        led_state = !led_state;
        gpio_set_level(BLINK_GPIO_NUM,led_state);
        //这行程序会影响任务的执行已解决，任务堆栈溢出。
        ESP_LOGI(TAG, "Turning the LED %s!", led_state == true ? "ON" : "OFF");
        
        //系统默认配置CONFIG_FREERTOS_HZ为100，
        //configTICK_RATE_HZ 为100，0.01s=10ms
        //pdMS_TO_TICKS 把ms转换为tick
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }  
}

void led_log_print(void * pvparam)
{
    while(1)
    {
        // ESP_LOGI(TAG, "led_log_print");
        // printf("Turning the LED %s!\n", led_state == true ? "ON" : "OFF");
        printf("Hi Dev!\n");
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void mytask(void * pvparam)
{
    while(1)
    {
        printf("Hello Jack!\n");
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    config_led();
    ESP_LOGI(TAG, "TASK START!");
    // ESP_LOGI(TAG,"Start Task!");
    // while(1)
    // {
    //     led_state = !led_state;
    //     gpio_set_level(BLINK_GPIO_NUM,led_state);
    //     ESP_LOGI(TAG, "Turning the LED %s!", led_state == true ? "ON" : "OFF");
        
    //     //系统默认配置CONFIG_FREERTOS_HZ为100，
    //     //configTICK_RATE_HZ 为100，0.01s=10ms
    //     //pdMS_TO_TICKS 把ms转换为tick
    //     vTaskDelay(pdMS_TO_TICKS(500));
    // }  
    //创造LED任务：函数，名称，堆栈大小，传递给函数的参数，优先级，任务句柄
    xTaskCreate(LED_blink_task,"LED_blink_task",2048,NULL,1,NULL);
    xTaskCreate(led_log_print , "led_log_print",1024,NULL,1,NULL);
    xTaskCreate(mytask , "mytask",1024,NULL,1,NULL);
//     vTaskStartScheduler();
 }




