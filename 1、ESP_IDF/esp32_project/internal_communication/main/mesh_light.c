/* Mesh Internal Communication Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string.h>
#include "esp_err.h"
#include "esp_mesh.h"
#include "mesh_light.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_log.h"
/*******************************************************
 *                Constants
 *******************************************************/

#define LEDC_IO_2    (2)

static const char *MESH_TAG1 = "light_on/off";
/*******************************************************
 *                Variable Definitions
 *******************************************************/
static bool s_light_inited = false;

/*******************************************************
 *                Function Definitions
 *******************************************************/
//由于官方开发板的选用多个IO口控制RGB灯颜色
//该初始化，初始化各个IO口的占空比，速度等配置
esp_err_t mesh_light_init(void)
{
    if (s_light_inited == true) {
        return ESP_OK;
    }
    s_light_inited = true;
    //注：LED control (LEDC) LED控制器
    //LED控制器定时器配置
    ledc_timer_config_t ledc_timer = {
        //通道占空比
        .duty_resolution = LEDC_TIMER_13_BIT,//13 位 LEDC PWM 占空比分辨率
        //LEDC 定时器频率 (Hz)
        .freq_hz = 5000,
        //LEDC speed speed_mode
        .speed_mode = LEDC_LOW_SPEED_MODE,//低速模式
        //通道的定时器源（0 - 3）
        .timer_num = LEDC_TIMER_0,
        //LEDC 源时钟
        .clk_cfg = LEDC_AUTO_CLK,//自动选择时钟源
    };
    ledc_timer_config(&ledc_timer);
    //LED控制器通道配置
    ledc_channel_config_t ledc_channel = {
        .channel = LEDC_CHANNEL_2,//通道0
        .duty = 100,//100%占空比
        //LED控制器输出gpio_num
        .gpio_num = LEDC_IO_2,//I0口0
        //配置中断
        .intr_type = LEDC_INTR_FADE_END,//fade中断使能
        .speed_mode = LEDC_LOW_SPEED_MODE,//低速模式
        //选择通道的定时器源 (0 - 3)
        .timer_sel = LEDC_TIMER_0,
        .hpoint = 0,
    };
    ledc_channel_config(&ledc_channel);

    //fade:从一个占空比值逐渐转换到另一个占空比值
    //占用 LEDC 模块的中断,安装 LEDC 淡入淡出功能
    ledc_fade_func_install(0);

    mesh_light_set(MESH_LIGHT_ON);//mesh RGB灯颜色设置
    return ESP_OK;
}
//设置灯亮灭
esp_err_t mesh_light_set(int color)
{
    switch (color) {
    case MESH_LIGHT_OFF:
        
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, 0);
        break;
    case MESH_LIGHT_ON:
        
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, 3000);
        break;
    default:
        /* off */
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, 3000);
    }
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2);

    return ESP_OK;
}


//mesh连接失败指示为
void mesh_disconnected_indicator(void)
{
    mesh_light_set(MESH_LIGHT_ON);
}

esp_err_t mesh_light_process(mesh_addr_t *from, uint8_t *buf, uint16_t len)
{
    mesh_light_ctl_t *in = (mesh_light_ctl_t *) buf;
    if (!from || !buf || len < sizeof(mesh_light_ctl_t)) {
        return ESP_FAIL;
    }
    if (in->token_id != MESH_TOKEN_ID || in->token_value != MESH_TOKEN_VALUE) {
        return ESP_FAIL;
    }
    if (in->cmd == MESH_CONTROL_CMD) {
    
        if (in->on) 
        {
            mesh_light_set(MESH_LIGHT_ON);
            ESP_LOGI(MESH_TAG1,"light on %d",in->on);
        } 
        else 
        {
            mesh_light_set(MESH_LIGHT_OFF);
            ESP_LOGI(MESH_TAG1,"light OFF %d",in->on);
        }
    }
    return ESP_OK;
}
