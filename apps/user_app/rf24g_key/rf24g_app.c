
#include "system/includes.h"
#include "task.h"
#include "event.h"
#include "btstack/btstack_typedef.h"
#include "app_config.h"
#include "rf24g_app.h"
#include "WS2812FX.H"
#include "led_strip_sys.h"

#include "../../../apps/user_app/lighting_animation/lighting_animation.h" // 灯光动画
// #include "../../../apps/user_app/led_strip/led_strand_effect.h"           // fc_effect 变量定义
#include "../../../apps/user_app/save_flash/save_flash.h" // 包含 save_info_t 结构体类型定义，save_info 结构体变量定义

#include "dp_data_tran.h"

#if (TCFG_RF24GKEY_ENABLE)

const u8 rf24g_key_event_table[][RF34G_KEY_EVENT_MAX + 1] = {
    {RF24G_KEY_ON_OFF, RF24G_KEY_EVENT_ON_OFF_CLICK, RF24G_KEY_EVENT_ON_OFF_HOLD, RF24G_KEY_EVENT_ON_OFF_LOOSE},

    {RF24G_KEY_MODE_ADD, RF24G_KEY_EVENT_MODE_ADD_CLICK, RF24G_KEY_EVENT_MODE_ADD_HOLD, RF24G_KEY_EVENT_MODE_ADD_LOOSE},
    {RF24G_KEY_MODE_SUB, RF24G_KEY_EVENT_MODE_SUB_CLICK, RF24G_KEY_EVENT_MODE_SUB_HOLD, RF24G_KEY_EVENT_MODE_SUB_LOOSE},

    {RF24G_KEY_SPEED_ADD, RF24G_KEY_EVENT_SPEED_ADD_CLICK, RF24G_KEY_EVENT_SPEED_ADD_HOLD, RF24G_KEY_EVENT_SPEED_ADD_LOOSE},
    {RF24G_KEY_SPEED_SUB, RF24G_KEY_EVENT_SPEED_SUB_CLICK, RF24G_KEY_EVENT_SPEED_SUB_HOLD, RF24G_KEY_EVENT_SPEED_SUB_LOOSE},

    {RF24G_KEY_DEMO, RF24G_KEY_EVENT_DEMO_CLICK, RF24G_KEY_EVENT_DEMO_HOLD, RF24G_KEY_EVENT_DEMO_LOOSE},

    {RF24G_KEY_COLOR_ADD, RF24G_KEY_EVENT_COLOR_ADD_CLICK, RF24G_KEY_EVENT_COLOR_ADD_HOLD, RF24G_KEY_EVENT_COLOR_ADD_LOOSE},
    {RF24G_KEY_COLOR_SUB, RF24G_KEY_EVENT_COLOR_SUB_CLICK, RF24G_KEY_EVENT_COLOR_SUB_HOLD, RF24G_KEY_EVENT_COLOR_SUB_LOOSE},

    {RF24G_KEY_BRIGHT_ADD, RF24G_KEY_EVENT_BRIGHT_ADD_CLICK, RF24G_KEY_EVENT_BRIGHT_ADD_HOLD, RF24G_KEY_EVENT_BRIGHT_ADD_LOOSE},
    {RF24G_KEY_BRIGHT_SUB, RF24G_KEY_EVENT_BRIGHT_SUB_CLICK, RF24G_KEY_EVENT_BRIGHT_SUB_HOLD, RF24G_KEY_EVENT_BRIGHT_SUB_LOOSE},

    {RF24G_KEY_B, RF24G_KEY_EVENT_B_CLICK, RF24G_KEY_EVENT_B_HOLD, RF24G_KEY_EVENT_B_LOOSE},
    {RF24G_KEY_F, RF24G_KEY_EVENT_F_CLICK, RF24G_KEY_EVENT_F_HOLD, RF24G_KEY_EVENT_F_LOOSE},
    {RF24G_KEY_G, RF24G_KEY_EVENT_G_CLICK, RF24G_KEY_EVENT_G_HOLD, RF24G_KEY_EVENT_G_LOOSE},
};

/*
    用 AK803-SOP16 写的遥控器
    完整的有28个按键（实际可能不会全部用到）
*/
const u8 rf24g_key_type_28keys_table[][4 + 1] = {
    {RF24G_KEY_VAL_R1C1, RF24G_28_KEY_EVENT_R1C1_CLICK, RF24G_28_KEY_EVENT_R1C1_LONG, RF24G_28_KEY_EVENT_R1C1_HOLD, RF24G_28_KEY_EVENT_R1C1_LOOSE},
    {RF24G_KEY_VAL_R1C2, RF24G_28_KEY_EVENT_R1C2_CLICK, RF24G_28_KEY_EVENT_R1C2_LONG, RF24G_28_KEY_EVENT_R1C2_HOLD, RF24G_28_KEY_EVENT_R1C2_LOOSE},
    {RF24G_KEY_VAL_R1C3, RF24G_28_KEY_EVENT_R1C3_CLICK, RF24G_28_KEY_EVENT_R1C3_LONG, RF24G_28_KEY_EVENT_R1C3_HOLD, RF24G_28_KEY_EVENT_R1C3_LOOSE},
    {RF24G_KEY_VAL_R1C4, RF24G_28_KEY_EVENT_R1C4_CLICK, RF24G_28_KEY_EVENT_R1C4_LONG, RF24G_28_KEY_EVENT_R1C4_HOLD, RF24G_28_KEY_EVENT_R1C4_LOOSE},

    {RF24G_KEY_VAL_R2C1, RF24G_28_KEY_EVENT_R2C1_CLICK, RF24G_28_KEY_EVENT_R2C1_LONG, RF24G_28_KEY_EVENT_R2C1_HOLD, RF24G_28_KEY_EVENT_R2C1_LOOSE},
    {RF24G_KEY_VAL_R2C2, RF24G_28_KEY_EVENT_R2C2_CLICK, RF24G_28_KEY_EVENT_R2C2_LONG, RF24G_28_KEY_EVENT_R2C2_HOLD, RF24G_28_KEY_EVENT_R2C2_LOOSE},
    {RF24G_KEY_VAL_R2C3, RF24G_28_KEY_EVENT_R2C3_CLICK, RF24G_28_KEY_EVENT_R2C3_LONG, RF24G_28_KEY_EVENT_R2C3_HOLD, RF24G_28_KEY_EVENT_R2C3_LOOSE},
    {RF24G_KEY_VAL_R2C4, RF24G_28_KEY_EVENT_R2C4_CLICK, RF24G_28_KEY_EVENT_R2C4_LONG, RF24G_28_KEY_EVENT_R2C4_HOLD, RF24G_28_KEY_EVENT_R2C4_LOOSE},

    {RF24G_KEY_VAL_R3C1, RF24G_28_KEY_EVENT_R3C1_CLICK, RF24G_28_KEY_EVENT_R3C1_LONG, RF24G_28_KEY_EVENT_R3C1_HOLD, RF24G_28_KEY_EVENT_R3C1_LOOSE},
    {RF24G_KEY_VAL_R3C2, RF24G_28_KEY_EVENT_R3C2_CLICK, RF24G_28_KEY_EVENT_R3C2_LONG, RF24G_28_KEY_EVENT_R3C2_HOLD, RF24G_28_KEY_EVENT_R3C2_LOOSE},
    {RF24G_KEY_VAL_R3C3, RF24G_28_KEY_EVENT_R3C3_CLICK, RF24G_28_KEY_EVENT_R3C3_LONG, RF24G_28_KEY_EVENT_R3C3_HOLD, RF24G_28_KEY_EVENT_R3C3_LOOSE},
    {RF24G_KEY_VAL_R3C4, RF24G_28_KEY_EVENT_R3C4_CLICK, RF24G_28_KEY_EVENT_R3C4_LONG, RF24G_28_KEY_EVENT_R3C4_HOLD, RF24G_28_KEY_EVENT_R3C4_LOOSE},

    {RF24G_KEY_VAL_R4C1, RF24G_28_KEY_EVENT_R4C1_CLICK, RF24G_28_KEY_EVENT_R4C1_LONG, RF24G_28_KEY_EVENT_R4C1_HOLD, RF24G_28_KEY_EVENT_R4C1_LOOSE},
    {RF24G_KEY_VAL_R4C2, RF24G_28_KEY_EVENT_R4C2_CLICK, RF24G_28_KEY_EVENT_R4C2_LONG, RF24G_28_KEY_EVENT_R4C2_HOLD, RF24G_28_KEY_EVENT_R4C2_LOOSE},
    {RF24G_KEY_VAL_R4C3, RF24G_28_KEY_EVENT_R4C3_CLICK, RF24G_28_KEY_EVENT_R4C3_LONG, RF24G_28_KEY_EVENT_R4C3_HOLD, RF24G_28_KEY_EVENT_R4C3_LOOSE},
    {RF24G_KEY_VAL_R4C4, RF24G_28_KEY_EVENT_R4C4_CLICK, RF24G_28_KEY_EVENT_R4C4_LONG, RF24G_28_KEY_EVENT_R4C4_HOLD, RF24G_28_KEY_EVENT_R4C4_LOOSE},

    {RF24G_KEY_VAL_R5C1, RF24G_28_KEY_EVENT_R5C1_CLICK, RF24G_28_KEY_EVENT_R5C1_LONG, RF24G_28_KEY_EVENT_R5C1_HOLD, RF24G_28_KEY_EVENT_R5C1_LOOSE},
    {RF24G_KEY_VAL_R5C2, RF24G_28_KEY_EVENT_R5C2_CLICK, RF24G_28_KEY_EVENT_R5C2_LONG, RF24G_28_KEY_EVENT_R5C2_HOLD, RF24G_28_KEY_EVENT_R5C2_LOOSE},
    {RF24G_KEY_VAL_R5C3, RF24G_28_KEY_EVENT_R5C3_CLICK, RF24G_28_KEY_EVENT_R5C3_LONG, RF24G_28_KEY_EVENT_R5C3_HOLD, RF24G_28_KEY_EVENT_R5C3_LOOSE},
    {RF24G_KEY_VAL_R5C4, RF24G_28_KEY_EVENT_R5C4_CLICK, RF24G_28_KEY_EVENT_R5C4_LONG, RF24G_28_KEY_EVENT_R5C4_HOLD, RF24G_28_KEY_EVENT_R5C4_LOOSE},

    {RF24G_KEY_VAL_R6C1, RF24G_28_KEY_EVENT_R6C1_CLICK, RF24G_28_KEY_EVENT_R6C1_LONG, RF24G_28_KEY_EVENT_R6C1_HOLD, RF24G_28_KEY_EVENT_R6C1_LOOSE},
    {RF24G_KEY_VAL_R6C2, RF24G_28_KEY_EVENT_R6C2_CLICK, RF24G_28_KEY_EVENT_R6C2_LONG, RF24G_28_KEY_EVENT_R6C2_HOLD, RF24G_28_KEY_EVENT_R6C2_LOOSE},
    {RF24G_KEY_VAL_R6C3, RF24G_28_KEY_EVENT_R6C3_CLICK, RF24G_28_KEY_EVENT_R6C3_LONG, RF24G_28_KEY_EVENT_R6C3_HOLD, RF24G_28_KEY_EVENT_R6C3_LOOSE},
    {RF24G_KEY_VAL_R6C4, RF24G_28_KEY_EVENT_R6C4_CLICK, RF24G_28_KEY_EVENT_R6C4_LONG, RF24G_28_KEY_EVENT_R6C4_HOLD, RF24G_28_KEY_EVENT_R6C4_LOOSE},

    {RF24G_KEY_VAL_R7C1, RF24G_28_KEY_EVENT_R7C1_CLICK, RF24G_28_KEY_EVENT_R7C1_LONG, RF24G_28_KEY_EVENT_R7C1_HOLD, RF24G_28_KEY_EVENT_R7C1_LOOSE},
    {RF24G_KEY_VAL_R7C2, RF24G_28_KEY_EVENT_R7C2_CLICK, RF24G_28_KEY_EVENT_R7C2_LONG, RF24G_28_KEY_EVENT_R7C2_HOLD, RF24G_28_KEY_EVENT_R7C2_LOOSE},
    {RF24G_KEY_VAL_R7C3, RF24G_28_KEY_EVENT_R7C3_CLICK, RF24G_28_KEY_EVENT_R7C3_LONG, RF24G_28_KEY_EVENT_R7C3_HOLD, RF24G_28_KEY_EVENT_R7C3_LOOSE},
    {RF24G_KEY_VAL_R7C4, RF24G_28_KEY_EVENT_R7C4_CLICK, RF24G_28_KEY_EVENT_R7C4_LONG, RF24G_28_KEY_EVENT_R7C4_HOLD, RF24G_28_KEY_EVENT_R7C4_LOOSE},
};

volatile u8 rf24g_key_driver_event = 0;   // 由key_driver_scan() 更新
volatile u8 rf24g_key_driver_value = 0;   // 由key_driver_scan() 更新
static volatile u8 rf24g_remote_type = 0; // 当前的遥控器类型

rf24g_ins_t rf24g_ins;

volatile u8 rf24g_rx_flag = 0; // 是否收到了新的数据

// u8 last_dynamic_code; // 记录上次的滚码，和当前的滚码对比，实现长按判定

u8 rf24g_get_key_value(void);

struct key_driver_para rf24g_scan_para = {
    .scan_time = RF24G_KEY_SCAN_TIME_MS,                                                     // 按键扫描频率, 单位: ms
    .last_key = NO_KEY,                                                                      // 上一次get_value按键值, 初始化为NO_KEY;
    .filter_time = RF24G_KEY_SCAN_FILTER_TIME_MS,                                            // 按键消抖延时;
    .long_time = RF24G_KEY_LONG_TIME_MS / RF24G_KEY_SCAN_TIME_MS,                            // 按键判定长按数量
    .hold_time = (RF24G_KEY_LONG_TIME_MS + RF24G_KEY_HOLD_TIME_MS) / RF24G_KEY_SCAN_TIME_MS, // 按键判定HOLD数量
    .click_delay_time = RF24G_KEY_SCAN_CLICK_DELAY_TIME_MS,                                  // 20,				//按键被抬起后等待连击延时数量
    .key_type = KEY_DRIVER_TYPE_RF24GKEY,
    .get_value = rf24g_get_key_value,
};

// 底层按键扫描，由__resolve_adv_report()调用
// void rf24g_scan(u8 *pBuf)
// {
//     rf24g_ins_t *p = (rf24g_ins_t *)pBuf;
//     if (p->header1 == HEADER1_1 && p->header2 == HEADER2_1)
//     {
//         memcpy((u8 *)&rf24g_ins, pBuf, sizeof(rf24g_ins_t));
//         // rf24g_ins.key_v = p->key_v;

//         // printf("rf24g_ins.key_v  %u\n", (u16)rf24g_ins.key_v);
//         rf24g_rx_flag = 1;
//     }
// }

void rf24g_scan(u8 *pBuf, u8 len)
{
    if (len >= 6 &&
        pBuf[0] == HEADER1_1 &&
        pBuf[1] == HEADER2_1)
    {
        rf24g_ins.key_v = pBuf[5]; // 获取按键值

        rf24g_remote_type = RF24G_REMOTE_TYPE_17_KEYS;
        rf24g_rx_flag = 1;
    }
    else if (len >= 7 &&
             pBuf[0] == REMOTE_TYPE_28KEY_HEADER_1 &&
             pBuf[1] == REMOTE_TYPE_28KEY_HEADER_2)
    {
        rf24g_ins.key_v = pBuf[2]; // 获取按键值
        rf24g_remote_type = RF24G_REMOTE_TYPE_28_KEYS;
        rf24g_rx_flag = 1;
    }
}

static u8 rf24g_get_key_value(void)
{
    u8 key_value = 0;
    static u8 time_out_cnt = 0; // 加入超时，防止丢包（超时时间与按键扫描时间有关）
    static u8 last_key_value = 0;

    if (rf24g_rx_flag == 1) // 收到2.4G广播
    {
        rf24g_rx_flag = 0;

        // if (rf24g_ins.header1 == HEADER1_1 && rf24g_ins.header2 == HEADER2_1)
        // {
        key_value = rf24g_ins.key_v;

        time_out_cnt = 20;
        last_key_value = key_value;

        rf24g_ins.key_v = NO_KEY;
        return key_value;
        // }
    }

    if (time_out_cnt != 0)
    {
        time_out_cnt--;
        return last_key_value;
    }

    return NO_KEY;
}

u8 rf24g_convert_key_event(u8 key_value, u8 key_driver_event)
{
    // 将key_driver_scan得到的key_event转换成自定义的key_event对应的索引

    u8 key_event_index = 0; // 默认为0，0对应无效索引

    if (RF24G_REMOTE_TYPE_17_KEYS == rf24g_remote_type)
    {
        // 索引对应 rf24g_key_event_table[][] 中的索引
        if (KEY_EVENT_CLICK == key_driver_event)
        {
            key_event_index = 1;
        }
        else if (KEY_EVENT_LONG == key_driver_event || KEY_EVENT_HOLD == key_driver_event)
        {
            // long和hold都当作hold处理
            key_event_index = 2;
        }
        else if (KEY_EVENT_UP == key_driver_event)
        {
            // 长按后松手
            key_event_index = 3;
        }

        if (0 == key_event_index || NO_KEY == key_value)
        {
            // 按键事件与上面的事件都不匹配
            // 得到的键值是无效键值
            return RF24G_KEY_EVENT_NONE;
        }

        for (u8 i = 0; i < sizeof(rf24g_key_event_table) / sizeof(rf24g_key_event_table[0]); i++)
        {
            if (key_value == rf24g_key_event_table[i][0])
            {
                return rf24g_key_event_table[i][key_event_index];
            }
        }
    }
    else if (RF24G_REMOTE_TYPE_28_KEYS == rf24g_remote_type)
    {
        // 索引对应 rf24g_key_type_28keys_table[][] 中的索引
        if (KEY_EVENT_CLICK == key_driver_event)
        {
            key_event_index = 1;
        }
        else if (KEY_EVENT_LONG == key_driver_event)
        {
            key_event_index = 2;
        }
        else if (KEY_EVENT_HOLD == key_driver_event)
        {
            key_event_index = 3;
        }
        else if (KEY_EVENT_UP == key_driver_event)
        {
            // 长按后松手
            key_event_index = 4;
        }

        if (0 == key_event_index || NO_KEY == key_value)
        {
            // 按键事件与上面的事件都不匹配
            // 得到的键值是无效键值
            return RF24G_KEY_EVENT_NONE;
        }

        for (u8 i = 0; i < ARRAY_SIZE(rf24g_key_type_28keys_table); i++)
        {
            if (key_value == rf24g_key_type_28keys_table[i][0])
            {
                return rf24g_key_type_28keys_table[i][key_event_index];
            }
        }
    }

    // 如果运行到这里，都没有找到对应的按键，返回无效按键事件
    return RF24G_KEY_EVENT_NONE;
}

void rf24_key_handle(void)
{
    u8 rf24g_key_event = 0;
    u8 tp_buffer[3]; // 存放要向app发送的数据

    if (NO_KEY == rf24g_key_driver_value)
        return;

    rf24g_key_event = rf24g_convert_key_event(rf24g_key_driver_value, rf24g_key_driver_event);
    rf24g_key_driver_value = NO_KEY;

    if (RF24G_REMOTE_TYPE_17_KEYS == rf24g_remote_type)
    {
        switch (rf24g_key_event)
        {
// 收到短按、长按后松手，再执行对应的功能
#if 1

        case RF24G_KEY_EVENT_ON_OFF_CLICK:
        case RF24G_KEY_EVENT_ON_OFF_LOOSE:
            printf("rf 24g key event on/off\n");

            // printf("save_info.flag_is_light_on = %u\n", (u16)save_info.flag_is_light_on);
            // printf("fc_effect.on_off_flag = %u\n", (u16)fc_effect.on_off_flag);

            printf("fc_effect.Now_state = %u\n", (u16)fc_effect.Now_state);

            if (0 == save_info.flag_is_light_on)
            {
                soft_turn_on_the_light(); // 开灯
            }
            else
            {
                soft_turn_off_lights(); // 关灯
            }

            // save_info_write();      // 保存 save_info 数据
            // save_user_data_area3(); // 保存参数配置到flash

            break;

            /* 恢复出厂设置  ========================================================== */
        case RF24G_KEY_EVENT_DEMO_CLICK:
        case RF24G_KEY_EVENT_DEMO_LOOSE:
            printf("key event demo\n");

            lighting_animation_init();

            // save_info_write();      // 保存 save_info 数据
            // save_user_data_area3(); // 保存参数配置到flash
            break;

            /* 模式加  ========================================================== */
        case RF24G_KEY_EVENT_MODE_ADD_CLICK:
        case RF24G_KEY_EVENT_MODE_ADD_LOOSE:
            printf("key_event_mode_add\n");

            lighting_animation_mode_add();

            // save_info_write();      // 保存 save_info 数据
            // save_user_data_area3(); // 保存参数配置到flash
            break;

            /* 模式减  ========================================================== */
        case RF24G_KEY_EVENT_MODE_SUB_CLICK:
        case RF24G_KEY_EVENT_MODE_SUB_LOOSE:
            printf("key evetn mode sub\n");

            lighting_animation_mode_sub();
            // 向app反馈流星灯速度
            app_feedback_meteor_lights_speed();
            break;

            /* 速度加  ========================================================== */
        case RF24G_KEY_EVENT_SPEED_ADD_CLICK:
        case RF24G_KEY_EVENT_SPEED_ADD_LOOSE:
            printf("key event speed add\n");

            lighting_animation_speed_add();
            // 向app反馈流星灯速度
            app_feedback_meteor_lights_speed();
            break;

            /* 速度减  ========================================================== */
        case RF24G_KEY_EVENT_SPEED_SUB_CLICK:
        case RF24G_KEY_EVENT_SPEED_SUB_LOOSE:
            printf("key event speed sub\n");
            lighting_animation_speed_sub();
            break;

            /* 流水间隔时间变长 */
        case RF24G_KEY_EVENT_COLOR_ADD_CLICK:
        case RF24G_KEY_EVENT_COLOR_ADD_LOOSE:
            printf("key event color add\n");
            lighting_animation_time_interval_add();
            // save_info_write();      // 保存参数配置到flash
            // save_user_data_area3(); // 保存参数配置到flash

            // 向app反馈时间间隔（app的时间间隔是 2~20，这里需要做转换）
            tp_buffer[0] = 0x2F;
            tp_buffer[1] = 0x03;
            /*
                将 4000~15000 转换成 2~20 的数值
                公式：
                newValue = (oldValue - 4000) * (20 - 2) / (15000 - 4000) + 2
            */
            tp_buffer[2] = (u32)(save_info.cur_lighting_animation_time_interval - 4000) * (20 - 2) / (15000 - 4000) + 2;
            zd_fb_2_app(tp_buffer, 3);
            break;

            /* 流水间隔时间变短 */
        case RF24G_KEY_EVENT_COLOR_SUB_CLICK:
        case RF24G_KEY_EVENT_COLOR_SUB_LOOSE:
            printf("key event color sub\n");
            lighting_animation_time_interval_sub();
            // save_info_write();      // 保存参数配置到flash
            // save_user_data_area3(); // 保存参数配置到flash

            // 向app反馈时间间隔（app的时间间隔是 2~20，这里需要做转换）
            tp_buffer[0] = 0x2F;
            tp_buffer[1] = 0x03;
            /*
                将 4000~15000 转换成 2~20 的数值
                公式：
                newValue = (oldValue - 4000) * (20 - 2) / (15000 - 4000) + 2
            */
            tp_buffer[2] = (u32)(save_info.cur_lighting_animation_time_interval - 4000) * (20 - 2) / (15000 - 4000) + 2;
            zd_fb_2_app(tp_buffer, 3);
            break;

            /* 亮度加  ========================================================== */
        case RF24G_KEY_EVENT_BRIGHT_ADD_CLICK:
        case RF24G_KEY_EVENT_BRIGHT_ADD_LOOSE:
            printf("key event bright add\n");
            lighting_animation_bright_add();

            // save_info_write();      // 保存参数配置到flash
            // save_user_data_area3(); // 保存参数配置到flash

            // 向app反馈亮度（app的亮度是 0~100，这里需要做转换）
            fc_effect.app_b = (u32)fc_effect.b * 100 / 255;
            fb_bright(); // 向app返回亮度数据

            break;

            /* 流水方向切换  */
        case RF24G_KEY_EVENT_BRIGHT_SUB_CLICK:
        // case RF24G_KEY_EVENT_BRIGHT_SUB_HOLD: // 加上hold会影响动画效果
        case RF24G_KEY_EVENT_BRIGHT_SUB_LOOSE:
            printf("key event bright sub\n");

            lighting_animation_dir_switch();
            // save_info_write();      // 保存参数配置到flash
            // save_user_data_area3(); // 保存参数配置到flash

            break;

            /* 流水间隔时间设置为最短 */
        case RF24G_KEY_EVENT_B_CLICK:
        case RF24G_KEY_EVENT_B_LOOSE:
            printf("key event b\n");
            lighting_animation_time_interval_fast(); // 时间间隔设置为最快
            // save_info_write();                       // 保存参数配置到flash
            // save_user_data_area3();                  // 保存参数配置到flash

            // 向app反馈时间间隔（app的时间间隔是 2~20，这里需要做转换）
            tp_buffer[0] = 0x2F;
            tp_buffer[1] = 0x03;
            /*
                将 4000~15000 转换成 2~20 的数值
                公式：
                newValue = (oldValue - 4000) * (20 - 2) / (15000 - 4000) + 2
            */
            tp_buffer[2] = (u32)(save_info.cur_lighting_animation_time_interval - 4000) * (20 - 2) / (15000 - 4000) + 2;
            zd_fb_2_app(tp_buffer, 3);

            break;

            /* 流水间隔时间设置为最长  */
        case RF24G_KEY_EVENT_G_CLICK:
        case RF24G_KEY_EVENT_G_LOOSE:
            printf("key event G\n");
            lighting_animation_time_interval_slow(); // 时间间隔设置为最慢
            // save_info_write();                       // 保存参数配置到flash
            // save_user_data_area3();                  // 保存参数配置到flash

            // 向app反馈时间间隔（app的时间间隔是 2~20，这里需要做转换）
            tp_buffer[0] = 0x2F;
            tp_buffer[1] = 0x03;
            /*
                将 4000~15000 转换成 2~20 的数值
                公式：
                newValue = (oldValue - 4000) * (20 - 2) / (15000 - 4000) + 2
            */
            tp_buffer[2] = (u32)(save_info.cur_lighting_animation_time_interval - 4000) * (20 - 2) / (15000 - 4000) + 2;
            zd_fb_2_app(tp_buffer, 3);
            break;

            /* 6种流水间隔时间固定 */
        case RF24G_KEY_EVENT_F_CLICK:
        case RF24G_KEY_EVENT_F_LOOSE:
            printf("key event F\n");
            lighting_animation_time_interval_mid(); // 时间间隔设置为适中
            // save_info_write();                      // 保存参数配置到flash
            // save_user_data_area3();                 // 保存参数配置到flash

            // 向app反馈时间间隔（app的时间间隔是 2~20，这里需要做转换）
            tp_buffer[0] = 0x2F;
            tp_buffer[1] = 0x03;
            /*
                将 4000~15000 转换成 2~20 的数值
                公式：
                newValue = (oldValue - 4000) * (20 - 2) / (15000 - 4000) + 2
            */
            tp_buffer[2] = (u32)(save_info.cur_lighting_animation_time_interval - 4000) * (20 - 2) / (15000 - 4000) + 2;
            zd_fb_2_app(tp_buffer, 3);
            break;

#endif
        default:
        {
            // 不是有效的键值，直接退出
            return;
        }
        break;
        }
    }
    else if (RF24G_REMOTE_TYPE_28_KEYS == rf24g_remote_type)
    {
        // printf("28keys key event %u\n", (u16)rf24g_key_event);
        rf24_key_handle_func_t rf24g_key_handle_func_ptr = rf24_28keys_handle_func_buff[rf24g_key_event];

        if (0 == save_info.flag_is_light_on)
        {
            // 如果设备没有启动，只对开关按键做处理
            if (rf24g_key_event != RF24G_28_KEY_EVENT_R1C4_CLICK &&
                rf24g_key_event != RF24G_28_KEY_EVENT_R1C4_LONG)
            {
                return;
            }

            if (NULL == rf24g_key_handle_func_ptr)
            {
                // 如果开关按键没有填写对应的处理函数，直接退出
                return;
            }

            rf24g_key_handle_func_ptr();
            os_taskq_post("msg_task", 1, MSG_USER_SAVE_INFO);
            return;
        }

        if (NULL == rf24g_key_handle_func_ptr)
        {
            return;
        }

        rf24g_key_handle_func_ptr();
    }

    os_taskq_post("msg_task", 1, MSG_USER_SAVE_INFO);
}

void rf24g_28keys_event_r1c1_click_handle(void)
{
    printf("28keys event r1c1\n");

    // 速度加
    lighting_animation_speed_add();
    // 向app反馈流星灯速度
    app_feedback_meteor_lights_speed();
}

void rf24g_28keys_event_r1c2_click_handle(void)
{
    printf("28keys event r1c2\n");

    // 速度减
    lighting_animation_speed_sub();
    // 向app反馈流星灯速度
    app_feedback_meteor_lights_speed();
}

void rf24g_28keys_event_r1c3_click_handle(void)
{
    printf("28keys event r1c3\n");

    // 关灯
    soft_turn_off_lights();
}

void rf24g_28keys_event_r1c4_click_handle(void)
{
    printf("28keys event r1c4\n");

    // 开灯
    soft_turn_on_the_light();
}

void rf24g_28keys_event_r3c1_click_handle(void)
{
    printf("28keys event r3c1\n");

    // 模式 加
    lighting_animation_mode_add();
}

void rf24g_28keys_event_r3c2_click_handle(void)
{
    printf("28keys event r3c2\n");

    // 模式 减
    lighting_animation_mode_sub();
}

void rf24g_28keys_event_r3c3_click_handle(void)
{
    printf("28keys event r3c3\n");

    // 亮度加
    lighting_animation_bright_add();
    app_feedback_meteor_lights_brightness();
}

void rf24g_28keys_event_r3c4_click_handle(void)
{
    printf("28keys event r3c4\n");

    // 亮度减
    lighting_animation_bright_sub();
    app_feedback_meteor_lights_brightness();
}

void rf24g_28keys_event_r4c1_click_handle(void)
{
    printf("28keys event r4c1\n");

    // 切换成 模式 1
    lighting_animation_mode_setting(1);
}

void rf24g_28keys_event_r4c2_click_handle(void)
{
    printf("28keys event r4c2\n");

    // 切换成 模式 2
    lighting_animation_mode_setting(2);
}

void rf24g_28keys_event_r4c3_click_handle(void)
{
    printf("28keys event r4c3\n");

    // 切换成 模式 3
    lighting_animation_mode_setting(3);
}

void rf24g_28keys_event_r4c4_click_handle(void)
{
    printf("28keys event r4c4\n");

    // 切换成 模式 4
    lighting_animation_mode_setting(4);
}

void rf24g_28keys_event_r5c1_click_handle(void)
{
    printf("28keys event r5c1\n");

    // 模式 5
    lighting_animation_mode_setting(5);
}

void rf24g_28keys_event_r5c2_click_handle(void)
{
    printf("28keys event r5c2\n");

    // 模式 6
    lighting_animation_mode_setting(6);
}

void rf24g_28keys_event_r5c3_click_handle(void)
{
    printf("28keys event r5c3\n");

    // 模式 7
    lighting_animation_mode_setting(7);
}

void rf24g_28keys_event_r5c4_click_handle(void)
{
    printf("28keys event r5c4\n");

    // 模式 8
    lighting_animation_mode_setting(8);
}

void rf24g_28keys_event_r6c1_click_handle(void)
{
    printf("28keys event r6c1\n");

    // 模式 9
    lighting_animation_mode_setting(9);
}

void rf24g_28keys_event_r6c2_click_handle(void)
{
    printf("28keys event r6c2\n");

    // 模式 10
    lighting_animation_mode_setting(10);
}

void rf24g_28keys_event_r6c3_click_handle(void)
{
    printf("28keys event r6c3\n");

    // 模式 11
    lighting_animation_mode_setting(11);
}

void rf24g_28keys_event_r6c4_click_handle(void)
{
    printf("28keys event r6c4\n");

    // 模式 12
    lighting_animation_mode_setting(12);
}

void rf24g_28keys_event_r7c1_click_handle(void)
{
    printf("28keys event r7c1\n");

    // 模式 13
    lighting_animation_mode_setting(13);
}

void rf24g_28keys_event_r7c2_click_handle(void)
{
    printf("28keys event r7c2\n");

    // 模式 14
    lighting_animation_mode_setting(14);
}

void rf24g_28keys_event_r7c3_click_handle(void)
{
    printf("28keys event r7c3\n");

    // 模式 15
    lighting_animation_mode_setting(15);
}

void rf24g_28keys_event_r7c4_click_handle(void)
{
    printf("28keys event r7c4\n");

    // 模式 16
    lighting_animation_mode_setting(16);
}

const rf24_key_handle_func_t rf24_28keys_handle_func_buff[RF24G_28_KEY_EVENT_MAX] = {
    [RF24G_28_KEY_EVENT_R1C1_CLICK] = rf24g_28keys_event_r1c1_click_handle, // 按键事件处理函数
    [RF24G_28_KEY_EVENT_R1C1_LONG] = rf24g_28keys_event_r1c1_click_handle,

    [RF24G_28_KEY_EVENT_R1C2_CLICK] = rf24g_28keys_event_r1c2_click_handle,
    [RF24G_28_KEY_EVENT_R1C2_LONG] = rf24g_28keys_event_r1c2_click_handle,

    [RF24G_28_KEY_EVENT_R1C3_CLICK] = rf24g_28keys_event_r1c3_click_handle,
    [RF24G_28_KEY_EVENT_R1C3_LONG] = rf24g_28keys_event_r1c3_click_handle,

    [RF24G_28_KEY_EVENT_R1C4_CLICK] = rf24g_28keys_event_r1c4_click_handle,
    [RF24G_28_KEY_EVENT_R1C4_LONG] = rf24g_28keys_event_r1c4_click_handle,

    [RF24G_28_KEY_EVENT_R2C1_CLICK] = NULL,

    [RF24G_28_KEY_EVENT_R3C1_CLICK] = rf24g_28keys_event_r3c1_click_handle,
    [RF24G_28_KEY_EVENT_R3C1_LONG] = rf24g_28keys_event_r3c1_click_handle,

    [RF24G_28_KEY_EVENT_R3C2_CLICK] = rf24g_28keys_event_r3c2_click_handle,
    [RF24G_28_KEY_EVENT_R3C2_LONG] = rf24g_28keys_event_r3c2_click_handle,

    [RF24G_28_KEY_EVENT_R3C3_CLICK] = rf24g_28keys_event_r3c3_click_handle,
    [RF24G_28_KEY_EVENT_R3C3_LONG] = rf24g_28keys_event_r3c3_click_handle,

    [RF24G_28_KEY_EVENT_R3C4_CLICK] = rf24g_28keys_event_r3c4_click_handle,
    [RF24G_28_KEY_EVENT_R3C4_LONG] = rf24g_28keys_event_r3c4_click_handle,
    // =======================================================================
    [RF24G_28_KEY_EVENT_R4C1_CLICK] = rf24g_28keys_event_r4c1_click_handle,
    [RF24G_28_KEY_EVENT_R4C1_LONG] = rf24g_28keys_event_r4c1_click_handle,

    [RF24G_28_KEY_EVENT_R4C2_CLICK] = rf24g_28keys_event_r4c2_click_handle,
    [RF24G_28_KEY_EVENT_R4C2_LONG] = rf24g_28keys_event_r4c2_click_handle,

    [RF24G_28_KEY_EVENT_R4C3_CLICK] = rf24g_28keys_event_r4c3_click_handle,
    [RF24G_28_KEY_EVENT_R4C3_LONG] = rf24g_28keys_event_r4c3_click_handle,

    [RF24G_28_KEY_EVENT_R4C4_CLICK] = rf24g_28keys_event_r4c4_click_handle,
    [RF24G_28_KEY_EVENT_R4C4_LONG] = rf24g_28keys_event_r4c4_click_handle,
    // =======================================================================
    [RF24G_28_KEY_EVENT_R5C1_CLICK] = rf24g_28keys_event_r5c1_click_handle,
    [RF24G_28_KEY_EVENT_R5C1_LONG] = rf24g_28keys_event_r5c1_click_handle,

    [RF24G_28_KEY_EVENT_R5C2_CLICK] = rf24g_28keys_event_r5c2_click_handle,
    [RF24G_28_KEY_EVENT_R5C2_LONG] = rf24g_28keys_event_r5c2_click_handle,

    [RF24G_28_KEY_EVENT_R5C3_CLICK] = rf24g_28keys_event_r5c3_click_handle,
    [RF24G_28_KEY_EVENT_R5C3_LONG] = rf24g_28keys_event_r5c3_click_handle,

    [RF24G_28_KEY_EVENT_R5C4_CLICK] = rf24g_28keys_event_r5c4_click_handle,
    [RF24G_28_KEY_EVENT_R5C4_LONG] = rf24g_28keys_event_r5c4_click_handle,
    // =======================================================================
    [RF24G_28_KEY_EVENT_R6C1_CLICK] = rf24g_28keys_event_r6c1_click_handle,
    [RF24G_28_KEY_EVENT_R6C1_LONG] = rf24g_28keys_event_r6c1_click_handle,

    [RF24G_28_KEY_EVENT_R6C2_CLICK] = rf24g_28keys_event_r6c2_click_handle,
    [RF24G_28_KEY_EVENT_R6C2_LONG] = rf24g_28keys_event_r6c2_click_handle,

    [RF24G_28_KEY_EVENT_R6C3_CLICK] = rf24g_28keys_event_r6c3_click_handle,
    [RF24G_28_KEY_EVENT_R6C3_LONG] = rf24g_28keys_event_r6c3_click_handle,

    [RF24G_28_KEY_EVENT_R6C4_CLICK] = rf24g_28keys_event_r6c4_click_handle,
    [RF24G_28_KEY_EVENT_R6C4_LONG] = rf24g_28keys_event_r6c4_click_handle,
    // =======================================================================
    [RF24G_28_KEY_EVENT_R7C1_CLICK] = rf24g_28keys_event_r7c1_click_handle,
    [RF24G_28_KEY_EVENT_R7C1_LONG] = rf24g_28keys_event_r7c1_click_handle,

    [RF24G_28_KEY_EVENT_R7C2_CLICK] = rf24g_28keys_event_r7c2_click_handle,
    [RF24G_28_KEY_EVENT_R7C2_LONG] = rf24g_28keys_event_r7c2_click_handle,

    [RF24G_28_KEY_EVENT_R7C3_CLICK] = rf24g_28keys_event_r7c3_click_handle,
    [RF24G_28_KEY_EVENT_R7C3_LONG] = rf24g_28keys_event_r7c3_click_handle,

    [RF24G_28_KEY_EVENT_R7C4_CLICK] = rf24g_28keys_event_r7c4_click_handle,
    [RF24G_28_KEY_EVENT_R7C4_LONG] = rf24g_28keys_event_r7c4_click_handle,

};

#endif // #if (TCFG_RF24GKEY_ENABLE)
