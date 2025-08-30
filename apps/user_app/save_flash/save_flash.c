
#include "system/includes.h"
#include "syscfg_id.h"
#include "save_flash.h"

// const u8 frist_mode[] = {0x3D, 0x00, 0x00, 0x0B, 0x00, 0x01, 0x00, 0x14, 0x00, 0x00, 0x01, 0x03, 0xE8, 0x03, 0xE8}; // 第一次上电默认模式

// extern u8 my_ble_state;

u8 my_ble_state = 1; // 默认开启BLE模块 /* 原本是433遥控器相关文件定义，现在移植到这里 */

volatile save_flash_t save_flash_structure = {0};

volatile save_info_t save_info = {0};

// extern u16 RF433_CODE;
/*******************************************************************************************************
**函数名：上电读取FLASH里保存的指令数据
**输  出：
**输  入：读取 CFG_USER_COMMAND_BUF_DATA 里保存的最后一条接收到的指令，
**描  述：读取 CFG_USER_LED_LEDGTH_DATA 里保存的第一次上电标志，灯带长度，顺序是：：第1字节：第一次上电标志位，第2、3字节：灯带长度
**说  明：
**版  本：
**修改日期：
*******************************************************************************************************/
void read_flash_device_status_init(void)
{
    int ret;

    // printf("%s %d\n", __func__, __LINE__);

    local_irq_disable(); // 禁用中断
    ret = syscfg_read(CFG_USER_LED_LEDGTH_DATA, (u8 *)(&save_flash_structure), sizeof(save_flash_t));
    local_irq_enable(); // 使能中断
    if (ret != sizeof(save_flash_t))
    {
        // 如果读取到的数据个数不一致
        // printf("read save info error \n");
        memset((u8 *)&save_flash_structure, 0, sizeof(save_flash_t));
    }

    os_time_dly(1);

    if (save_flash_structure.header != 0x55) // 第一次上电
    {
        fc_data_init();
        my_ble_state = 1; // 默认开启BLE模块
        // printf("is first power on\n");
        save_user_data_area3(); // 将初始化后的数据写回flash
    }
    else
    {
        // memcpy((u8 *)(&fc_effect), (u8 *)(&save_flash_structure.fc_save), sizeof(fc_effect_t));
        fc_effect = save_flash_structure.fc_save; // 结构体变量赋值
        my_ble_state = save_flash_structure.sa_ble_state;
        // RF433_CODE = save_flash3.sa_rf433_code;
        // printf("is not first power on\n");
    }

    // printf("%s %d\n", __func__, __LINE__);

    // printf("fc_effect.cur_mode %u\n", (u16)fc_effect.cur_mode);
    // printf("fc_effect.star_speed %u\n", (u16)fc_effect.star_speed);
    // printf("fc_effect.b %u\n", (u16)fc_effect.b);
    // printf("fc_effect.on_off_flag %u\n", (u16)fc_effect.on_off_flag);
    // printf("fc_effect.cur_options %u\n", (u16)fc_effect.cur_options);
    // printf("fc_effect.sequence %u\n", (u16)fc_effect.cur_options);
}

// 把用户数据写到区域3
void save_user_data_area3(void)
{
    printf("%s %d\n", __func__, __LINE__);
    int ret = 0;
    save_flash_structure.header = 0x55; // 表示数据有效
    save_flash_structure.sa_ble_state = my_ble_state;
    // save_data.sa_rf433_code = RF433_CODE;
    // memcpy((u8 *)(&save_flash_structure.fc_save), (u8 *)(&fc_effect), sizeof(fc_effect_t));
    save_flash_structure.fc_save = fc_effect; // 结构体变量赋值

    local_irq_disable(); // 禁用中断
    ret = syscfg_write(CFG_USER_LED_LEDGTH_DATA, (u8 *)(&save_flash_structure), sizeof(save_flash_t));
    local_irq_enable(); // 使能中断
    if (ret != sizeof(save_flash_t))
    {
        // 如果实际写入的数据与配置的参数不一致
    }

    // printf("ret %d \n", ret);
    // printf("sizeof(save_flash_t) == %u \n", (u16)sizeof(save_flash_t));

    // printf("save info done \n");
    // printf("%s %d\n", __func__, __LINE__);
}

// 从flash读出 save_info
void save_info_read(void)
{
    printf("%s %d\n", __func__, __LINE__);
    int ret = 0;
    local_irq_disable(); // 禁用中断
    ret = syscfg_read(CFG_USER_SAVE_INFO_ID, (u8 *)(&save_info), sizeof(save_info_t));
    local_irq_enable(); // 使能中断
    if (ret != sizeof(save_info_t))
    {
        // 如果读取到的数据个数不一致
        printf("read save info error \n");
        memset((u8 *)&save_info, 0, sizeof(save_info_t));
    }

    if (save_info.is_data_valid != 0xC5) // 第一次上电
    {
        printf("is first power on\n");

        save_info.is_data_valid = 0xC5; 
        lighting_animation_config_init(); // 初始化参数

        save_info_write(); // 将初始化后的数据写回flash
    }
    else
    {
        printf("is not first power on\n");
    }

    // printf("save_info.flag_is_light_on %u\n", (u16)save_info.flag_is_light_on);
    // printf("cur_options %u\n", (u16)save_info.cur_options);
    // printf("cur_brightness %u\n", (u16)save_info.cur_brightness);
    // printf("cur_lighting_animation_mode %u\n", (u16)save_info.cur_lighting_animation_mode);
    // printf("cur_speed %u\n", (u16)save_info.cur_speed);
    // printf("%s %d\n", __func__, __LINE__);
}

// 写入 save_info 到flash
void save_info_write(void)
{
    // printf("%s %d\n", __func__, __LINE__);
    int ret = 0;
    save_info.is_data_valid = 0xC5; // 表示数据有效

    local_irq_disable(); // 禁用中断
    ret = syscfg_write(CFG_USER_SAVE_INFO_ID, (u8 *)(&save_info), sizeof(save_info_t));
    local_irq_enable(); // 使能中断
    // printf("ret %d \n", ret);
    if (ret != sizeof(save_info_t))
    {
        // 如果实际写入的数据与配置的参数不一致
    }

    // printf("sizeof(save_info_t) == %u \n", (u16)sizeof(save_info_t));

    // printf("save info done \n");
    // printf("%s %d\n", __func__, __LINE__);
}

// 默认的初始化，从flash中读出保存的数据
// void save_info_init_by_default(void)
// {

// }
