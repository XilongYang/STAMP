# STAMP Demo

## 简介

一个基于Qt5和Unix平台的STAMP的不完全实现。

实现的功能：

* 无验证模式下的性能测试
* 验证模式下的性能测试
* 有状态模式的性能测试
* HMAC-SHA256加密算法
* 可选择UDP端口的服务器
* 可选择目标IP与端口的图形化客户端
* 生成NTP格式的时间戳

未实现功能：

* 获取IP数据报的TTL

项目分为三个部分：STAMP-Protocl、STAMP-Client、STAMP-Server。

## 组成

### STAMP-Protocol部分

一个由STAMP-Client和STAMP-Server共用的库，包括以下设施：

* Packet类：为以uint8_t数组存储的数据提供简单的封装，维护一个size，并控制指针生命周期。
  * 可以使用get()方法获取内部指针，但不要自行对指针进行new与delete。
  * 需要变更Packet大小时，使用拷贝或移动一个新Packet的方式，Packet的析构函数会自动释放内存。
* big_endian()函数，判断机器的字节序，大端返回true，小端返回false。
* hwswitch()模板，对一个任意长度数字类型的字节序进行变换。
* hton_any()模板，对一个任意长度的机器字节序数字类型，返回其网络字节序表示。
* ntoh_any()模板，对一个任意长度的网络字节序数字类型，返回其机器字节序表示。
* sha256()函数，接收一个Packet，以一个size为32的Packet返回其信息摘要。
* hmac_sha256()函数，接收一个Packet，以一个size为32的Packet返回其信息摘要。

STAMP-Protocol将构建为一个静态库，以供其它程序使用。

### STAMP-Server部分

一个Qt console程序，通过config.h进行配置。

```c++
// 开关验证模式
constexpr bool auth_mode = true;
// 设置密钥，仅在验证模式下有效
constexpr char key_str[] = "123456";
// 设置端口,STAMP的默认端口为862，但为了开发测试方便使用了2233。
constexpr int port = 2233;
// 开关状态模式
constexpr bool state_mode = false;
```

更改配置后重新编译运行可使配置生效。

STAMP-Server会持续监听指定端口，并对接收到的数据包进行以下处理：

* 验证数据包，通过error estimate区域与hmac区域进行验证。
  * 对于未通过验证的数据包，返回一个Packet，内容为"Varlify Failed"。
* 对于通过验证的数据包，构造并返回对应的返回包。

### STAMP-Client部分

STAMP-Client提供了一个图形界面，可以方便地进行各种选项的配置。其行为是:

* 根据配置构造一个发送包并发送至指定地址和端口
* 监听发送时使用的端口等待回应
  * 超时则放弃这个包，继续运行。
* 根据接收到的返回包计算时延和丢包情况。

## 构建

1. 使用Qt Creator构建出STAMP-Protocol
2. 使用Qt Creator打开STAMP-Server和STAMP-Client，并将已构建好的STAMP-Protocol作为库添加进工程配置文件
3. 构建STAMP-Client和STAMP-Server

## 运行截图

![image-20210617212651133](Doc/Images/image-20210617212651133.png)

