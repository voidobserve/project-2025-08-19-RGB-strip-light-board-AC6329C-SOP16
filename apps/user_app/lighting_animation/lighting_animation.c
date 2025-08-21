#include "lighting_animation.h"
#include "led_strand_effect.h"
#include "../../apps/user_app/ws2812-fx-lib/WS2812FX_C/WS2812FX.h"

extern fc_effect_t fc_effect;    // 幻彩灯串效果数据
extern Segment *_seg;            // currently active segment (20 bytes)
extern Segment_runtime *_seg_rt; // currently active segment runtime (16 bytes)
extern uint16_t _seg_len;        // num LEDs in the currently active segment

const u32 color_buff[] = {RED, GREEN, BLUE, YELLOW, PINK, CYAN, WHITE};

// 动画效果，对应样机模式8
u16 WS2812FX_sample_8(void)
{
    u8 offset = 13;

    if ((get_effect_p() == 1) && (fc_effect.mode_cycle == 1))
    {
        return (_seg->speed);
    }

    WS2812FX_fade_out();

    if (IS_REVERSE)
    {
        if ((_seg->stop - _seg->start) >= _seg_rt->counter_mode_step)
            WS2812FX_setPixelColor(_seg->stop - _seg_rt->counter_mode_step, _seg->colors[_seg_rt->aux_param]);
    }
    else
    {
        if (_seg_rt->counter_mode_step < _seg->stop + 1)
            WS2812FX_setPixelColor(_seg->start + _seg_rt->counter_mode_step, _seg->colors[_seg_rt->aux_param]);
    }

    _seg_rt->counter_mode_step = (_seg_rt->counter_mode_step + 1) % (_seg_len + offset);
    if (_seg_rt->counter_mode_step == 0)
    {
        SET_CYCLE;
        fc_effect.mode_cycle = 1;

        // 每轮动画结束，切换颜色
        // 颜色索引对应 color_buff[]
        if (_seg_rt->aux_param == 0)
        {
            _seg_rt->aux_param = 1;
        }
        else if (_seg_rt->aux_param == 1)
        {
            _seg_rt->aux_param = 2;
        }
        else if (_seg_rt->aux_param == 2)
        {
            _seg_rt->aux_param = 0;
        }
    }

    return (_seg->speed);
}

// 动画效果，对应样机模式9
u16 WS2812FX_sample_9(void)
{
    u8 offset = 13;

    if ((get_effect_p() == 1) && (fc_effect.mode_cycle == 1))
    {
        return (_seg->speed);
    }

    WS2812FX_fade_out();

    if (IS_REVERSE)
    {
        if ((_seg->stop - _seg->start) >= _seg_rt->counter_mode_step)
            WS2812FX_setPixelColor(_seg->stop - _seg_rt->counter_mode_step, _seg->colors[_seg_rt->aux_param]);
    }
    else
    {
        if (_seg_rt->counter_mode_step < _seg->stop + 1)
            WS2812FX_setPixelColor(_seg->start + _seg_rt->counter_mode_step, _seg->colors[_seg_rt->aux_param]);
    }

    _seg_rt->counter_mode_step = (_seg_rt->counter_mode_step + 1) % (_seg_len + offset);
    if (_seg_rt->counter_mode_step == 0)
    {
        SET_CYCLE;
        fc_effect.mode_cycle = 1;

        // 每轮动画结束，切换颜色
        // 颜色索引对应 color_buff[]
        if (_seg_rt->aux_param == 0)
        {
            _seg_rt->aux_param = 1; // GREEN 绿
        }
        else if (_seg_rt->aux_param == 1)
        {
            _seg_rt->aux_param = 2; // BLUE 蓝
        }
        else if (_seg_rt->aux_param == 2)
        {
            _seg_rt->aux_param = 6; // WHITE 白
        }
        else if (_seg_rt->aux_param == 6)
        {
            _seg_rt->aux_param = 3; // YELLOW 黄
        }
        else if (_seg_rt->aux_param == 3)
        {
            _seg_rt->aux_param = 4; // PINK 粉
        }
        else if (_seg_rt->aux_param == 4)
        {
            _seg_rt->aux_param = 5; // CYAN
        }
        else if (_seg_rt->aux_param == 5)
        {
            _seg_rt->aux_param = 0; // RED 红
        }
    }

    return (_seg->speed);
}
