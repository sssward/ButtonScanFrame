#include <stdint.h>
#include <stdio.h>  // 用于测试输出

//1.按键结构体定义
typedef struct {
    uint8_t val;              // 当前按键值
    uint8_t val_last;         // 上一次按键值
    uint8_t edge;             // 按键有效判定边沿：0为下降沿，1为上升沿
    uint8_t T_scan;           // 按键扫描周期(推荐10ms),ms
    uint16_t cnt;             // 边沿间隔计数
    uint8_t mode;             // 状态机状态
    uint16_t Interval_time;   // 双击间隔判定时间,ms
    uint16_t Longpress_time;  // 长摁判定时间,ms
    uint8_t single_flag;      // 单击标志位
    uint8_t double_flag;      // 双击标志位
    uint8_t Longpress_flag;   // 长摁标志位
    uint8_t (*readPin)(void); // 函数指针
} Button;

//2.重写按键检测函数
static inline uint8_t key1_readPin(void) 
{
    return (uint8_t)HAL_GPIO_ReadPin(GPIOB, GPIO_Pin_13);
}


//3.结构体初始化函数
void KeyInit(Button* button, uint8_t T_scan, uint8_t edge, uint8_t interval_time, uint8_t Longpress_time, uint8_t (*readPin)(void)) 
{
    button->val = 0;
    button->val_last = 0;
    button->edge = edge;
    button->T_scan = T_scan;
    button->cnt = 0;
    button->mode = 0;
    button->Interval_time = interval_time;
    button->Longpress_time = Longpress_time;
    button->single_flag = 0;
    button->double_flag = 0;
    button->Longpress_flag = 0;
    button->readPin = readPin;
}

//4.按键扫描函数
//放在一个周期为10ms的定时器中断中扫描
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
        if(key->cnt >= key->Longpress_time /key->T_scan)
        {
            key->Longpress_flag = 1;
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
            key->double_flag = 1;
            key->mode = 0;
            key->cnt = 0;
        } 
        else if (key->cnt >= (uint16_t)(key->Interval_time / key->T_scan)) 
        {
            key->single_flag = 1;
            key->mode = 0;
            key->cnt = 0;
        }
    }

    key->val_last = key->val;
}

//5.主函数扫描相应事件flag
int main() {
    Button key1;
    KeyInit(&key1, 10, 0, 100, 500, key1_readPin);
    
    while (1) 
    {
        key_scan(&key1);

        if (key1.single_flag) 
        {
            //单击事件
            key1.single_flag = 0;
        }
        if (key1.double_flag) 
        {
            //双击事件
            key1.double_flag = 0;
        }
        if (key1.Longpress_flag) 
        {
            //长摁事件
            key1.Longpress_flag = 0;
        }
    }

    return 0;
}
