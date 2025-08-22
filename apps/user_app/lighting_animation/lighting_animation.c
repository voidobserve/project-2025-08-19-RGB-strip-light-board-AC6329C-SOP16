#include "lighting_animation.h"
#include "led_strand_effect.h"
#include "../../apps/user_app/ws2812-fx-lib/WS2812FX_C/WS2812FX.h"

extern fc_effect_t fc_effect;    // 幻彩灯串效果数据
extern Segment *_seg;            // currently active segment (20 bytes)
extern Segment_runtime *_seg_rt; // currently active segment runtime (16 bytes)
extern uint16_t _seg_len;        // num LEDs in the currently active segment

const u32 color_buff[] = {RED, GREEN, BLUE, YELLOW, PINK, CYAN, WHITE};

/*
    动画效果，对应样机模式8
    依次进行流星动画，每个颜色的动画都有时间间隔
    红 绿 蓝

    注意：
    速度值 == 单个动画时间
*/
u16 WS2812FX_sample_8(void)
{
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

    /*
        执行一次 0->_seg_len的动画所需时间由 _seg->speed 决定
        interval 控制每轮动画的时间，
        每轮动画的时间 == 执行一次 0->_seg_len的动画所需时间 + 动画之间的时间间隔
    */
    u16 interval = 0;
    if (_seg->speed == 1000) // 速度值对应 1s，从0到_seg_len的动画时间为1s
    {
        interval = _seg_len * (4 + 1); //
    }
    else if (_seg->speed == 2000) // 速度值对应 2s，从0到_seg_len的动画时间为2s
    {
        interval = _seg_len * ((8 + 2) / 2);
        // interval = _seg_len; // 测试去掉动画时间间隔，动画本身所需的时间
    }
    else if (_seg->speed == 3000)
    {
        // interval = _seg_len * ((10 + 3) *10 / 3) / 10; // 这一句会丢失部分时间精度
        interval = _seg_len * (((double)10 + 3) / 3);
        // interval = _seg_len; // 测试去掉动画时间间隔，动画本身所需的时间
    }
    else if (_seg->speed == 4000)
    {
        interval = _seg_len * (((double)15 + 4) / 4);
        // interval = _seg_len; // 测试去掉动画时间间隔，动画本身所需的时间
    }

    _seg_rt->counter_mode_step = (_seg_rt->counter_mode_step + 1) % (interval);

    if (_seg_rt->counter_mode_step == 0)
    {
        SET_CYCLE;
        fc_effect.mode_cycle = 1; // 目前未使用

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

    return (_seg->speed / fc_effect.led_num);
}

/*
    动画效果，对应样机模式9
    依次进行流星动画，每个颜色的动画都有时间间隔
    红 绿 蓝 白 黄 粉 cyan

    注意：
    速度值 == 单个动画时间
*/
u16 WS2812FX_sample_9(void)
{
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

    /*
        执行一次 0->_seg_len的动画所需时间由 _seg->speed 决定
        interval 控制每轮动画的时间，
        每轮动画的时间 == 执行一次 0->_seg_len的动画所需时间 + 动画之间的时间间隔
    */
    u16 interval = 0;
    if (_seg->speed == 1000) // 速度值对应 1s，从0到_seg_len的动画时间为1s
    {
        interval = _seg_len * (4 + 1); //
    }
    else if (_seg->speed == 2000) // 速度值对应 2s，从0到_seg_len的动画时间为2s
    {
        interval = _seg_len * ((8 + 2) / 2);
        // interval = _seg_len; // 测试去掉动画时间间隔，动画本身所需的时间
    }
    else if (_seg->speed == 3000)
    {
        // interval = _seg_len * ((10 + 3) *10 / 3) / 10; // 这一句会丢失部分时间精度
        interval = _seg_len * (((double)10 + 3) / 3);
        // interval = _seg_len; // 测试去掉动画时间间隔，动画本身所需的时间
    }
    else if (_seg->speed == 4000)
    {
        interval = _seg_len * (((double)15 + 4) / 4);
        // interval = _seg_len; // 测试去掉动画时间间隔，动画本身所需的时间
    }

    _seg_rt->counter_mode_step = (_seg_rt->counter_mode_step + 1) % (interval);

    if (_seg_rt->counter_mode_step == 0)
    {
        SET_CYCLE;
        fc_effect.mode_cycle = 1; // 表示模式完成一个循环 // 目前未使用

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

    return (_seg->speed / fc_effect.led_num);
}

/*
    动画效果，对应样机模式12
    红 绿 蓝 黄 粉 cyan 白 依次进行流水动画，亮度均匀，几乎没有动画时间间隔
    例如:
    红色流水一次，灯光全部熄灭，再到绿色流水一次，灯光全部熄灭...

    注意，传入的速度值，在有关灯珠数量的系数上，应该在灯光数量的基础上加一，因为每一轮动画结束后有个灯光全部熄灭的动画
    例如：速度值 == 单个动画时间
*/
u16 WS2812FX_sample_12(void)
{
    if (_seg_rt->counter_mode_step < _seg_len)
    {
        uint32_t led_offset = _seg_rt->counter_mode_step;
        if (IS_REVERSE)
        {
            WS2812FX_setPixelColor(_seg->stop - led_offset, _seg->colors[_seg_rt->aux_param]);
        }
        else
        {
            WS2812FX_setPixelColor(_seg->start + led_offset, _seg->colors[_seg_rt->aux_param]);
        }
    }
    else
    {
        // 一轮动画结束后，让所有灯光显示黑色（熄灭）
        for (u16 i = 0; i < _seg_len; i++)
        {
            WS2812FX_setPixelColor(i, BLACK);
        }
    }

    _seg_rt->counter_mode_step = (_seg_rt->counter_mode_step + 1) % (_seg_len + 1);

    if (_seg_rt->counter_mode_step == 0)
    {
        SET_CYCLE;
        fc_effect.mode_cycle = 1; // 表示模式完成一个循环

        _seg_rt->aux_param++;
        if (_seg_rt->aux_param >= 7) // 移植时需要后改为对应数组长度
        {
            _seg_rt->aux_param = 0;
        }
    }

    return (_seg->speed / (fc_effect.led_num + 1));
}

/*
    动画效果，对应样机模式14
    11个灯为一组，进行流水动画，顺序：红 绿 蓝 黄 粉 cyan 白（一开始是红色全亮），没有动画时间间隔
    注意：
    1. 一开始是红灯全亮，之后点亮第一个灯，从第二个灯开始色流水，依次进行流水动画
    2. 由于是11个灯为一组，总数是12个灯，传入的速度值，在有关灯珠数量的系数上，应该在灯光数量的基础上减一
        例如： 速度值 == 单个动画时间


    动画拆分：
    1. 首次进入，所有灯显示红色
    2. 第一个灯显示绿色
    3. 从第二个灯开始流水
    4. 流水到最后一个灯的同时，第一个灯要换成下一种颜色
    5. 第一个灯换到下一种颜色显示后，继续流水，从第二个灯开始流水
    除了首次进入，依次执行步骤 345
*/
u16 WS2812FX_sample_14(void)
{
    if (_seg_rt->counter_mode_call == 0)
    {
        // 动画刚开始时，所有灯光填充第1个颜色(灯光全部显示第一个颜色)，从第2个颜色开始流水
        for (u16 i = 0; i < _seg_len; i++)
        {
            WS2812FX_setPixelColor(i, _seg->colors[_seg_rt->aux_param]);
        }

        // _seg_rt->counter_mode_step = 0; // 刚进入该模式，从第一个灯开始上色（刚开始就是0，可以不写）

        _seg_rt->aux_param++; // 切换到下一种颜色

        return (_seg->speed); // 提前返回
    }

    if (0 == _seg_rt->counter_mode_step)
    {
        if (IS_REVERSE)
        {
            WS2812FX_setPixelColor(_seg->stop, _seg->colors[_seg_rt->aux_param]);
        }
        else
        {
            WS2812FX_setPixelColor(_seg->start, _seg->colors[_seg_rt->aux_param]);
        }
        _seg_rt->counter_mode_step = 1;

        if (1 == _seg_rt->counter_mode_call) // 刚进入该模式，从第一个灯开始上色
            return (_seg->speed);            // 提前返回
    }

    if (_seg_rt->counter_mode_step < _seg_len)
    {
        uint32_t led_offset = _seg_rt->counter_mode_step;

        if (IS_REVERSE)
        {
            WS2812FX_setPixelColor(_seg->stop - led_offset, _seg->colors[_seg_rt->aux_param]);
        }
        else
        {
            WS2812FX_setPixelColor(_seg->start + led_offset, _seg->colors[_seg_rt->aux_param]);
        }

        if (_seg_rt->counter_mode_step >= _seg_len - 1)
        {
            u8 index = 0;
            index = _seg_rt->aux_param + 1;
            if (index >= 7) // 移植时需要后改为对应数组长度
            {
                index = 0;
            }

            if (IS_REVERSE)
            {
                WS2812FX_setPixelColor(_seg->stop, _seg->colors[index]);
            }
            else
            {
                WS2812FX_setPixelColor(_seg->start, _seg->colors[index]);
            }
        }
    }

    _seg_rt->counter_mode_step = (_seg_rt->counter_mode_step + 1) % (_seg_len);

    if (_seg_rt->counter_mode_step == 0)
    {
        SET_CYCLE;
        // fc_effect.mode_cycle = 1; // 表示模式完成一个循环

        _seg_rt->aux_param++;
        if (_seg_rt->aux_param >= 7) // 移植时需要后改为对应数组长度
        {
            _seg_rt->aux_param = 0;
        }
    }

    return (_seg->speed / (fc_effect.led_num - 1));
}

/*
    动画效果，对应样机模式16
    单个灯为一组，进行流水动画，背景为黑色
    顺序：红 绿 蓝 黄 粉 cyan 白
    每轮动画结束后，灯光全灭，灭灯的时间很短

    注意：
    速度值 == 单个动画时间
*/
u16 WS2812FX_sample_16(void)
{
    if (_seg_rt->counter_mode_step < _seg_len)
    {
        uint32_t led_offset = _seg_rt->counter_mode_step;
        u32 black_offset = 0;
        if (led_offset == 0)
        {
            black_offset = _seg->stop;
        }
        else
        {
            black_offset = led_offset - 1;
        }

        // Adafruit_NeoPixel_clear(); // 节省时间，不使用该语句

        if (IS_REVERSE)
        {
            WS2812FX_setPixelColor(_seg->stop - black_offset, BLACK);
            WS2812FX_setPixelColor(_seg->stop - led_offset, _seg->colors[_seg_rt->aux_param]);
        }
        else
        {
            WS2812FX_setPixelColor(_seg->start + black_offset, BLACK);
            WS2812FX_setPixelColor(_seg->start + led_offset, _seg->colors[_seg_rt->aux_param]);
        }
    }

    _seg_rt->counter_mode_step = (_seg_rt->counter_mode_step + 1) % (_seg_len);

    if (_seg_rt->counter_mode_step == 0)
    {
        SET_CYCLE;
        // fc_effect.mode_cycle = 1; // 表示模式完成一个循环

        _seg_rt->aux_param++;
        if (_seg_rt->aux_param >= 7) // 移植时需要后改为对应数组长度
        {
            _seg_rt->aux_param = 0;
        }
    }

    return (_seg->speed / fc_effect.led_num);
}

/*
    动画效果，对应样机模式18
    mode12 + mode16的动画组合
    先跑完mode12，再跑mode16

    注意：
    传入的速度值 == 单个动画运行时间
*/
u16 WS2812FX_sample_18(void)
{
    if (0 == _seg_rt->counter_mode_step)
    {
        // 清除上一轮的颜色残留
        Adafruit_NeoPixel_clear();
    }

    if (0 == _seg_rt->aux_param3)
    {
        if (_seg_rt->counter_mode_step < _seg_len)
        {
            uint32_t led_offset = _seg_rt->counter_mode_step;
            if (IS_REVERSE)
            {
                WS2812FX_setPixelColor(_seg->stop - led_offset, _seg->colors[_seg_rt->aux_param]);
            }
            else
            {
                WS2812FX_setPixelColor(_seg->start + led_offset, _seg->colors[_seg_rt->aux_param]);
            }
        }
        else
        {
            // 一轮动画结束后，让所有灯光显示黑色（熄灭）
            for (u16 i = 0; i < _seg_len; i++)
            {
                WS2812FX_setPixelColor(i, BLACK);
            }
        }

        _seg_rt->counter_mode_step = (_seg_rt->counter_mode_step + 1) % (_seg_len + 1);
    }
    else
    {
        if (_seg_rt->counter_mode_step < _seg_len)
        {
            uint32_t led_offset = _seg_rt->counter_mode_step;
            u32 black_offset = 0;
            if (led_offset == 0)
            {
                black_offset = _seg->stop;
            }
            else
            {
                black_offset = led_offset - 1;
            }

            if (IS_REVERSE)
            {
                WS2812FX_setPixelColor(_seg->stop - black_offset, BLACK);
                WS2812FX_setPixelColor(_seg->stop - led_offset, _seg->colors[_seg_rt->aux_param]);
            }
            else
            {
                WS2812FX_setPixelColor(_seg->start + black_offset, BLACK);
                WS2812FX_setPixelColor(_seg->start + led_offset, _seg->colors[_seg_rt->aux_param]);
            }
        }

        _seg_rt->counter_mode_step = (_seg_rt->counter_mode_step + 1) % (_seg_len);
    }

    if (_seg_rt->counter_mode_step == 0)
    {
        SET_CYCLE;
        // fc_effect.mode_cycle = 1; // 表示模式完成一个循环

        _seg_rt->aux_param++;
        if (_seg_rt->aux_param >= 7) // 移植时需要后改为对应数组长度
        {
            _seg_rt->aux_param = 0;

            if (_seg_rt->aux_param3 == 0)
            {
                _seg_rt->aux_param3 = 1;
            }
            else
            {
                _seg_rt->aux_param3 = 0;
            }
        }
    }

    if (_seg_rt->aux_param3 == 0)
    {
        return (_seg->speed / (fc_effect.led_num + 1));
    }
    else
    {
        return (_seg->speed / fc_effect.led_num);
    }
}