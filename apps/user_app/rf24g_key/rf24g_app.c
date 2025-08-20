
#include "system/includes.h"
#include "task.h"
#include "event.h"
#include "btstack/btstack_typedef.h"
#include "app_config.h"
#include "rf24g_app.h"
#include "WS2812FX.H"
#include "led_strip_sys.h"
#if(TCFG_RF24GKEY_ENABLE)

/* ==============================================================

                2.4g遥控初始化

    备注：该2.4G遥控的代码逻辑，是根据遥控而定的，可能不适宜其他遥控
=================================================================*/



#define HEADER1 0XDC
#define HEADER2 0XDC

rf24g_ins_t rf24g_ins;  //2.4g遥控数据包格式

#define SUPPORT_LONG (1)

struct RF24G_PARA is_rf24g_ = {

    .clink_delay_up = 0,
    .long_press_cnt = 0,
    .last_key_v = NO_KEY,
    .last_dynamic_code = 0,
    .rf24g_rx_flag = 0,
    .hold_pess_cnt = 0,
    .is_long_time = 1000,
    .rf24g_key_state = 0,
    #if SUPPORT_LONG
    .is_click = 35,        //因没有长按功能，缩短抬起判断时间
    #else
    .is_click = 10,        //因没有长按功能，缩短抬起判断时间
    #endif
    .is_long = 15,
    ._sacn_t = 10,
};


static u8 Get_24G_KeyValue()
{

    if(is_rf24g_.rf24g_rx_flag)
    {
        is_rf24g_.rf24g_rx_flag = 0;
        is_rf24g_.clink_delay_up = 0;
        

        if((is_rf24g_.last_dynamic_code != rf24g_ins.dynamic_code ) )  //避免响应两帧数据，因为第二帧数据，不知道为什么总是相隔500ms
        {

            is_rf24g_.last_key_v = rf24g_ins.key_v;
            is_rf24g_.last_dynamic_code = rf24g_ins.dynamic_code;
            return rf24g_ins.key_v;   //当前2.4G架构无用,使用杰理遥控按键就使用
        }else{

            is_rf24g_.long_press_cnt++;

        }
    }
    return NO_KEY;

}




struct key_driver_para rf24g_scan_para = {
    .scan_time 	  	  = 2,				//按键扫描频率, 单位: ms
    .last_key 		  = NO_KEY,  		//上一次get_value按键值, 初始化为NO_KEY;
    .filter_time  	  = 0,				//按键消抖延时;
    .long_time 		  = 200,  			//按键判定长按数量
    .hold_time 		  = 200,  	        //按键判定HOLD数量
    .click_delay_time = 20,		        //按键被抬起后等待连击延时数量
    .key_type		  = KEY_DRIVER_TYPE_RF24GKEY,
    .get_value 		  = Get_24G_KeyValue,
};

// 底层按键扫描，由__resolve_adv_report()调用
void rf24g_scan(u8 *pBuf)
{
    rf24g_ins_t *p = (rf24g_ins_t *) pBuf;
    if(p->header1 == HEADER1 && p->header2 == HEADER2)
    {
        // printf_buf(pBuf,sizeof(rf24g_ins_t));
        memcpy((u8*)&rf24g_ins, pBuf, sizeof(rf24g_ins_t));
       is_rf24g_.rf24g_rx_flag = 1;
    }

}


/* ==============================================================

                2.4g遥控按键功能


=================================================================*/

void RF24G_Key_Handle(void)
{
    u8 key_value = 0;
    if(is_rf24g_.clink_delay_up < 0xFF)
    is_rf24g_.clink_delay_up++;
    // printf("is_rf24g_.clink_delay_up=%d",is_rf24g_.clink_delay_up);
    if((is_rf24g_.clink_delay_up > is_rf24g_.is_click) && (is_rf24g_.long_press_cnt < is_rf24g_.is_long))
    {
        key_value =  is_rf24g_.last_key_v;
        if(key_value == NO_KEY) return ;
        is_rf24g_.last_key_v = NO_KEY;
        is_rf24g_.clink_delay_up = 0;
        is_rf24g_.long_press_cnt = 0;
        printf(" ------------------------duanna key_value = %x", key_value);

        /*  code  */

        //开
        if(key_value == RF24_K01)
        {
           ls_ledStrip_switch();
        }

        if(get_on_off_state())
        {
            /* code */
            if(key_value == RF24_K03 )
            {
                ls_add_star_speed();
                
            }
            else if(key_value == RF24_K04 )
            {
                ls_sub_star_speed();
            }
            else if(key_value == RF24_K02 )
            {
                ls_change_star_mode();
                    
            }
            save_user_data_area3();
        }
        
    }
    else if (is_rf24g_.clink_delay_up > is_rf24g_.is_click)
    {
        is_rf24g_.clink_delay_up = 0;
        is_rf24g_.long_press_cnt = 0;
        is_rf24g_.last_key_v = NO_KEY;

    }


   if(is_rf24g_.long_press_cnt > is_rf24g_.is_long)
   {

        key_value =  is_rf24g_.last_key_v;
        if(key_value == NO_KEY) return ;
        is_rf24g_.last_key_v = NO_KEY;
        /*  因为执行这里，清了键值，而长按时，1s才获取一次键值 */
        printf(" ================================longkey_value = %d", key_value);


        #if SUPPORT_LONG

        /* code */
        if(get_on_off_state())
        {

            if(key_value == RF24_K02)
            {
            

                app_set_mereor_mode(7);

            }
            if(key_value == RF24_K21)
            {
                app_set_mereor_mode(13);
            }
            save_user_data_area3();
        }

        #endif



   }


}









#endif


