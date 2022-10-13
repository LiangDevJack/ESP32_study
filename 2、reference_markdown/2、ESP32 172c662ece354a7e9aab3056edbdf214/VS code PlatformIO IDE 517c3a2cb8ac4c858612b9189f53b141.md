# VS code PlatformIO IDE

![Untitled](VS%20code%20PlatformIO%20IDE%20517c3a2cb8ac4c858612b9189f53b141/Untitled.png)

![Untitled](VS%20code%20PlatformIO%20IDE%20517c3a2cb8ac4c858612b9189f53b141/Untitled%201.png)

- 建工程（VS code PlatformIO IDE）
    
    

![Untitled](VS%20code%20PlatformIO%20IDE%20517c3a2cb8ac4c858612b9189f53b141/Untitled%202.png)

工程名称、板子类型

![Untitled](VS%20code%20PlatformIO%20IDE%20517c3a2cb8ac4c858612b9189f53b141/Untitled%203.png)

选择工程位置

![Untitled](VS%20code%20PlatformIO%20IDE%20517c3a2cb8ac4c858612b9189f53b141/Untitled%204.png)

- LED
    
    ```cpp
    
    String UID = "669523240";
    String followerUrl = "http://api.bilibili.com/x/relation/stat?vmid=" + UID; // 粉丝数
    long follower = 0; // 粉丝数
    DynamicJsonDocument doc(1024)；
    
    void getBiliBiliFollower()
    {
    	HTTPClient http;
    	http.begin(followerUrl); //HTTP begin
    	int httpCode = http.GET();
    
    	if (httpCode > 0)
    	{
    		// httpCode will be negative on error
    		Serial.printf("HTTP Get Code: %d\r\n", httpCode);
    
    		if (httpCode == HTTP_CODE_OK) // 收到正确的内容
    		{
    			String resBuff = http.getString();
    
    			//	输出示例：{"mid":123456789,"following":226,"whisper":0,"black":0,"follower":867}}
    			Serial.println(resBuff);
    
    			//	使用ArduinoJson_6.x版本，具体请移步：https://github.com/bblanchon/ArduinoJson
    			deserializeJson(doc, resBuff); //开始使用Json解析
    			follower = doc["data"]["follower"];
    			Serial.printf("Follers: %ld \r\n", follower);
    		}
    	}
    	else
    	{
    		Serial.printf("HTTP Get Error: %s\n", http.errorToString(httpCode).c_str());
    	}
    
    	http.end();
    }
    
    //扩展
    //和风天气
    	long int wenDu = 0;
    	String cityUrl = "https://geoapi.qweather.com/v2/city/lookup?location=114.34816,34.78861&key=8b8f146e6fe4417ab31ab8e2bb066d70";
    	String weatherUrl = "https://devapi.qweather.com/v7/weather/now?location=114.34816,34.78861&key=8b8f146e6fe4417ab31ab8e2bb066d70&gzip=n&en";
    	DynamicJsonDocument doc(1024);
    ```
    
- 中断
- 定时器&PWM
- WIFI

*API（Application Programming Interface,应用程序编程接口）是一些预先定义的函数，目的是提供应用程序与开发人员基于某软件或硬件得以访问一组例程的能力，而又无需访问源码，或理解内部工作机制的细节。*

和风天气API为用户提供一个简洁的RESTful API接口，用以访问基于位置的天气数据，包括实况天气、30天预报、逐小时预报、空气质量AQI，灾害预警、分钟级降水、生活指数等天气数据服务。

JSON(JavaScript Object Notation) 是一种轻量级的数据交换格式。易于人阅读和编写。同时也易于机器解析和生成。

## JOSN

JOSN是一种轻量级的数据交换格式，遵循以下规则：

1.并列的数据之间用逗号(,)分隔

2.映射用冒号(:)表示

3.并列数据的集合(数组)用方括号([])表示

4.映射的集合(对象)用大括号({})表示

![Untitled](VS%20code%20PlatformIO%20IDE%20517c3a2cb8ac4c858612b9189f53b141/Untitled%205.png)

出现乱码的情况可以在PlatformIO 配置里面添加中文包U8g2？

AP模式（AT指令）：提供ip地址，充当路由器供其他设备连接。

STA模式：连接WIFI，通过API获取网站信息。

**后续：**

PlatformIO IDE 是下一代的物联网生态系统 [PlatformIO](http://www.oschina.net/p/platformio) 的集成开发环境。基于 [Github Atom](http://www.oschina.net/p/github-atom) "hackable" 文本编辑器。支持多平台。

最近，[Platform.io](https://link.zhihu.com/?target=https%3A//community.platformio.org/t/support-for-esp-idf-4-1-protected-management-frames-pmf-improvements-to-bluetooth-and-wi-fi-stacks-new-menuconfig-tool/15810) 宣布支持乐鑫 ESP-IDF 的最新版本 V4.1，并发布 PlatformIO **Espressif32**开发平台的 V2.0.0 版本。PlatformIO 及其专业的一键式解决方案（[PIO 统一调试器](https://link.zhihu.com/?target=https%3A//docs.platformio.org/en/latest/plus/debugging.html) 和 [PIO 单元测试](https://link.zhihu.com/?target=https%3A//docs.platformio.org/en/latest/plus/unit-testing.html)）开箱即用，且兼容知名操作系统（Windows、macOS 和 Linux）及十余种主流 IDE，用户无需依赖第三方工具或构建系统即可进行开发。