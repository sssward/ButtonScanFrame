#include <stdint.h>
#include <stdio.h>  // 用于测试输出

// 按键结构体定义
typedef struct {
    uint8_t val;              // 当前按键值
    uint8_t val_last;         // 上一次按键值
    uint8_t edge;             // 按键有效判定边沿：0为下降沿，1为上升沿
    uint8_t T_scan;           // 按键扫描周期(推荐10ms),ms
    uint16_t cnt;             // 边沿间隔计数
    uint8_t mode;             // 状态机状态
    uint16_t Interval_time;   // 双击间隔判定时间,ms
    uint16_t Longpress_time;  // 长摁判定时间,ms
    uint8_t (*readPin)(void); // 按键读取函数
    void (*singlePress)(void);// 单击响应函数
    void (*doublePress)(void);// 双击响应函数
    void (*longPress)(void);  // 长摁响应函数
} Button;

// 重写按键检测和相应事件处理函数
static inline uint8_t key1_readPin(void) 
{
    //重写按键检测的函数
}

static inline void key1_singlePress(void) 
{
    //单击响应函数
}

static inline void key1_doublePress(void) 
{
    //双击响应函数
}

static inline void key1_longPress(void) 
{
    //长摁响应函数
}

// 结构体初始化函数
void KeyInit(Button* button, uint8_t T_scan, uint8_t edge, uint16_t interval_time, uint16_t Longpress_time, 
                uint8_t (*readPin)(void), void (*singlePress)(void), void (*doublePress)(void), void (*longPress)(void)) 
{
    button->val = 0;
    button->val_last = 0;
    button->edge = edge;
    button->T_scan = T_scan;
    button->cnt = 0;
    button->mode = 0;
    button->Interval_time = interval_time;
    button->Longpress_time = Longpress_time;
    button->readPin = readPin;
    button->singlePress = singlePress;
    button->doublePress = doublePress;
    button->longPress = longPress;
}

// 按键扫描函数
// 放在一个周期为10ms的定时器中断中扫描
void key_scan(Button *key) 
{
    key->val = key->readPin();

    if (key->mode == 0 && key->val == key->edge && key->val_last != key->edge) 
    {
        key->mode = 1;
    }
    else if (key->mode == 1 && key->val == key->edge && key->val_last == key->edge) 
    {
        key->cnt++;
        if(key->cnt >= key->Longpress_time / key->T_scan)
        {
            key->longPress();
            key->mode = 0;
            key->cnt = 0;
        }
    }
    else if (key->mode == 1 && key->val != key->val_last) 
    {
        key->mode = 2;
        key->cnt = 0;
    }
    else if (key->mode == 2)
    {
        if (key->val == 1) 
        {
            key->cnt++;
        }
        if (key->cnt < (uint16_t)(key->Interval_time / key->T_scan) && key->val == 0) 
        {
            key->doublePress();
            key->mode = 0;
            key->cnt = 0;
        } 
        else if (key->cnt >= (uint16_t)(key->Interval_time / key->T_scan)) 
        {
            key->singlePress();
            key->mode = 0;
            key->cnt = 0;
        }
    }

    key->val_last = key->val;
}

// 主函数初始化按键实例
int main() {
    Button key1;
    KeyInit(&key1, 10, 0, 100, 500, key1_readPin, key1_singlePress, key1_doublePress, key1_longPress);
    
    while (1) 
    {
        //主函数
    }

    return 0;
}

//假设这个是周期为key->T_scan的定时器中断
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    key_scan(&key1);//扫描按键
}
