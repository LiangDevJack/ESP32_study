# problems&solving

1、查电脑IP

![Untitled](problems&solving%2096361975eac845b8bdbb90c684509ca2/Untitled.png)

127.0.0.1是本地环回测试地址

2、DNS查阅失败

```cpp
E (3634) TRANS_TCP: DNS lookup failed err=202 res=0x0
E (3634) HTTP_CLIENT: Connection failed, sock < 0
E (3634) HTTP_CLIENT: HTTP GET request failed: ESP_ERR_HTTP_CONNECT

E (3625) HTTP_CLIENT: config should have either URL or host & path
//要么host和path存在，要么就的有url
//URL=host:port/path
```

![Untitled](problems&solving%2096361975eac845b8bdbb90c684509ca2/Untitled%201.png)

底层是字符串拼接

url+host+port+path，公网上用网址好记，服务器IP换了没事，局域网用IP地址好编程

3、http get和post

[https://baijiahao.baidu.com/s?id=1696438195609065302&wfr=spider&for=pc](https://baijiahao.baidu.com/s?id=1696438195609065302&wfr=spider&for=pc)

get请求：从服务器获取资源 post： 把资源发到服务器

![Untitled](problems&solving%2096361975eac845b8bdbb90c684509ca2/Untitled%202.png)

浏览器是http client、请求名称是hi的资源、从绑定在套接字127.0.0.1：8080上的http server上请求

![Untitled](problems&solving%2096361975eac845b8bdbb90c684509ca2/Untitled%203.png)

服务器代码

![Untitled](problems&solving%2096361975eac845b8bdbb90c684509ca2/Untitled%204.png)

？后面的是你要的资源参数、key也是参数

请求啥资源就在端口后面加，发送数据就用post
http是数据传输的协议
然后网页是在这个基础上搭建出来的
用http请求html、css和js的资源

tcp/ip 是 http 的底层

用http协议把文件发给浏览器，浏览器解析出网页

4、分布式物联网操作系统

![Untitled](problems&solving%2096361975eac845b8bdbb90c684509ca2/Untitled%205.png)

![Untitled](problems&solving%2096361975eac845b8bdbb90c684509ca2/Untitled%206.png)