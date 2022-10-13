# ESP-IDF FreeRTOS

# problems & solving

## 1、（solved）日志干扰程序执行？🧐

```c
//ESP_LOGI放在任务函数外会被执行（任务外没有划定堆栈大小），放在任务则不被执行（程序跑飞）。已解决，任务堆栈溢出。
void LED_blink_task(void *pvParameters)
{
    while(1)
    {
        led_state = !led_state;
        gpio_set_level(BLINK_GPIO_NUM,led_state);
        //这行程序会影响任务的执行
        // ESP_LOGI(TAG, "Turning the LED %s!", led_state == true ? "ON" : "OFF");
        
        //系统默认配置CONFIG_FREERTOS_HZ为100，
        //configTICK_RATE_HZ 为100，0.01s=10ms
        //pdMS_TO_TICKS 把ms转换为tick
        vTaskDelay(500 / portTICK_PERIOD_MS);

    }  
}

//加上esp_loge日志打印，任务堆栈1024不够，得需要2048大小
xTaskCreate(LED_blink_task,"LED_blink_task",2048,NULL,1,NULL);
```

![Untitled](ESP-IDF%20FreeRTOS%20f88f23fb158042b194fdb6282dbf323d/Untitled.png)

### 问题延伸：怎么测量一个函数所需要多少堆栈大小？

### ESP日志打印知识补充

- `ESP_LOGE` - error (lowest)
- `ESP_LOGW` - warning
- `ESP_LOGI` - info
- `ESP_LOGD` - debug
- `ESP_LOGV` - verbose (highest)

此外，`ESP_EARLY_LOGx`这些宏中的每一个都有不同的版本，例如**`[ESP_EARLY_LOGE](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/log.html#c.ESP_EARLY_LOGE)`**. 这些版本只能在早期启动代码中显式使用，在堆分配器和系统调用被初始化之前。编译引导加载程序时也可以使用普通`ESP_LOGx`宏，但它们将回退到与`ESP_EARLY_LOGx`宏相同的实现。

这些宏也有`ESP_DRAM_LOGx`各自的版本，例如**`[ESP_DRAM_LOGE](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/log.html#c.ESP_DRAM_LOGE)`**. 这些版本用于在禁用中断或无法访问闪存缓存的情况下发生日志记录的某些地方。应尽可能少使用此宏，因为出于性能原因应避免登录这些类型的代码。

内部临界区中断被禁用，因此只能使用`ESP_DRAM_LOGx`（首选）或`ESP_EARLY_LOGx`. 尽管可以在这些情况下登录，但如果您的程序可以构造成不需要它，那就更好了。

# 一、配置

ESP-IDF FreeRTOS 基于 Vanilla FreeRTOS v10.4.3。

Vanilla FreeRTOS允许端口和应用程序通过添加各种`#define config...`
宏到`FreeRTOSConfig.h`

去配置内核。通过这些宏，可以启用或禁用内核的调度行为和各种内核特性。**但是，在 ESP-IDF FreeRTOS 中，``FreeRTOSConfig.h`` 文件被视为私有文件，用户不得修改**。任何向用户公开的 FreeRTOS 配置都将通过 menuconfig 完成。

# ****ESP-IDF FreeRTOS 应用****

与 Vanilla FreeRTOS 不同，用户不得调用**`[vTaskStartScheduler()](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/api-reference/system/freertos.html#_CPPv419vTaskStartSchedulerv)`**. 相反，ESP-IDF FreeRTOS 会自动启动。入口点是用户定义的函数。`void app_main(void)`

- 通常，用户会从`app_main`.
- 该`app_main`函数可以在任何时候返回（即在应用程序终止之前）。
- 该`app_main`函数是从`main`任务中调用的。

该`main`任务是 ESP-IDF 在启动期间自动生成的多个任务之一。这些任务是：

![Untitled](ESP-IDF%20FreeRTOS%20f88f23fb158042b194fdb6282dbf323d/Untitled%201.png)