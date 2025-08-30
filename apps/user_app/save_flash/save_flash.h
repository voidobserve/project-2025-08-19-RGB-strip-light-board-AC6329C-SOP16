
#ifndef save_flash_h
#define save_flash_h
#include "led_strand_effect.h"
#include "led_strip_drive.h"
typedef enum
{
    FRIST_BYTE,    // 第一次上电标志
    LED_LEDGTH_MS, // 灯带长度高8位
    LED_LEDGTH_LS, // 灯带长度低8位
} FLASH_BYTE_FLAG;

#pragma pack(1)
typedef struct
{
    unsigned char header; // 头部 ，用于检测存放的数据是否有效
    fc_effect_t fc_save;
    unsigned char sa_ble_state; // ble模块状态
    // u16 sa_rf433_code;
} save_flash_t;

#pragma pack()

typedef struct
{
    u16 cur_speed;                            // 当前动画速度
    u16 cur_lighting_animation_time_interval; // 当前动画与动画之间的时间间隔
    // u8 rgb_neopixel_permutations;   // RGB像素排列
    u8 cur_lighting_animation_mode; // 当前灯光动画模式

    u8 cur_brightness;                 // 当前灯光亮度
    u8 cur_options;                    // 当前选项
    u8 flag_is_light_on;               // 标志位，灯光是否开启
    u8 flag_is_cur_rf_24g_mode_enable; // 标志位，当前是否要使能2.4G遥控器的功能；app控制和2.4G控制会有冲突，需要加上这个标志位来区分

    u8 is_data_valid; // 0xC5表示数据有效，其他值表示无效
} save_info_t;

extern volatile save_info_t save_info;

void read_flash_device_status_init(void);
void save_user_data_area3(void);

void save_info_read(void);
void save_info_write(void);

#endif