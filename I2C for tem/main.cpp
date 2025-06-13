#include "mbed.h"
#include <cstdio>

I2C i2c(D4,D5);  // SDA, SCL
const int tmp102_addr = 0x48 << 1;  // 左移后地址为0x90
// 使用BufferedSerial替代旧的Serial类
BufferedSerial pc(USBTX, USBRX);  // 串口通信

int main() {
    // 设置串口波特率
    pc.set_baud(9600);
    
    // 配置TMP102(连续转换模式，12位分辨率)
    char config[3] = {0x01, 0x60, 0xA0};  // 指针寄存器，配置MSB，配置LSB
    i2c.write(tmp102_addr, config, 3);
    
    while(1) {
        char reg[1] = {0x00};  // 温度寄存器
        char data[2] = {0, 0};
        
        i2c.write(tmp102_addr, reg, 1);
        i2c.read(tmp102_addr|1, data, 2);
        
        // 转换数据为温度值(摄氏度)
        int16_t temp = (data[0] << 4) | (data[1] >> 4);
        float temperature = temp * 0.0625;
        
        // 使用printf输出到串口
        printf("temperature: %.2f C\r\n", temperature);
        ThisThread::sleep_for(1000ms);
    }
}