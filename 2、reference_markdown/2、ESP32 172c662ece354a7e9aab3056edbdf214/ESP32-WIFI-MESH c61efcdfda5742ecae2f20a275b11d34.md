# ESP32-WIFI-MESH

[参考资料](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/api-guides/esp-wifi-mesh.html#esp-wifi-mesh)

[互联网通信知识补充](%E8%AE%A1%E7%AE%97%E6%9C%BA%E7%BD%91%E7%BB%9C%E7%9F%A5%E8%AF%86%E8%A1%A5%E5%85%85%205c9a2e3c620a4b799144930782a7068d.md)

[ESP32-WIFI-MESH（实践）](ESP32-WIFI-MESH%20c61efcdfda5742ecae2f20a275b11d34/ESP32-WIFI-MESH%EF%BC%88%E5%AE%9E%E8%B7%B5%EF%BC%89%20f77806032d124ea4b946b483f4246f9f.md)

# **概述**

ESP-WIFI-MESH 是一套建立在 Wi-Fi 协议之上的网络协议。ESP-WIFI-MESH 允许分布在大范围区域内（室内和室外）的大量设备（下文称节点）在同一个 WLAN（无线局域网）中相互连接。ESP-WIFI-MESH 具有自组网和自修复的特性，也就是说 mesh 网络可以自主地构建和维护。

# 一、简介

## 1、**传统WIFI网络架构**

![Untitled](ESP32-WIFI-MESH%20c61efcdfda5742ecae2f20a275b11d34/Untitled.png)

## 2、**ESP32-WIFI-MESH网络架构**

![Untitled](ESP32-WIFI-MESH%20c61efcdfda5742ecae2f20a275b11d34/Untitled%201.png)

ESP-WIFI-MESH 与传统 Wi-Fi 网络的不同之处在于：网络中的节点不需要连接到中心节点，而是可以与相邻节点连接。各节点均负责相连节点的数据中继。由于无需受限于距离中心节点的位置，所有节点仍可互连，因此 ESP-WIFI-MESH 网络的覆盖区域更广。类似地，由于不再受限于中心节点的容量限制，ESP-WIFI-MESH 允许更多节点接入，也不易于超载。

## 3、**树型拓扑**

ESP-WIFI-MESH 建立在传统 Wi-Fi 协议之上，可被视为一种将多个独立 Wi-Fi 网络组合为一个单一 WLAN 网络的组网协议。在 Wi-Fi 网络中，station 在任何时候都仅限于与 AP 建立单个连接（上行连接），而 AP 则可以同时连接到多个 station（下行连接）。然而，ESP-WIFI-MESH 网络则允许节点同时充当 station 和 AP。因此，ESP-WIFI-MESH 中的节点可以使用 **其 SoftAP 接口建立多个下行连接**，同时使用 **其 station 接口建立一个上行连接**。这将自然产生一个由多层父子结构组成的树型网络拓扑结构。

![Untitled](ESP32-WIFI-MESH%20c61efcdfda5742ecae2f20a275b11d34/Untitled%202.png)

ESP-WIFI-MESH 是一个多跳网络，也就是说网络中的节点可以通过单跳或多跳向网络中的其他节点传送数据包。因此，ESP-WIFI-MESH 中的节点不仅传输自己的数据包，而且同时充当其他节点的中继。假设 ESP-WIFI-MESH 网络中的任意两个节点存在物理层上连接（通过单跳或多跳），则这两个节点可以进行通信。ESP-WIFI-MESH 网络中的大小（节点总数）取决于网络中允许的最大层级，以及每个节点可以具有的最大下行连接数。因此，这两个变量可用于配置 ESP-WIFI-MESH 网络的大小。

![Untitled](ESP32-WIFI-MESH%20c61efcdfda5742ecae2f20a275b11d34/Untitled%203.png)

## 4、**信标帧和 RSSI 阈值（[资料](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/api-guides/esp-wifi-mesh.html#rssi)）**

ESP-WIFI-MESH 中能够形成下行连接的每个节点（即具有 SoftAP 接口）都会定期传输 Wi-Fi 信标帧。节点可以通过信标帧让其他节点检测自己的存在和状态。空闲节点将侦听信标帧以生成一个潜在父节点列表，并与其中一个潜在父节点形成上行连接。ESP-WIFI-MESH 使用“供应商信息元素”来存储元数据，例如：

- 节点类型（根节点、中间父节点、叶子节点、空闲节点)
- 节点当前所处的层级
- 网络中允许的最大层级
- 当前子节点数量
- 可接受的最大下行连接数量

![Untitled](ESP32-WIFI-MESH%20c61efcdfda5742ecae2f20a275b11d34/Untitled%204.png)

潜在上行连接的信号强度可由潜在父节点信标帧的 RSSI 表示。为了防止节点形成弱上行连接，ESP-WIFI-MESH 采用了针对信标帧的 RSSI 阈值控制机制。如果节点检测到某节点的信标帧 RSSI 过低（即低于预设阈值），则会在尝试形成上行连接时忽略该节点。

事实上，ESP-WIFI-MESH 网络中的节点在 MAC 层仍可以接收所有的信标帧，但 RSSI 阈值控制功能可以过滤掉所有 RSSI 低于预设阈值的信标帧。

## 5、****首选父节点（[资料](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/api-guides/esp-wifi-mesh.html#id7)）**

![Untitled](ESP32-WIFI-MESH%20c61efcdfda5742ecae2f20a275b11d34/Untitled%205.png)

## 6、****路由表****

![Untitled](ESP32-WIFI-MESH%20c61efcdfda5742ecae2f20a275b11d34/Untitled%206.png)

1、**`[esp_mesh_get_routing_table()](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/api-reference/network/esp-wifi-mesh.html#_CPPv426esp_mesh_get_routing_tableP11mesh_addr_tiPi)`**获取一个节点的路由表

2、 **`[esp_mesh_get_routing_table_size()](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/api-reference/network/esp-wifi-mesh.html#_CPPv431esp_mesh_get_routing_table_sizev)`** 获取一个路由表的大小

3、 **`[esp_mesh_get_subnet_nodes_list()](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/api-reference/network/esp-wifi-mesh.html#_CPPv430esp_mesh_get_subnet_nodes_listPK11mesh_addr_tP11mesh_addr_ti)`**获取某个子节点的子路由表

4、 **`[esp_mesh_get_subnet_nodes_num()](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/api-reference/network/esp-wifi-mesh.html#_CPPv429esp_mesh_get_subnet_nodes_numPK11mesh_addr_tPi)`**获取子路由表的大小

# 二、****建立网络（[资料](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/api-guides/esp-wifi-mesh.html#mesh-building-a-network)）**

## 1、****一般过程****

ESP-WIFI-MESH 正式开始构建网络前，必须确保网络中所有节点具有相同的配置（见 **`[mesh_cfg_t](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/api-reference/network/esp-wifi-mesh.html#_CPPv410mesh_cfg_t)`**
）。每个节点必须配置 **相同 MESH 网络 ID、路由器配置和 SoftAP 配置**。

![Untitled](ESP32-WIFI-MESH%20c61efcdfda5742ecae2f20a275b11d34/Untitled%207.png)

## 2、****自动根节点选择****

![Untitled](ESP32-WIFI-MESH%20c61efcdfda5742ecae2f20a275b11d34/Untitled%208.png)

注：

1、**`esp_mesh_set_attempts()`**配置选举的最小迭代次数。

2、**得票百分比阈值**也可以使用 **`[esp_mesh_set_vote_percentage()](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/api-reference/network/esp-wifi-mesh.html#_CPPv428esp_mesh_set_vote_percentagef)`**进行配置。得票百分比阈值过低 **可能导致同一 mesh 网络中两个或多个节点成为根节点**，进而分化为多个 mesh 网络。

## 3、****用户指定根节点****

![Untitled](ESP32-WIFI-MESH%20c61efcdfda5742ecae2f20a275b11d34/Untitled%209.png)

注：

一旦指定根节点，该根节点应调用**`[esp_mesh_set_parent()](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/api-reference/network/esp-wifi-mesh.html#_CPPv419esp_mesh_set_parentPK13wifi_config_tPK11mesh_addr_t11mesh_type_ti)`**使其直接与路由器连接。类似地，所有其他节点都应该调用 **`[esp_mesh_fix_root()](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/api-reference/network/esp-wifi-mesh.html#_CPPv417esp_mesh_fix_rootb)`**放弃选举过程。

## 4、****异步上电复位（[资料](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/api-guides/esp-wifi-mesh.html#id18)）**

ESP-WIFI-MESH 网络构建可能会受到节点上电顺序的影响。如果网络中的某些节点为异步上电（即相隔几分钟上电），**网络的最终结构可能与所有节点同步上电时的理想情况不同**。延迟上电的节点将遵循以下规则：

![Untitled](ESP32-WIFI-MESH%20c61efcdfda5742ecae2f20a275b11d34/Untitled%2010.png)

**同步上电**：如果所有节点均同步上电，节点 E （-10 dB）由于路由器 RSSI 最强而成为根节点。此时形成的网络结构将与异步上电的情况截然不同。**但是，如果用户手动切换根节点，则仍可以达到同步上电的网络结构**（请见 **`[esp_mesh_waive_root()](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/api-reference/network/esp-wifi-mesh.html#_CPPv419esp_mesh_waive_rootPK11mesh_vote_ti)`**）。

注：从某种程度上，ESP-WIFI-MESH 可以自动修复部分因异步上电引起的父节点选择的偏差（请见 [父节点切换](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/api-guides/esp-wifi-mesh.html#id24))

## 5、**环路避免、检测和处理**

环路是指特定节点与其后代节点（特定节点子网中的节点）形成上行连接的情况。因此产生的循环连接路径将打破 mesh 网络的树型拓扑结构。ESP-WIFI-MESH 的节点在选择父节点时将主动排除路由表（见 [路由表](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/api-guides/esp-wifi-mesh.html#id8)）中的节点，从而避免与其子网中的节点建立上行连接并形成环路。

在存在环路的情况下，ESP-WIFI-MESH 可利用路径验证机制和能量传递机制来检测环路的产生。因与子节点建立上行连接而导致环路形成的父节点将通知子节点环路的存在，并主动断开连接。

# 三、****管理网络****

**作为一个自修复网络，ESP-WIFI-MESH 可以检测并修正网络路由中的故障**。当具有一个或多个子节点的父节点断开或父节点与其子节点之间的连接不稳定时，会发生故障。ESP-WIFI-MESH 中的子节点将自主选择一个新的父节点，并与其形成上行连接，以维持网络互连。ESP-WIFI-MESH 可以处理根节点故障和中间父节点故障。

## 1、****根节点故障****

如果根节点断开，则与其连接的节点（第二层节点）将及时检测到该根节点故障。第二层节点将主动尝试与根节点重连。但是在多次尝试失败后，第二层节点将启动新一轮的根节点选举。 **第二层中 RSSI 最强的节点将当选为新的根节点**，而剩余的第二层节点将与新的根节点（如果不在范围内的话，也可与相邻父节点连接）形成上行连接。

如果根节点和下面多层的节点（例如根节点、第二层节点和第三层节点）同时断开，则位于最浅层的仍在正常工作的节点将发起根节点选举。

![Untitled](ESP32-WIFI-MESH%20c61efcdfda5742ecae2f20a275b11d34/Untitled%2011.png)

注：如果是手动指定的根节点断开，则无法进行自动修复。**任何节点不会在存在指定根节点的情况下开始选举过程**。

## 2、****中间父节点故障****

![Untitled](ESP32-WIFI-MESH%20c61efcdfda5742ecae2f20a275b11d34/Untitled%2012.png)

注：如果子节点的父节点已被指定，则子节点不会尝试与其他潜在父节点连接。此时，该子节点将无限期地保持空闲状态。

## 3、**根节点切换**

除非根节点断开，否则 ESP-WIFI-MESH 不会自动切换根节点。即使根节点的路由器 RSSI 降低至必须断开的情况，根节点也将保持不变。根节点切换是指明确启动新选举过程的行为，即具有更强路由器 RSSI 的节点选为新的根节点。这可以用于应对根节点性能降低的情况。

要触发根节点切换，当前根节点必须明确调用 **`[esp_mesh_waive_root()](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/api-reference/network/esp-wifi-mesh.html#_CPPv419esp_mesh_waive_rootPK11mesh_vote_ti)`**以触发新的选举。

![Untitled](ESP32-WIFI-MESH%20c61efcdfda5742ecae2f20a275b11d34/Untitled%2013.png)

注：根节点切换必须要求选举，因此只有在使用自组网 ESP-WIFI-MESH 网络时才支持。换句话说，如果使用指定的根节点，则不能进行根节点切换。

## 4、**父节点切换**

父节点切换是指一个子节点将其上行连接切换到更浅一层的另一个父节点。**父节点切换是自动的**，这意味着如果较浅层出现了可用的潜在父节点（因“异步上电复位”产生），子节点将自动更改其上行连接。

所有潜在的父节点将定期发送信标帧（参见 [信标帧和 RSSI 阈值](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/api-guides/esp-wifi-mesh.html#rssi)），从而允许子节点扫描较浅层的父节点的可用性。由于父节点切换，自组网 ESP-WIFI-MESH 网络可以动态调整其网络结构，以确保每个连接均具有良好的 RSSI 值，并且网络中的层级最小。

# 四、**数据传输（[资料](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/api-guides/esp-wifi-mesh.html#mesh-data-transmission)）**

## 1、****ESP-WIFI-MESH 数据包****

ESP-WIFI-MESH 网络使用 ESP-WIFI-MESH 数据包传输数据。ESP-WIFI-MESH 数据包 **完全包含在 WiFi 数据帧**中。ESP-WIFI-MESH 网络中的多跳数据传输将涉及通过不同 Wi-Fi 数据帧在每个无线跳上传输的单个 ESP-WIFI-MESH 数据包。

![Untitled](ESP32-WIFI-MESH%20c61efcdfda5742ecae2f20a275b11d34/Untitled%2014.png)

ESP-WIFI-MESH 数据包的 **报头**包含源节点和目标节点的 MAC 地址。**选项 (option)**字段包含有关特殊类型 ESP-WIFI-MESH 数据包的信息，例如组传输或来自外部 IP 网络的数据包（请参阅 **`[MESH_OPT_SEND_GROUP](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/api-reference/network/esp-wifi-mesh.html#c.MESH_OPT_SEND_GROUP)`**和 **`[MESH_OPT_RECV_DS_ADDR](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/api-reference/network/esp-wifi-mesh.html#c.MESH_OPT_RECV_DS_ADDR)`**）。

ESP-WIFI-MESH 数据包的 **有效载荷** 包含实际的应用数据。该数据可以为原始二进制数据，也可以是使用 HTTP、MQTT 和 JSON 等应用层协议的编码数据（请见：**`[mesh_proto_t](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/api-reference/network/esp-wifi-mesh.html#_CPPv412mesh_proto_t)`**）。

**注：**当向外部 IP 网络发送 ESP-WIFI-MESH 数据包时，报头的目标地址字段将包含目标服务器的 IP 地址和端口号，而不是节点的 MAC 地址（请见：**`[mesh_addr_t](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/api-reference/network/esp-wifi-mesh.html#_CPPv411mesh_addr_t)`**）。此外，根节点将处理外发 TCP/IP 数据包的形成。

## 2、**组控制和组播（[资料](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/api-guides/esp-wifi-mesh.html#id27)）**

组播功能允许将单个 ESP-WIFI-MESH 数据包同时发送给网络中的多个节点。ESP-WIFI-MESH 中的组播可以通过“指定一个目标节点列表”或“预配置一个节点组”来实现。这两种组播方式均需调用 **`[esp_mesh_send()](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/api-reference/network/esp-wifi-mesh.html#_CPPv413esp_mesh_sendPK11mesh_addr_tPK11mesh_data_tiA_K10mesh_opt_ti)`** 实现。

在组播期间，网络中的所有节点在 MAC 层都会收到 ESP-WIFI-MESH 数据包。然而，不包括在 MAC 地址列表或目标组中的节点将简单地过滤掉这些数据包。

## 3、**广播（[资料](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/api-guides/esp-wifi-mesh.html#id28)）**

广播功能允许将单个 ESP-WIFI-MESH 数据包同时发送给网络中的所有节点。每个节点可以将一个广播包转发至其所有上行和下行连接，使得数据包尽可能快地在整个网络中传播。

## 4、**上行流量控制（[资料](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/api-guides/esp-wifi-mesh.html#id29)）**

ESP-WIFI-MESH 依赖父节点来控制其直接子节点的上行数据流。为了防止父节点的消息缓冲因上行传输过载而溢出，父节点将为每个子节点分配一个称为 **接收窗口** 的上行传输配额。 **每个子节点均必须申请接收窗口才允许进行上行传输**。接收窗口的大小可以动态调整。

ESP-WIFI-MESH 不支持任何下行流量控制。由于 [父节点切换](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/api-guides/esp-wifi-mesh.html#id24)，数据包可能会在上行传输期间丢失。

由于根节点是通向外部 IP 网络的唯一接口，因此下行节点必须了解根节点与外部 IP 网络的连接状态。否则，节点可能会尝试向一个已经与 IP 网络断开连接的根节点发送数据，从而造成不必要的传输和数据包丢失。ESP-WIFI-MESH 可以基于监测根节点和外部 IP 网络的连接状态，提供一种稳定外发数据吞吐量的机制。根节点可以通过调用 **`[esp_mesh_post_toDS_state()](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/api-reference/network/esp-wifi-mesh.html#_CPPv424esp_mesh_post_toDS_stateb)`**将自身与外部 IP 网络的连接状态广播给所有其他节点。

## 5、****双向数据流（[资料](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/api-guides/esp-wifi-mesh.html#id30)）**

![Untitled](ESP32-WIFI-MESH%20c61efcdfda5742ecae2f20a275b11d34/Untitled%2015.png)

# 五、****信道切换（[资料](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/api-guides/esp-wifi-mesh.html#mesh-channel-switching)）**