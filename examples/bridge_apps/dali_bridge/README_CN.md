# DALI 桥接灯光(Matter)

本示例通过 ESP32 RMT 外设扫描 DALI(IEC 62386)总线,并为每一个发现的 DALI 短地址创建一个 **Matter 桥接灯光 Endpoint(Bridged Node)**。DT6 控制装置暴露为 Dimmable Light,DT8 控制装置暴露为 Extended Color Light。

整个应用是一个 Matter Bridge:一套配网身份 + 一个 Aggregator Endpoint + 每个 DALI 控制装置一个 Bridged Node。桥接端点持久化在 NVS 中,重启后按原端点 ID 恢复。该设备兼容 **Home Assistant**(Matter 集成)及其他 Matter 控制器。

## Matter Endpoint 拓扑

```text
Endpoint 0  Root Node
Endpoint 1  Aggregator
Endpoint 2… Bridged Dimmable Light / Extended Color Light Endpoint
```

## 新设备添加

DALI 总线只在模组上电启动时扫描一次。程序运行期间，轮询只查询已经建过桥接端点的短地址，因此此时接到总线上的新控制装置不会被添加。

要添加新设备，需要重启模组。重启后桥会重新扫描总线，为新的短地址创建桥接端点，并把该端点上报给已经配网的控制器。桥本身不会重新进入配网。

已经写入 NVS 的 Endpoint ID 保持不变。新端点使用下一个未占用的 ID：在上一台桥接灯的 Endpoint 上加 1；如果中间已有端点占用了编号，则会更大。例如上一台设备是 Endpoint 4，新设备就是 Endpoint 5 或更大的编号。

## 可达性

桥接应用每 5 秒对所有已知 DALI 短地址发送 `QUERY STATUS` 轮询:

- 设备失联(被移除、断电或灯管故障)时,其
`BridgedDeviceBasicInformation::Reachable` 属性被置为 **false**;
- 桥接端点本身保留,设备重新应答后属性恢复为 **true**,并把持久化的开关/亮度状态推送到硬件。

## 支持的 Matter 集群

| 集群                            | 属性                                                          | DALI 映射                   |
| ----------------------------- | ----------------------------------------------------------- | ------------------------- |
| BridgedDeviceBasicInformation | Reachable                                                   | `QUERY STATUS` 轮询         |
| OnOff                         | OnOff                                                       | `OFF` 命令 / DAPC 恢复上次亮度    |
| LevelControl                  | CurrentLevel(1–254)                                         | 直接弧功率控制(DAPC)             |
| ColorControl(DT8)             | ColorTemperatureMireds / CurrentX+Y / CurrentHue+Saturation | IEC 62386-209 `SET COLOR` |

## 硬件连接

| 信号      | 默认 GPIO | Kconfig 选项            |
| ------- | ------- | --------------------- |
| DALI RX | GPIO 5                       | `CONFIG_DALI_RX_GPIO` |
| DALI TX | GPIO 6(ESP32/H2 上为 GPIO 4) | `CONFIG_DALI_TX_GPIO` |

ESP32 的 GPIO6 连接片内 SPI Flash，测试用的 ESP32-H2-DevKitM-1 开发板没有引出 GPIO6。这两个目标的 `CONFIG_DALI_TX_GPIO` 默认值均为 GPIO4，RX 仍为 GPIO5。

典型接口电路:将 ESP32 GPIO 经光耦隔离器连接到 DALI 供电电源(例如简单的 16 V 限流电源)。
如果硬件存在奇数次信号反相,请在 `sdkconfig.defaults` 中设置 `CONFIG_DALI_TX_INVERT` / `CONFIG_DALI_RX_INVERT`。两者默认均为 `y`(对应反相参考收发器);非反相接口需改为 `n`。

本示例项目使用到的硬件有：

- esp32c6模组/开发板
- DALI2通信扩展板
- Dali总线电源
- HDR-60-24变压器
- 5通道DALI调光控制器（DT6/8）
- 4通道DLI调光控制器
- RGBCW彩色灯条
- COB/Led灯条

硬件连接示意图：

```text
                                [ Type-C 5V 供电 ]
                                       |
                                       v
                             +-------------------+
                             |        ESP32      |
                             |    + DALI转TTL    |
                             +-------------------+
                                 | DA2       | DA1
                                 |           |
      +----------------+         |           |
      | DALI Bus Power |         |           |
      | DALI 总线电源  |---------+           |
      | (Part 101)     |---------------------+
      +----------------+         |           |
               ^                 |           |   <-- DALI 本地总线 (两线制，无极性主干网)
               |                 |           |
         ~ AC 50/60Hz ~          |           |
         (市电 220V)             |           |
               |                 +-----------+
               v                 |           |
      +----------------+         |           |
      | Constant Volt. |         |           |
      | 恒压直流电源   |           |           |
      | (12-48V DC)    |         |           |
      +----------------+         |           |
         | V+      | V-          |           |
         |         |             |           |
         |         |             |           |
         |         |             |           |
         |         |             |           |
         |         |             |           |        +--------------------+
         |         |             |           |        |                    |
         |         |             |           +------->| DA1                |
         |         |             +------------------->| DA2                |
         |         |             |           |        | DALI 驱动器 (DT6)  |
         |         +----------------------+--+------->| DC_IN- (Part 207)  |
         +--------------------------------+--+------->| DC_IN+    LED-  V+ |
         |         |             |           |         +------------+----+--+
         |         |             |           |                      |    |
         |         |             |           |                      v    v
         |         |             |           |         +--------------------+
         |         |             |           |         |    单色 LED 灯具   |
         |         |             |           |         +--------------------+
         |         |             |           |
         |         |             v           v        +---------------------------+
         |         |             |           |        |                           |
         |         |             |           +------->| DA1                       |
         |         |             +------------------->| DA2                       |
         |         |                                  |      DALI 驱动器 (DT8)    |
         |         +--------------------------------->| DC_IN- (Part 209)         |
         +------------------------------------------->| DC_IN+                    |
                                                      |    (R   G    B  C  W  V+) |
                                                      +-----+---+---+---+--+---+---
                                                            |   |   |   |  |   |
                                                            v   v   v   v  v   v
                                                         +------------------------+
                                                         |     RGBCW LED 灯具     |
                                                         +------------------------+
```

## 编译与烧录

```bash
# 设置必要的环境变量
cd /path/to/esp-matter
. ./export.sh                  # 设置esp-matter环境变量
cd /path/to/esp-idf
. ./export.sh                  # 设置esp-idf环境变量

cd $ESP_MATTER_PATH/examples/bridge_apps/dali_bridge

# 配置目标芯片(可选,默认值已在 sdkconfig.defaults 中设置)
idf.py set-target esp32c6     # 也可选 esp32、esp32s3、esp32h2 等
idf.py menuconfig

# 编译并烧录
idf.py build flash monitor
```

ESP32-H2 没有 Wi-Fi。`sdkconfig.defaults.esp32h2` 会关闭 Wi-Fi Station 并启用 OpenThread，该目标通过 Thread 配网。`CONFIG_DALI_TX_GPIO` 在 ESP32(GPIO6 连接片内 SPI Flash)和 ESP32-H2(DevKitM-1 未引出 GPIO6)上默认 GPIO4，其它目标默认 GPIO6。

> 桥接设备上限为 31 个(`CONFIG_ESP_MATTER_MAX_DYNAMIC_ENDPOINT_COUNT=33` 减去 root 和 aggregator)。DALI 设备更多时需调大该值。

## 项目结构

```
dali_bridge/
├── CMakeLists.txt          顶层项目 CMake 文件
├── partitions.csv          自定义分区表(支持 OTA,4 MB Flash)
├── sdkconfig.defaults      默认 Kconfig 配置
└── main/
    ├── CMakeLists.txt      组件注册及 dali 组件依赖声明
    ├── Kconfig.projbuild   示例级 Kconfig 菜单(DALI GPIO 默认值/反相)
    ├── app_priv.h          驱动 API 声明及编译时常量
    ├── app_main.cpp        Matter bridge 数据模型初始化
    └── app_driver.cpp      硬件驱动层(DALI 总线命令封装)
```
