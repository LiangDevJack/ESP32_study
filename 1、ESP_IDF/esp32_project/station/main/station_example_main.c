/* WiFi station Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_http_client.h"
#include "esp_tls.h"

#include "lwip/err.h"
#include "lwip/sys.h"

// #define TAG "cdc_acm"
#define MAX_HTTP_RECV_BUFFER 512
#define MAX_HTTP_OUTPUT_BUFFER 2048

/* The examples use WiFi configuration that you can set via project configuration menu

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define EXAMPLE_ESP_WIFI_SSID      "MI11"
#define EXAMPLE_ESP_WIFI_PASS      "123456789"
//设置重连的最大次数
#define EXAMPLE_ESP_MAXIMUM_RETRY  5

extern const char howsmyssl_com_root_cert_pem_start[] asm("_binary_howsmyssl_com_root_cert_pem_start");
extern const char howsmyssl_com_root_cert_pem_end[]   asm("_binary_howsmyssl_com_root_cert_pem_end");

extern const char postman_root_cert_pem_start[] asm("_binary_postman_root_cert_pem_start");
extern const char postman_root_cert_pem_end[]   asm("_binary_postman_root_cert_pem_end");
/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1
static const char *TAG = "HTTP_CLIENT";
// static const char *TAG = "wifi softap_station";

static int s_retry_num = 0;

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) 
    {
        esp_wifi_connect();
    } 
    //如连接不成功，接着连
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) //限制重连次数
        {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } 
        else //重连次数超过最大允许重连值，标志位WIFI_FAIL_BIT置1
        {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);//置一，连接失败
        }
        ESP_LOGI(TAG,"connect to the AP fail");
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) 
    {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);//置一，连接成功
    }
}



void wifi_init_sta(void)//STA模式初始化
{
    s_wifi_event_group = xEventGroupCreate();
    //1、主任务通过调用函数 esp_netif_init() 创建一个 LwIP 核心任务，并初始化 LwIP 相关工作。返回ESP_OK表成功
    ESP_ERROR_CHECK(esp_netif_init());
    //2、主任务通过调用函数 esp_event_loop_create() 创建一个系统事件任务，并初始化应用程序事件的回调函数。
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    //3、主任务通过调用函数esp_netif_create_default_wifi_sta()创建有TCP/IP堆栈的默认网络接口实例绑定station。
    esp_netif_create_default_wifi_sta();
    //4、主任务通过调用函数 esp_wifi_init() 创建 Wi-Fi 驱动程序任务，并初始化 Wi-Fi 驱动程序。
    //始终使用 WIFI_INIT_CONFIG_DEFAULT 宏将配置初始化为默认值，这样可以保证在以后的版本中添加更多字段
    //到wifi_init_config_t时所有字段都得到正确的值。
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    //配置station 想要连接的目标 AP 的 SSID，密码（password）,安全模式的阈值
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .password = EXAMPLE_ESP_WIFI_PASS,
            /* Setting a password implies station will connect to all security modes including WEP/WPA.
             * However these modes are deprecated and not advisable to be used. Incase your Access point
             * doesn't support WPA2, these mode can be enabled by commenting below line */
	           .threshold.authmode = WIFI_AUTH_WPA2_PSK,//设置阈值
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );//选用STA WIFI模式
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );//配置STA模式
    ESP_ERROR_CHECK(esp_wifi_start() );//启动WIFI

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    /* Waiting until either the connection is established（建立） (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,//只等待这两位改变
            pdFALSE,//不用退出清零
            pdFALSE,//等待有一位为1，任务退出
            portMAX_DELAY);//portMAX_DELAY，一定等到成功才返回

    /* xEventGroupWaitBits() returns the bits before the call returned, hence（之后） we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }

    /* The event will not be processed after unregister（注销） */
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
    vEventGroupDelete(s_wifi_event_group);
}


esp_err_t client_event_get_handler(esp_http_client_event_handle_t evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ON_DATA:
        printf("HTTP_EVENT_ON_DATA: %.*s\n", evt->data_len, (char *)evt->data);
        break;

    default:
        break;
    }
    return ESP_OK;
}

// static void rest_get()
// {
//     const char *post_data = "{\"field1\":\"value1\"}";
//     char output_buffer[10];
//     esp_http_client_config_t config_get = {
//         .url = "http://127.0.0.1:8080/hi",
//         .method = HTTP_METHOD_GET,
//         .cert_pem = NULL,
//         .event_handler = client_event_get_handler};
        
//     esp_http_client_handle_t client = esp_http_client_init(&config_get);
//     esp_http_client_perform(client);
//     esp_http_client_cleanup(client);
//     esp_http_client_read_response(client, output_buffer, 10);
//     printf(output_buffer);
//     esp_http_client_write(client, post_data, strlen(post_data));
// }
// static void http_test_task(void *pvParameters)
// {
//     http_rest_with_url();
//     http_rest_with_hostname_path();
// #if CONFIG_ESP_HTTP_CLIENT_ENABLE_BASIC_AUTH
//     http_auth_basic();
//     http_auth_basic_redirect();
// #endif
// #if CONFIG_ESP_HTTP_CLIENT_ENABLE_DIGEST_AUTH
//     http_auth_digest();
// #endif
//     http_relative_redirect();
//     http_absolute_redirect();
//     http_absolute_redirect_manual();
// #if CONFIG_MBEDTLS_CERTIFICATE_BUNDLE
//     https_with_url();
// #endif
//     https_with_hostname_path();
//     http_redirect_to_https();
//     http_download_chunk();
//     http_perform_as_stream_reader();
//     https_async();
//     https_with_invalid_url();
//     http_native_request();
// #if CONFIG_MBEDTLS_CERTIFICATE_BUNDLE
//     http_partial_download();
// #endif

//     ESP_LOGI(TAG, "Finish http example");
//     vTaskDelete(NULL);
// }
esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    static char *output_buffer;  // Buffer to store response of http request from event handler
    static int output_len;       // Stores number of bytes read
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            /*
             *  Check for chunked encoding is added as the URL for chunked encoding used in this example returns binary data.
             *  However, event handler can also be used in case chunked encoding is used.
             */
            if (!esp_http_client_is_chunked_response(evt->client)) {
                // If user_data buffer is configured, copy the response into the buffer
                if (evt->user_data) {
                    memcpy(evt->user_data + output_len, evt->data, evt->data_len);
                } else {
                    if (output_buffer == NULL) {
                        output_buffer = (char *) malloc(esp_http_client_get_content_length(evt->client));
                        output_len = 0;
                        if (output_buffer == NULL) {
                            ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
                            return ESP_FAIL;
                        }
                    }
                    memcpy(output_buffer + output_len, evt->data, evt->data_len);
                }
                output_len += evt->data_len;
            }

            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
            if (output_buffer != NULL) {
                // Response is accumulated in output_buffer. Uncomment the below line to print the accumulated response
                // ESP_LOG_BUFFER_HEX(TAG, output_buffer, output_len);
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            int mbedtls_err = 0;
            esp_err_t err = esp_tls_get_and_clear_last_error((esp_tls_error_handle_t)evt->data, &mbedtls_err, NULL);
            if (err != 0) {
                ESP_LOGI(TAG, "Last esp error code: 0x%x", err);
                ESP_LOGI(TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
            }
            if (output_buffer != NULL) {
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;
            break;
        // case HTTP_EVENT_REDIRECT:
        //     ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");
        //     esp_http_client_set_header(evt->client, "From", "user@example.com");
        //     esp_http_client_set_header(evt->client, "Accept", "text/html");
        //     break;
    }
    return ESP_OK;
}
static void http_rest_with_url(void)
{
    char local_response_buffer[MAX_HTTP_OUTPUT_BUFFER] = {0};
    /**
     * NOTE: All the configuration parameters for http_client must be spefied either in URL or as host and path parameters.
     * If host and path parameters are not set, query parameter will be ignored. In such cases,
     * query parameter should be specified in URL.
     *
     * If URL as well as host and path parameters are specified, values of host and path will be considered.
     */
    esp_http_client_config_t config = {
        // .url ="http://192.168.1.2:8080/hi",
        .host = "httpbin.org",
         .path = "/get",
        //  .host = "192.168.1.2:8080",
        //  .port = 8080,
        //  .path = "/hi",
         
        // .path = "/hi",
        // .query = "esp",
         .event_handler = _http_event_handler,
        .user_data = local_response_buffer,        // Pass address of local buffer to get response
         .disable_auto_redirect = true,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
  
    // GET
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        // ESP_LOGI("TAG", "HTTP GET Status = %d, content_length = %lld",
        //         esp_http_client_get_status_code(client),
        //         esp_http_client_get_content_length(client));
         ESP_LOGE(TAG, "HTTP GET Status = %d", esp_http_client_get_status_code(client));
    } else {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }
    ESP_LOG_BUFFER_HEX(TAG, local_response_buffer, strlen(local_response_buffer));
    ESP_LOG_BUFFER_CHAR(TAG, local_response_buffer, strlen(local_response_buffer));

    // POST
    /*
    const char *post_data = "{\"field1\":\"value1\"}";
    esp_http_client_set_url(client, "http://httpbin.org/post");
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, post_data, strlen(post_data));
    err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP POST Status = %d, content_length = %lld",
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
    }

    //PUT
    esp_http_client_set_url(client, "http://httpbin.org/put");
    esp_http_client_set_method(client, HTTP_METHOD_PUT);
    err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP PUT Status = %d, content_length = %lld",
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ESP_LOGE(TAG, "HTTP PUT request failed: %s", esp_err_to_name(err));
    }

    //PATCH
    esp_http_client_set_url(client, "http://httpbin.org/patch");
    esp_http_client_set_method(client, HTTP_METHOD_PATCH);
    esp_http_client_set_post_field(client, NULL, 0);
    err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP PATCH Status = %d, content_length = %lld",
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ESP_LOGE(TAG, "HTTP PATCH request failed: %s", esp_err_to_name(err));
    }

    //DELETE
    esp_http_client_set_url(client, "http://httpbin.org/delete");
    esp_http_client_set_method(client, HTTP_METHOD_DELETE);
    err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP DELETE Status = %d, content_length = %lld",
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ESP_LOGE(TAG, "HTTP DELETE request failed: %s", esp_err_to_name(err));
    }

    //HEAD
    esp_http_client_set_url(client, "http://httpbin.org/get");
    esp_http_client_set_method(client, HTTP_METHOD_HEAD);
    err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP HEAD Status = %d, content_length = %lld",
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ESP_LOGE(TAG, "HTTP HEAD request failed: %s", esp_err_to_name(err));
    }*/

    esp_http_client_cleanup(client);
}


void app_main(void)
{
     
    //Initialize NVS
    nvs_flash_init();
  

    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    wifi_init_sta();
    //5、主任务通过调用 OS API 创建应用程序任务。
    // esp_http_client_write(client,"Hellow World!",13);
    ESP_LOGI(TAG, "Connected to AP, begin http example");
    http_rest_with_url();
    // xTaskCreate(&http_test_task, "http_test_task", 8192, NULL, 5, NULL);
        // rest_get();
 }



