#define INFO_TYPE "nm-car" // 类型 代号
#define INFO_HARDWARE_VERSION "v2.0" // 硬件版本
#define INFO_HARDWARE_VERSION_NUMBER 200 // 硬件版本


// ------ pin配置 ------

// i2c引脚
#define PIN_SDA 4
#define PIN_SCL 5

// 指示灯，拉高开启
#define PIN_TIP_LED 3

// 重置引脚，上电时拉低10秒，将重置所有配置
#define PIN_RESET 45

// UDP 最大包长
#define UDP_MAX_PACKET_SIZE 5460

// 相机
#if INFO_HARDWARE_VERSION_NUMBER <= 100
// 相机供电开关引脚，拉低启动相机
#define PIN_CAM_ENABLE 6
// 电机驱动使能引脚，拉高使能
#define PIN_MOTOR_ENABLE 21

// 相机引脚
#define PWDN_GPIO_NUM  40
#define RESET_GPIO_NUM 42
#define XCLK_GPIO_NUM  48
#define SIOD_GPIO_NUM  1
#define SIOC_GPIO_NUM  2
#define Y2_GPIO_NUM 11
#define Y3_GPIO_NUM 46
#define Y4_GPIO_NUM 8
#define Y5_GPIO_NUM 10
#define Y6_GPIO_NUM 12
#define Y7_GPIO_NUM 14
#define Y8_GPIO_NUM 47
#define Y9_GPIO_NUM 38
#define VSYNC_GPIO_NUM 41
#define HREF_GPIO_NUM  39
#define PCLK_GPIO_NUM  13

// ------ 默认配置 ------
// 板载的IO口
#define OPTIONS_BUILT_IN_IO_DEK "[{\"pin\":7,\"name\":\"IO7\",\"func\":[\"pwm\",\"servo\",\"input\",\"digital\"]},{\"pin\":15,\"name\":\"IO15\",\"func\":[\"pwm\",\"servo\",\"input\",\"digital\"]},{\"pin\":16,\"name\":\"IO16\",\"func\":[\"pwm\",\"servo\",\"input\",\"digital\"]},{\"pin\":17,\"name\":\"IO17\",\"func\":[\"pwm\",\"servo\",\"input\",\"digital\"]},{\"pin\":18,\"name\":\"IO18\",\"func\":[\"pwm\",\"servo\",\"input\",\"digital\"]},{\"pin\":19,\"name\":\"OTG_D-\",\"func\":[\"pwm\",\"servo\",\"input\",\"digital\"]},{\"pin\":20,\"name\":\"OTG_D+\",\"func\":[\"pwm\",\"servo\",\"input\",\"digital\"]}]"
// 默认的扩展配置
#define OPTIONS_WALK_EXTENDS_DEF "[{\"id\":\"A\",\"type\":\"pwms\",\"address\":64,\"hz\":3500,\"name\":\"内置引脚扩展\",\"chip\":\"pca9685\",\"builtIn\":true,\"pins\":[{\"pin\":10,\"func\":[\"pwm\",\"digital\",\"servo\"]},{\"pin\":11,\"func\":[\"pwm\",\"digital\",\"servo\"]},{\"pin\":12,\"func\":[\"pwm\",\"digital\",\"servo\"]},{\"pin\":13,\"func\":[\"pwm\",\"digital\",\"servo\"]}],\"pwms\":[{\"pin\":15,\"hide\":true},{\"pin\":4,\"hide\":true},{\"pin\":3,\"hide\":true},{\"pin\":8,\"hide\":true}],\"digitals\":[{\"pin\":14,\"hide\":true},{\"pin\":0,\"hide\":true},{\"pin\":2,\"hide\":true},{\"pin\":1,\"hide\":true},{\"pin\":5,\"hide\":true},{\"pin\":6,\"hide\":true},{\"pin\":9,\"hide\":true},{\"pin\":7,\"hide\":true}],\"groups\":[{\"name\":\"电机1\",\"type\":\"2D1Pwm\",\"pwm\":\"p:A-15\",\"forward\":\"o:A-14\",\"back\":\"o:A-0\"},{\"name\":\"电机2\",\"type\":\"2D1Pwm\",\"pwm\":\"p:A-3\",\"forward\":\"o:A-2\",\"back\":\"o:A-1\"},{\"name\":\"电机3\",\"type\":\"2D1Pwm\",\"pwm\":\"p:A-4\",\"forward\":\"o:A-5\",\"back\":\"o:A-6\"},{\"name\":\"电机4\",\"type\":\"2D1Pwm\",\"pwm\":\"p:A-8\",\"forward\":\"o:A-9\",\"back\":\"o:A-7\"}]}]"
// 默认的PWM输出配置
#define OPTIONS_WALK_PWMS_DEF "[{\"id\":\"p:1\",\"pin\":\"io:17\",\"name\":\"大灯\"}]"
// 默认的电平输出配置
#define OPTIONS_WALK_DIGITALS_DEF "[{\"id\":\"o:1\",\"pin\":\"io:18\",\"name\":\"喇叭\"}]"
// 默认的舵机配置
#define OPTIONS_WALK_SERVOS_DEF "[{\"id\":\"s:1\",\"pin\":\"io:16\",\"name\":\"云台俯仰\",\"type\":\"180\",\"def\":0}]"

#else

// 相机引脚
#define PWDN_GPIO_NUM  40
#define RESET_GPIO_NUM 42
#define XCLK_GPIO_NUM  48
#define SIOD_GPIO_NUM  1
#define SIOC_GPIO_NUM  2
#define Y2_GPIO_NUM 12
#define Y3_GPIO_NUM 10
#define Y4_GPIO_NUM 46
#define Y5_GPIO_NUM 11
#define Y6_GPIO_NUM 13
#define Y7_GPIO_NUM 21
#define Y8_GPIO_NUM 47
#define Y9_GPIO_NUM 38
#define VSYNC_GPIO_NUM 41
#define HREF_GPIO_NUM  39
#define PCLK_GPIO_NUM  14


// ------ 默认配置 ------
// 板载的IO口
#define OPTIONS_BUILT_IN_IO_DEK "[{\"pin\":6,\"name\":\"IO6\",\"func\":[\"pwm\",\"servo\",\"input\",\"digital\"]},{\"pin\":7,\"name\":\"IO7\",\"func\":[\"pwm\",\"servo\",\"input\",\"digital\"]},{\"pin\":8,\"name\":\"IO8\",\"func\":[\"pwm\",\"servo\",\"input\",\"digital\"]},{\"pin\":18,\"name\":\"IO18\",\"func\":[\"pwm\",\"servo\",\"input\",\"digital\"]},{\"pin\":15,\"name\":\"IO15\",\"func\":[\"pwm\",\"servo\",\"input\",\"digital\"]},{\"pin\":16,\"name\":\"IO16\",\"func\":[\"pwm\",\"servo\",\"input\",\"digital\"]},{\"pin\":17,\"name\":\"IO17\",\"func\":[\"pwm\",\"servo\",\"input\",\"digital\"]},{\"pin\":19,\"name\":\"OTG_D-\",\"func\":[\"pwm\",\"servo\",\"input\",\"digital\"]},{\"pin\":20,\"name\":\"OTG_D+\",\"func\":[\"pwm\",\"servo\",\"input\",\"digital\"]}]"
// 默认的扩展配置
#define OPTIONS_WALK_EXTENDS_DEF "[{\"id\":\"A\",\"type\":\"pwms\",\"address\":65,\"hz\":3500,\"name\":\"内置引脚扩展\",\"chip\":\"pca9685\",\"builtIn\":true,\"pins\":[{\"pin\":10,\"func\":[\"pwm\",\"digital\",\"servo\"]},{\"pin\":11,\"func\":[\"pwm\",\"digital\",\"servo\"]},{\"pin\":12,\"func\":[\"pwm\",\"digital\",\"servo\"]},{\"pin\":13,\"func\":[\"pwm\",\"digital\",\"servo\"]}],\"pwms\":[{\"pin\":15,\"hide\":true},{\"pin\":4,\"hide\":true},{\"pin\":3,\"hide\":true},{\"pin\":8,\"hide\":true}],\"digitals\":[{\"pin\":14,\"hide\":true},{\"pin\":0,\"hide\":true},{\"pin\":2,\"hide\":true},{\"pin\":1,\"hide\":true},{\"pin\":5,\"hide\":true},{\"pin\":6,\"hide\":true},{\"pin\":9,\"hide\":true},{\"pin\":7,\"hide\":true}],\"groups\":[{\"name\":\"电机1\",\"type\":\"2D1Pwm\",\"pwm\":\"p:A-15\",\"forward\":\"o:A-14\",\"back\":\"o:A-0\"},{\"name\":\"电机2\",\"type\":\"2D1Pwm\",\"pwm\":\"p:A-3\",\"forward\":\"o:A-2\",\"back\":\"o:A-1\"},{\"name\":\"电机3\",\"type\":\"2D1Pwm\",\"pwm\":\"p:A-4\",\"forward\":\"o:A-5\",\"back\":\"o:A-6\"},{\"name\":\"电机4\",\"type\":\"2D1Pwm\",\"pwm\":\"p:A-8\",\"forward\":\"o:A-9\",\"back\":\"o:A-7\"}]}]"
// 默认的PWM输出配置
#define OPTIONS_WALK_PWMS_DEF "[{\"id\":\"p:1\",\"pin\":\"io:16\",\"name\":\"大灯\"}]"
// 默认的电平输出配置
#define OPTIONS_WALK_DIGITALS_DEF "[{\"id\":\"o:1\",\"pin\":\"io:17\",\"name\":\"喇叭\"}]"
// 默认的舵机配置
#define OPTIONS_WALK_SERVOS_DEF "[{\"id\":\"s:1\",\"pin\":\"io:15\",\"name\":\"云台俯仰\",\"type\":\"180\",\"def\":0}]"

#endif

// ------ 默认配置(全版本通用) ------
// 默认的组合配置
#define OPTIONS_WALK_GROUPS_DEF "[]"
// 默认的云台配置
#define OPTIONS_PZT_DEF "{\"pitch_device\":\"s:1\"}"
// 默认的底盘配置
#define OPTIONS_CHASSIS_DEF "{\"mode\":\"tank\",\"motor\":{\"top-left\":\"g:A-0\",\"top-right\":\"g:A-1\",\"bottom-left\":\"g:A-2\",\"bottom-right\":\"g:A-3\"}}"
// 默认的屏幕配置
#define OPTIONS_SCREEN_DEF "{\"enable\":false,\"address\":\"60\",\"module\":[{\"type\":0,\"prefix\":\"wifi:\"},{\"type\":2,\"prefix\":\"ip: \"},{\"type\":1,\"prefix\":\"rssi: \"}]}"
// 默认的传感器配置
#define OPTIONS_SENSORS_DEF "[{\"id\":\"power\",\"name\":\"电源\",\"type\":\"board-adc\",\"pin\":9,\"u_t\":10000}]"
