/*********************************************************************************************
    *   Filename        : app_main.c

    *   Description     :

    *   Copyright:(c)JIELI  2011-2019  @ , All Rights Reserved.
*********************************************************************************************/
#include "system/includes.h"
#include "app_config.h"
#include "app_action.h"
#include "app_main.h"
#include "update.h"
#include "update_loader_download.h"
#include "app_charge.h"
#include "app_power_manage.h"
#include "asm/charge.h"

#if TCFG_KWS_VOICE_RECOGNITION_ENABLE
#include "jl_kws/jl_kws_api.h"
#endif /* #if TCFG_KWS_VOICE_RECOGNITION_ENABLE */

#define LOG_TAG_CONST APP
#define LOG_TAG "[APP]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"
#include "rf433_app.h"
#include "led_strip_drive.h"
#include "hardware.h"

#include "../../apps/user_app/led_strip/led_strand_effect.h"
#include "../../apps/user_app/ws2812-fx-lib/WS2812FX_C/ws2812fx_effect.h"
#include "../../apps/user_app/lighting_animation/lighting_animation.h"

OS_SEM LED_TASK_SEM;

/*任务列表   */
const struct task_info task_info_table[] = {
#if CONFIG_APP_FINDMY
    {"app_core", 1, 0, 640 * 2, 128},
#else
    {"app_core", 1, 0, 640, 128},
#endif

    {"sys_event", 7, 0, 256, 0},
    {"btctrler", 4, 0, 512, 256},
    {"btencry", 1, 0, 512, 128},
    {"btstack", 3, 0, 768, 256},
    {"systimer", 7, 0, 128, 0},
    {"update", 1, 0, 512, 0},
    {"dw_update", 2, 0, 256, 128},
#if (RCSP_BTMATE_EN)
    {"rcsp_task", 2, 0, 640, 0},
#endif
#if (USER_UART_UPDATE_ENABLE)
    {"uart_update", 1, 0, 256, 128},
#endif
#if (XM_MMA_EN)
    {"xm_mma", 2, 0, 640, 256},
#endif
    {"usb_msd", 1, 0, 512, 128},
#if TCFG_AUDIO_ENABLE
    {"audio_dec", 3, 0, 768, 128},
    {"audio_enc", 4, 0, 512, 128},
#endif /*TCFG_AUDIO_ENABLE*/
#if TCFG_KWS_VOICE_RECOGNITION_ENABLE
    {"kws", 2, 0, 256, 64},
#endif /* #if TCFG_KWS_VOICE_RECOGNITION_ENABLE */
#if (TUYA_DEMO_EN)
    {"user_deal", 2, 0, 512, 512}, // 定义线程 tuya任务调度
#endif
#if (CONFIG_APP_HILINK)
    {"hilink_task", 2, 0, 1024, 0}, // 定义线程 hilink任务调度
#endif

    {"led_task", 2, 0, 512, 512}, // 灯光

    {"test_task", 2, 0, 512, 512}, // 定义线程 main 任务调度
    {0, 0},
};

APP_VAR app_var;

void app_var_init(void)
{
    app_var.play_poweron_tone = 1;

    app_var.auto_off_time = TCFG_AUTO_SHUT_DOWN_TIME;
    app_var.warning_tone_v = 340;
    app_var.poweroff_tone_v = 330;
}

__attribute__((weak))
u8
get_charge_online_flag(void)
{
    return 0;
}

void clr_wdt(void);
void check_power_on_key(void)
{
#if TCFG_POWER_ON_NEED_KEY

    u32 delay_10ms_cnt = 0;
    while (1)
    {
        clr_wdt();
        os_time_dly(1);

        extern u8 get_power_on_status(void);
        if (get_power_on_status())
        {
            log_info("+");
            delay_10ms_cnt++;
            if (delay_10ms_cnt > 70)
            {
                /* extern void set_key_poweron_flag(u8 flag); */
                /* set_key_poweron_flag(1); */
                return;
            }
        }
        else
        {
            log_info("-");
            delay_10ms_cnt = 0;
            log_info("enter softpoweroff\n");
            power_set_soft_poweroff();
        }
    }
#endif
}

void app_main()
{
    struct intent it;

    if (!UPDATE_SUPPORT_DEV_IS_NULL())
    {
        int update = 0;
        update = update_result_deal();
    }

    printf(">>>>>>>>>>>>>>>>>app_main...\n");
    printf(">>> v220,2022-11-23 >>>\n");

    if (get_charge_online_flag())
    {
#if (TCFG_SYS_LVD_EN == 1)
        vbat_check_init();
#endif
    }
    else
    {
        check_power_on_voltage();
    }

#if TCFG_POWER_ON_NEED_KEY
    check_power_on_key();
#endif

#if TCFG_AUDIO_ENABLE
    extern int audio_dec_init();
    extern int audio_enc_init();
    audio_dec_init();
    audio_enc_init();
#endif /*TCFG_AUDIO_ENABLE*/

#if TCFG_KWS_VOICE_RECOGNITION_ENABLE
    jl_kws_main_user_demo();
#endif /* #if TCFG_KWS_VOICE_RECOGNITION_ENABLE */

    init_intent(&it);

#if CONFIG_APP_SPP_LE
    it.name = "spp_le";
    it.action = ACTION_SPPLE_MAIN;

#elif CONFIG_APP_AT_COM || CONFIG_APP_AT_CHAR_COM
    it.name = "at_com";
    it.action = ACTION_AT_COM;

#elif CONFIG_APP_DONGLE
    it.name = "dongle";
    it.action = ACTION_DONGLE_MAIN;

#elif CONFIG_APP_MULTI
    it.name = "multi_conn";
    it.action = ACTION_MULTI_MAIN;

#elif CONFIG_APP_NONCONN_24G
    it.name = "nonconn_24g";
    it.action = ACTION_NOCONN_24G_MAIN;

#elif CONFIG_APP_HILINK
    it.name = "hilink";
    it.action = ACTION_HILINK_MAIN;

#elif CONFIG_APP_LL_SYNC
    it.name = "ll_sync";
    it.action = ACTION_LL_SYNC;

#elif CONFIG_APP_TUYA
    it.name = "tuya";
    it.action = ACTION_TUYA;

#elif CONFIG_APP_CENTRAL
    it.name = "central";
    it.action = ACTION_CENTRAL_MAIN;

#elif CONFIG_APP_DONGLE
    it.name = "dongle";
    it.action = ACTION_DONGLE_MAIN;

#elif CONFIG_APP_BEACON
    it.name = "beacon";
    it.action = ACTION_BEACON_MAIN;

#elif CONFIG_APP_IDLE
    it.name = "idle";
    it.action = ACTION_IDLE_MAIN;

#elif CONFIG_APP_CONN_24G
    it.name = "conn_24g";
    it.action = ACTION_CONN_24G_MAIN;

#elif CONFIG_APP_FINDMY
    it.name = "findmy";
    it.action = ACTION_FINDMY;

#else
    while (1)
    {
        printf("no app!!!");
    }
#endif

    log_info("run app>>> %s", it.name);
    log_info("%s,%s", __DATE__, __TIME__);

    start_app(&it);

#if TCFG_CHARGE_ENABLE
    set_charge_event_flag(1);
#endif
}

/*
 * app模式切换
 */
void app_switch(const char *name, int action)
{
    struct intent it;
    struct application *app;

    log_info("app_exit\n");

    init_intent(&it);
    app = get_current_app();
    if (app)
    {
        /*
         * 退出当前app, 会执行state_machine()函数中APP_STA_STOP 和 APP_STA_DESTORY
         */
        it.name = app->name;
        it.action = ACTION_BACK;
        start_app(&it);
    }

    /*
     * 切换到app (name)并执行action分支
     */
    it.name = name;
    it.action = action;
    start_app(&it);
}

int eSystemConfirmStopStatus(void)
{
    /* 系统进入在未来时间里，无任务超时唤醒，可根据用户选择系统停止，或者系统定时唤醒(100ms) */
    // 1:Endless Sleep
    // 0:100 ms wakeup
    /* log_info("100ms wakeup"); */
    return 1;
}

__attribute__((used)) int *__errno()
{
    static int err;
    return &err;
}

#if 1

// --------------------------------------------------------------------------定时器
static const u16 timer_div[] = {
    /*0000*/ 1,
    /*0001*/ 4,
    /*0010*/ 16,
    /*0011*/ 64,
    /*0100*/ 2,
    /*0101*/ 8,
    /*0110*/ 32,
    /*0111*/ 128,
    /*1000*/ 256,
    /*1001*/ 4 * 256,
    /*1010*/ 16 * 256,
    /*1011*/ 64 * 256,
    /*1100*/ 2 * 256,
    /*1101*/ 8 * 256,
    /*1110*/ 32 * 256,
    /*1111*/ 128 * 256,
};
#define APP_TIMER_CLK (CONFIG_BT_NORMAL_HZ / 2) // clk_get("timer")
#define MAX_TIME_CNT 0x7fff
#define MIN_TIME_CNT 0x100
#define TIMER_UNIT 1

#define TIMER_CON JL_TIMER2->CON
#define TIMER_CNT JL_TIMER2->CNT
#define TIMER_PRD JL_TIMER2->PRD
#define TIMER_VETOR IRQ_TIME2_IDX

___interrupt
    AT_VOLATILE_RAM_CODE void
    user_timer_isr(void) // 125us
{

    TIMER_CON |= BIT(14);

    extern void one_wire_send(void);
    one_wire_send(); // steomotor

#if TCFG_RF433GKEY_ENABLE
    extern void timer125us_hook(void);
    timer125us_hook();
#endif
}

void user_timer_init(void)
{
    u32 prd_cnt;
    u8 index;

    //	printf("********* user_timer_init **********\n");
    for (index = 0; index < (sizeof(timer_div) / sizeof(timer_div[0])); index++)
    {
        prd_cnt = TIMER_UNIT * (APP_TIMER_CLK / 8000) / timer_div[index]; // 8000==125us
        if (prd_cnt > MIN_TIME_CNT && prd_cnt < MAX_TIME_CNT)
        {
            break;
        }
    }

    TIMER_CNT = 0;
    TIMER_PRD = prd_cnt;
    request_irq(TIMER_VETOR, 0, user_timer_isr, 0);
    TIMER_CON = (index << 4) | BIT(0) | BIT(3);
}
__initcall(user_timer_init);

#endif
extern void count_down_run(void);
extern void time_clock_handler(void);
extern void rf433_handle(void);
extern void meteor_period_sub(void);

void main_while(void)
{

    // while(1)
    // {

    // =================================
    //           默认内容，不用修改
    // =================================
    sound_handle();
    run_tick_per_10ms();
    WS2812FX_service();

    // os_time_dly(1);
    // }
}

extern fc_effect_t fc_effect; // 幻彩灯串效果数据
extern const u8 size_type[4];
extern uint16_t SM_mode_comet_1(void);

// u32 color_buff[] = {RED, GREEN, BLUE};
void test_task(void)
{
    // os_time_dly(100);
    fc_effect.on_off_flag == DEVICE_ON;

#if 0 // 流星灯动画
    // fc_effect.star_speed = 180; // 变化速度(0~65535，值越小，速度越快) 动画时间约 4 s
    // fc_effect.star_speed = 135;// 变化速度(0~65535，值越小，速度越快) 动画时间约 3 s
    fc_effect.star_speed = 90; // 变化速度(0~65535，值越小，速度越快) 动画时间约 2 s
    // fc_effect.star_speed = 45; // 变化速度(0~65535，值越小，速度越快) 动画时间约 1 s

 

    // fc_effect.meteor_period = 4 + 1; // 时间周期，包括执行动画的时间
    fc_effect.meteor_period = 8 + 2; // 时间周期，包括执行动画的时间
    // fc_effect.meteor_period = 10 + 3; // 时间周期，包括执行动画的时间
    // fc_effect.meteor_period = 15 + 4; // 时间周期，包括执行动画的时间

    Adafruit_NeoPixel_clear(); // 清空缓存残留
    WS2812FX_show();

    extern void WS2812FX_mode_comet_1(void);
    WS2812FX_stop();
    WS2812FX_setSegment_colorOptions(
        0,                                      // 第0段
        0, fc_effect.led_num,                   // 起始位置，结束位置
        &WS2812FX_mode_comet_1,                 // 效果
        PINK,                                   // 颜色，WS2812FX_setColors设置
        fc_effect.star_speed,                   // 速度
        B00000101 /* 6个灯为一组 */ | REVERSE); // 选项
    WS2812FX_start();
#endif

#if 0  // 多种颜色依次进行的流星灯动画
    // fc_effect.star_speed = 180; // 变化速度(0~65535，值越小，速度越快) 动画时间约 4 s
    // fc_effect.star_speed = 135;// 变化速度(0~65535，值越小，速度越快) 动画时间约 3 s
    // fc_effect.star_speed = 90; // 变化速度(0~65535，值越小，速度越快) 动画时间约 2 s
    fc_effect.star_speed = 45; // 变化速度(0~65535，值越小，速度越快) 动画时间约 1 s

    fc_effect.meteor_period = 4 + 1; // 时间周期，包括执行动画的时间
    // fc_effect.meteor_period = 8 + 2; // 时间周期，包括执行动画的时间
    // fc_effect.meteor_period = 10 + 3; // 时间周期，包括执行动画的时间
    // fc_effect.meteor_period = 15 + 4; // 时间周期，包括执行动画的时间

    Adafruit_NeoPixel_clear(); // 清空缓存残留
    WS2812FX_show();

    WS2812FX_stop();
    WS2812FX_setSegment_colorsOptions(
        0,                            // 第0段
        0, fc_effect.led_num,         // 起始位置，结束位置
        &WS2812FX_sample_9,           // 效果
        color_buff,                   // 颜色，WS2812FX_setColors设置
        fc_effect.star_speed,         // 速度
        B00000101 /* 6个灯为一组 */); // 选项
    WS2812FX_start();
#endif // 多种颜色依次进行的流星灯动画

#if 1 // 流水灯动画

    // 这里的速度和时间间隔跟流星灯的不一样
    // fc_effect.star_speed = 2000 / (fc_effect.led_num); // 变化速度(0~65535，值越小，速度越快)
    // fc_effect.star_speed = 4000; // 变化速度(0~65535，值越小，速度越快)
    // fc_effect.star_speed = 3000;      // 变化速度(0~65535，值越小，速度越快)
    fc_effect.star_speed = 2000;      // 变化速度(0~65535，值越小，速度越快)
    // fc_effect.star_speed = 1000;      // 变化速度(0~65535，值越小，速度越快)
    fc_effect.meteor_period = 15 + 4; // 时间周期，包括执行动画的时间

    fc_effect.b = 100;
    WS2812FX_setBrightness(fc_effect.b);

    Adafruit_NeoPixel_clear(); // 清空缓存残留
    WS2812FX_show();

    WS2812FX_stop();
    WS2812FX_setSegment_colorsOptions(
        0,                     // 第0段
        0,                     // 起始位置
        fc_effect.led_num - 1, // 结束位置（函数内部传参会给这个参数加一，所以填传参要减去1）
        &WS2812FX_sample_15,   // 效果
        color_buff,            // 颜色，WS2812FX_setColors设置
        fc_effect.star_speed,  // 速度
        // NO_OPTIONS);              // 选项
        REVERSE); // 选项
    WS2812FX_start();

    /* 对应样机的模式13： */
    // WS2812FX_stop();
    // WS2812FX_setSegment_colorsOptions(
    //     0,                     // 第0段
    //     0,                     // 起始位置
    //     fc_effect.led_num - 1, // 结束位置（函数内部传参会给这个参数加一，所以填传参要减去1）
    //     &WS2812FX_sample_13,   // 效果
    //     color_buff_mode13,     // 颜色，WS2812FX_setColors设置
    //     fc_effect.star_speed,  // 速度
    //     NO_OPTIONS);           // 选项
    //     // REVERSE); // 选项
    // WS2812FX_start();

#endif

    while (1)
    {
        rf24_key_handle();
        os_time_dly(1);
    }
}

void my_main(void)
{

    mic_gpio_init();
    led_state_init(); // 初始化LED接口
    led_pwr_on();
#if TCFG_RF433GKEY_ENABLE
    // rf433_gpio_init();
#endif

    read_flash_device_status_init();
    full_color_init();

    // sys_s_hi_timer_add(NULL, count_down_run, 10); //注册定时关机定时器
    // sys_s_hi_timer_add(NULL, time_clock_handler, 10); //注册定时做的时间计时定时器
    // sys_s_hi_timer_add(NULL, ir_timer_handler, 10); //注册红外定时器
    sys_s_hi_timer_add(NULL, meteor_period_sub, 10); // 注册流星周期定时器
    // sys_s_hi_timer_add(NULL, rf433_handle, 10);      // 注册433遥控功能定时器

    sys_timer_add(NULL, main_while, 10);

    // os_sem_create(&LED_TASK_SEM,0);
    // task_create(main_while, NULL, "led_task");

    task_create(test_task, NULL, "test_task");
}
