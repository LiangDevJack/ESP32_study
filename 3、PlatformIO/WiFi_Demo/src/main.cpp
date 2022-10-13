#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
//#include <U8g2lib.h>



// 记得解压ArduinoJson文件夹

// JOSN是一种轻量级的数据交换格式，遵循一下规则：
// 1.并列的数据之间用逗号(,)分隔
// 2.映射用冒号(:)表示
// 3.并列数据的集合(数组)用方括号([])表示
// 4.映射的集合(对象)用大括号({})表示

//	WiFi的初始化和连接
void WiFi_Connect()
{
	WiFi.begin("MI11", "123456789");
	while (WiFi.status() != WL_CONNECTED)
	{ //这里是阻塞程序，直到连接成功
		delay(300);
		Serial.print(".");
	}
}

void setup()
{

	Serial.begin(9600); // open the serial port at 115200 bps;
	delay(100);

	Serial.print("Connecting.. ");

	WiFi_Connect();

	Serial.println("WiFi connected");

	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
}


// bilibili api: follower
// String UID = "669523240";
// String followerUrl = "http://api.bilibili.com/x/relation/stat?vmid=" + UID; // 粉丝数

// long follower = 0; // 粉丝数

// DynamicJsonDocument doc(1024);

// //	获取粉丝数
// void getBiliBiliFollower()
// {
// 	HTTPClient http;
// 	http.begin(followerUrl); //HTTP begin
// 	int httpCode = http.GET();

// 	if (httpCode > 0)
// 	{
// 		// httpCode will be negative on error
// 		Serial.printf("HTTP Get Code: %d\r\n", httpCode);

// 		if (httpCode == HTTP_CODE_OK) // 收到正确的内容
// 		{
// 			String resBuff = http.getString();

// 			//	输出示例：{"mid":123456789,"following":226,"whisper":0,"black":0,"follower":867}}
// 			Serial.println(resBuff);

// 			//	使用ArduinoJson_6.x版本，具体请移步：https://github.com/bblanchon/ArduinoJson
// 			deserializeJson(doc, resBuff); //开始使用Json解析
// 			follower = doc["data"]["follower"];
// 			Serial.printf("Follers: %ld \r\n", follower);
// 		}
// 	}
// 	else
// 	{
// 		Serial.printf("HTTP Get Error: %s\n", http.errorToString(httpCode).c_str());
// 	}

// 	http.end();
// }

	// long int wenDu = 0;
	// String cityUrl = "https://geoapi.qweather.com/v2/city/lookup?location=114.34816,34.78861&key=8b8f146e6fe4417ab31ab8e2bb066d70";
	// String weatherUrl = "https://devapi.qweather.com/v7/weather/now?location=114.34816,34.78861&key=8b8f146e6fe4417ab31ab8e2bb066d70&gzip=n&en";
	// DynamicJsonDocument doc(1024);
// void GetWeatherData()
// {
// 	HTTPClient http;
// 	http.begin(weatherUrl); //HTTP begin
// 	int httpCode = http.GET();

// 	if (httpCode > 0)
// 	{
// 		// httpCode will be negative on error
// 		Serial.printf("HTTP Get Code: %d\r\n", httpCode);

// 		if (httpCode == HTTP_CODE_OK) // 收到正确的内容
// 		{
// 			String resBuff = http.getString();

// 			//	输出示例：{"mid":123456789,"following":226,"whisper":0,"black":0,"follower":867}}
// 			Serial.println(resBuff);

// 			//	使用ArduinoJson_6.x版本，具体请移步：https://github.com/bblanchon/ArduinoJson
// 			deserializeJson(doc, resBuff); //开始使用Json解析
// 			wenDu = doc["now"]["temp"];//now类里的temp
// 			Serial.printf("wenDu: %ld \r\n", wenDu);
// 		}
// 	}
// 	else
// 	{
// 		Serial.printf("HTTP Get Error: %s\n", http.errorToString(httpCode).c_str());
// 	}

// 	http.end();

// }


/*
long int wenDu = 0;
	String weatherUrl = "https://devapi.qweather.com/v7/weather/now?location=114.34816,34.78861&key=b408a1df55fa4c22b270be9a6a27f561&gzip=n&en";
	DynamicJsonDocument doc(1024);
  void GetWeather()
  {
	  HTTPClient http;
	  http.begin(weatherUrl); 
	  int httpCode = http.GET();
	  if (httpCode > 0)
	{
		// httpCode will be negative on error
		Serial.printf("HTTP Get Code: %d\r\n", httpCode);
		if (httpCode == HTTP_CODE_OK) // 收到正确的内容
		{
			String resBuff = http.getString();
 
			//输出示例{"code":"200","updateTime":"2021-10-03T11:32+08:00","fxLink":"http://hfx.link/1tsq1","now":{"obsTime":"2021-10-03T11:23+08:00","temp":"28","feelsLike":"31","icon":"104","text":"阴","wind360":"180","windDir":"南风","windScale":"2","windSpeed":"7","humidity":"73","precip":"0.0","pressure":"993","vis":"11","cloud":"100","dew":"21"},"refer":{"sources":["QWeather","NMC","ECMWF"],"license":["no commercial use"]}}
			Serial.println(resBuff);
 
			deserializeJson(doc, resBuff);
			wenDu = doc["now"]["temp"];
			Serial.printf("温度: %ld \r\n", wenDu);
		}
	}
	else
	{
		Serial.printf("HTTP Get Error: %s\n", http.errorToString(httpCode).c_str());
	}
 
	http.end();
 
  }
*/
void loop()
{
  	HTTPClient http;
	// http.begin("http://api.bilibili.com/x/relation/stat?vmid=");
	http.begin("http://127.0.0.1:8080/hi"); //HTTP begin
	int httpCode = http.GET();
	Serial.printf("HTTP Get Code: %d\r\n", httpCode);
 	 if (httpCode == HTTP_CODE_OK) // 收到正确的内容
		{
			String resBuff = http.getString();
			Serial.println(resBuff);
		}
		http.POST("Hello world!");
	// GetWeatherData();
	//getBiliBiliFollower();
	delay(500);
}
	