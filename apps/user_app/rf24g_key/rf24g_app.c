
#include "system/includes.h"
#include "task.h"
#include "event.h"
#include "btstack/btstack_typedef.h"
#include "app_config.h"
#include "rf24g_app.h"
#include "WS2812FX.H"
#include "led_strip_sys.h"

#include "../../../apps/user_app/lighting_animation/lighting_animation.h" // 灯光动画
#include "../../../apps/user_app/led_strip/led_strand_effect.h"           // fc_effect 变量定义

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

volatile u8 rf24g_key_driver_event = 0; // 由key_driver_scan() 更新
volatile u8 rf24g_key_driver_value = 0; // 由key_driver_scan() 更新

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
void rf24g_scan(u8 *pBuf)
{
    rf24g_ins_t *p = (rf24g_ins_t *)pBuf;
    if (p->header1 == HEADER1_1 && p->header2 == HEADER2_1)
    {
        memcpy((u8 *)&rf24g_ins, pBuf, sizeof(rf24g_ins_t));
        // rf24g_ins.key_v = p->key_v;

        // printf("rf24g_ins.key_v  %u\n", (u16)rf24g_ins.key_v);
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

        if (rf24g_ins.header1 == HEADER1_1 && rf24g_ins.header2 == HEADER2_1)
        {
            key_value = rf24g_ins.key_v;

            time_out_cnt = 20;
            last_key_value = key_value;

            rf24g_ins.key_v = NO_KEY;
            return key_value;
        }
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
    // 索引对应 rf24g_key_event_table[][] 中的索引
    u8 key_event_index = 0; // 默认为0，0对应无效索引
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

    // 如果运行到这里，都没有找到对应的按键，返回无效按键事件
    return RF24G_KEY_EVENT_NONE;
}

void rf24_key_handle(void)
{
    u8 rf24g_key_event = 0;

    if (NO_KEY == rf24g_key_driver_value)
        return;

    rf24g_key_event = rf24g_convert_key_event(rf24g_key_driver_value, rf24g_key_driver_event);
    rf24g_key_driver_value = NO_KEY;

    switch (rf24g_key_event)
    {
// 收到短按、长按后松手，再执行对应的功能
#if 0

    case RF24G_KEY_EVENT_ON_OFF_CLICK:
    case RF24G_KEY_EVENT_ON_OFF_LOOSE:
        printf("key event on/off\n");
        // 开机/关机
        if (fc_effect.on_off_flag == DEVICE_OFF)
        {
            fc_effect.on_off_flag = DEVICE_ON;
            lighting_animation_mode_change();
            // fb_led_on_off_state();  // 与app同步开关状态
            printf("soft_turn_on_the_light");
        }
        else
        {
            // 实际上只是关灯，没有低功耗
            fc_effect.on_off_flag = DEVICE_OFF;
            WS2812FX_stop();
            // fb_led_on_off_state();  // 与app同步开关状态
            printf("soft_turn_off_lights");
        }

          save_user_data_area3(); // 保存参数配置到flash

        break;

        /* 恢复出厂设置  ========================================================== */
    case RF24G_KEY_EVENT_DEMO_CLICK:
    case RF24G_KEY_EVENT_DEMO_LOOSE:
        printf("key event demo\n");
        lighting_animation_init();

        save_user_data_area3(); // 保存参数配置到flash
        break;

        /* 模式加  ========================================================== */
    case RF24G_KEY_EVENT_MODE_ADD_CLICK:
    case RF24G_KEY_EVENT_MODE_ADD_LOOSE:
        printf("key_event_mode_add\n");
        lighting_animation_mode_add();
        save_user_data_area3(); // 保存参数配置到flash
        break;

        /* 模式减  ========================================================== */
    case RF24G_KEY_EVENT_MODE_SUB_CLICK:
    case RF24G_KEY_EVENT_MODE_SUB_LOOSE:
        printf("key evetn mode sub\n");
        lighting_animation_mode_sub();
        save_user_data_area3(); // 保存参数配置到flash
        break;

        /* 速度加  ========================================================== */
    case RF24G_KEY_EVENT_SPEED_ADD_CLICK:
    case RF24G_KEY_EVENT_SPEED_ADD_LOOSE:
        printf("key event speed add\n");
        lighting_animation_speed_add();
        save_user_data_area3(); // 保存参数配置到flash
        break;

        /* 速度减  ========================================================== */
    case RF24G_KEY_EVENT_SPEED_SUB_CLICK:
    case RF24G_KEY_EVENT_SPEED_SUB_LOOSE:
        printf("key event speed sub\n");
        lighting_animation_speed_sub();
        save_user_data_area3(); // 保存参数配置到flash
        break;

        /* 流水间隔时间变长（待确认）  ========================================================== */
    case RF24G_KEY_EVENT_COLOR_ADD_CLICK:
    case RF24G_KEY_EVENT_COLOR_ADD_LOOSE:
        printf("key event color add\n");
        lighting_animation_speed_sub();
        save_user_data_area3(); // 保存参数配置到flash
        break;

        /* 流水间隔时间变短（待确认）  ========================================================== */
    case RF24G_KEY_EVENT_COLOR_SUB_CLICK:
    case RF24G_KEY_EVENT_COLOR_SUB_LOOSE:
        printf("key event color sub\n");
        lighting_animation_speed_add();
        save_user_data_area3(); // 保存参数配置到flash
        break;

        /* 亮度加  ========================================================== */
    case RF24G_KEY_EVENT_BRIGHT_ADD_CLICK:
    case RF24G_KEY_EVENT_BRIGHT_ADD_LOOSE:
        printf("key event bright add\n");
        lighting_animation_bright_add();
        save_user_data_area3(); // 保存参数配置到flash
        break;

        /* 流水方向切换（待确认）  ========================================================== */
    case RF24G_KEY_EVENT_BRIGHT_SUB_CLICK:
    // case RF24G_KEY_EVENT_BRIGHT_SUB_HOLD: // 加上hold会影响动画效果
    case RF24G_KEY_EVENT_BRIGHT_SUB_LOOSE:
        printf("key event bright sub\n");
        lighting_animation_dir_switch();
        save_user_data_area3(); // 保存参数配置到flash
        break;

        /* 流水间隔时间设置为最短（待确认）  ========================================================== */
    case RF24G_KEY_EVENT_B_CLICK:
    case RF24G_KEY_EVENT_B_LOOSE:
        printf("key event b\n");
        lighting_animation_speed_max();
        save_user_data_area3(); // 保存参数配置到flash
        break;

        /* 流水间隔时间设置为最长（待确认）  ========================================================== */
    case RF24G_KEY_EVENT_G_CLICK:
    case RF24G_KEY_EVENT_G_LOOSE:
        printf("key event G\n");
        lighting_animation_speed_min();
        save_user_data_area3(); // 保存参数配置到flash
        break;

        /* 6中流水间隔时间固定（待确认）  ========================================================== */
    case RF24G_KEY_EVENT_F_CLICK:
    case RF24G_KEY_EVENT_F_LOOSE:
        printf("key event F\n");
        lighting_animation_speed_mid();
        save_user_data_area3(); // 保存参数配置到flash
        break;
#endif

// 测试2.4G遥控器接收：
#if 0
    case RF24G_KEY_EVENT_ON_OFF_CLICK:
        printf("key event on/off click\n");
        break;
    case RF24G_KEY_EVENT_ON_OFF_HOLD:
        printf("key event on/off hold\n");
        break;

    case RF24G_KEY_EVENT_ON_OFF_LOOSE:
        printf("key event on/off loose\n");
        break;

    case RF24G_KEY_EVENT_DEMO_CLICK:
        printf("key event demo click\n");
        break;
    case RF24G_KEY_EVENT_DEMO_HOLD:
        printf("key event demo hold\n");
        break;
    case RF24G_KEY_EVENT_DEMO_LOOSE:
        printf("key event demo loose\n");
        break;

    case RF24G_KEY_EVENT_MODE_ADD_CLICK:
        printf("key event mode add click\n");
        break;
    case RF24G_KEY_EVENT_MODE_ADD_HOLD:
        printf("key event mode add hold\n");
        break;
    case RF24G_KEY_EVENT_MODE_ADD_LOOSE:
        printf("key event mode add loose\n");
        break;

    case RF24G_KEY_EVENT_MODE_SUB_CLICK:
        printf("key event mode sub click\n");
        break;
    case RF24G_KEY_EVENT_MODE_SUB_HOLD:
        printf("key event mode sub hold\n");
        break;
    case RF24G_KEY_EVENT_MODE_SUB_LOOSE:
        printf("key evetn mode sub\n");
        break;

    case RF24G_KEY_EVENT_SPEED_ADD_CLICK:
        printf("key event speed add click\n");
        break;
    case RF24G_KEY_EVENT_SPEED_ADD_HOLD:
        printf("key event speed add hold\n");
        break;
    case RF24G_KEY_EVENT_SPEED_ADD_LOOSE:
        printf("key event speed add\n");
        break;

    case RF24G_KEY_EVENT_SPEED_SUB_CLICK:
    case RF24G_KEY_EVENT_SPEED_SUB_LOOSE:
        printf("key event speed sub\n");
        break;

    case RF24G_KEY_EVENT_COLOR_ADD_CLICK:
    case RF24G_KEY_EVENT_COLOR_ADD_LOOSE:
        printf("key event color add\n");
        break;

    case RF24G_KEY_EVENT_COLOR_SUB_CLICK:
    case RF24G_KEY_EVENT_COLOR_SUB_LOOSE:
        printf("key event color sub\n");
        break;

    case RF24G_KEY_EVENT_BRIGHT_ADD_CLICK:
    case RF24G_KEY_EVENT_BRIGHT_ADD_LOOSE:
        printf("key event bright add\n");
        break;

    case RF24G_KEY_EVENT_BRIGHT_SUB_CLICK:
    case RF24G_KEY_EVENT_BRIGHT_SUB_LOOSE:
        printf("key event bright sub\n");
        break;

    case RF24G_KEY_EVENT_B_CLICK:
    case RF24G_KEY_EVENT_B_LOOSE:
        printf("key event b\n");
        break;

    case RF24G_KEY_EVENT_G_CLICK:
    case RF24G_KEY_EVENT_G_LOOSE:
        printf("key event G\n");
        break;

    case RF24G_KEY_EVENT_F_CLICK:
    case RF24G_KEY_EVENT_F_LOOSE:
        printf("key event F\n");
        break;
#endif
    } 
}

#endif // #if (TCFG_RF24GKEY_ENABLE)
