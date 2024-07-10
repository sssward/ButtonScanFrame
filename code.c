// 定义 Button 结构体和相关的函数指针类型和按键读取函数
typedef struct {
    uint8_t  val;            // 当前按键值
    uint8_t  val_last;       // 上一次按键值
    uint8_t  edge;           // 边沿类型：0为下降沿，1为上升沿
    uint16_t cnt;            // 边沿间隔计数
    uint8_t  mode;           // 状态机状态
    uint8_t  Interval_time;  // 双击间隔判定时间，单位ms
    uint8_t  single_flag;    // 单击标志位
    uint8_t  double_flag;    // 双击标志位
    uint8_t (*readPin)(void); // 函数指针
} Button;

uint8_t key1_readPin(void) {
    return HAL_GPIO_ReadPin(GPIOB, GPIO_Pin_13);
}

uint8_t key2_readPin(void) {
    return HAL_GPIO_ReadPin(GPIOB, GPIO_Pin_14);
}

void keyScan(Button *key) {
    // 使用没有参数的函数指针更新按键值
    key->val = key->readPin();
    
    // 初始状态（mode == 0）
    if (key->mode == 0 && key->val == key->edge && key->val_last != key->edge) {
        key->mode = 1; // 切换到模式 1
    }
    // 按键按下且未松手（mode == 1）
    else if (key->mode == 1 && key->val == key->edge && key->val_last == key->edge) {
        // 保持在模式 1
    }
    // 按键松手（mode == 1）
    else if (key->mode == 1 && key->val != key->val_last) {
        key->mode = 2; // 切换到模式 2
    }
    // 等待第二次按键按下或触发单击（mode == 2）
    else if (key->mode == 2) {
        if (key->val == 1) {
            key->cnt++;
        }
        if (key->cnt < (uint16_t)(key->Interval_time / 10) && key->val == 0) {
            key->double_flag = 1;
            key->mode = 0;
            key->cnt = 0;
        } else if (key->cnt >= (uint16_t)(key->Interval_time / 10)) {
            key->single_flag = 1;
            key->mode = 0;
            key->cnt = 0;
        }
    }
    
    // 更新上一次按键值
    key->val_last = key->val;
}

int main() {
    Button key1 = {
        .val = 0,
        .val_last = 0,
        .edge = 0, // 根据你的需求设置
        .cnt = 0,
        .mode = 0,
        .Interval_time = 100, // 根据你的需求设置
        .single_flag = 0,
        .double_flag = 0,
        .readPin = key1_readPin // 初始化函数指针
    };

    Button key2 = {
        .val = 0,
        .val_last = 0,
        .edge = 0, // 根据你的需求设置
        .cnt = 0,
        .mode = 0,
        .Interval_time = 100, // 根据你的需求设置
        .single_flag = 0,
        .double_flag = 0,
        .readPin = key2_readPin // 初始化函数指针
    };

    while (1) {
        keyScan(&key1);
        keyScan(&key2);

        if (key1.single_flag) {
            //key1单击事件
            key1.single_flag = 0; // 重置标志位
        }
        if (key1.double_flag) {
            //key1双击事件
            key1.double_flag = 0; // 重置标志位
        }


        if (key2.single_flag) {
            //key2单击事件
            key2.single_flag = 0; // 重置标志位
        }
        if (key2.double_flag) {
            //key2双击事件
            key2.double_flag = 0; // 重置标志位
        }
    }

    return 0;
}
