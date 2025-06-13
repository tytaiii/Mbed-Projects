#include "mbed.h"

// 硬件接口定义
SPI acc(A6, A5, A1);  // MOSI, MISO, SCLK
DigitalOut cs(D3);       // SPI3片选
DigitalOut leds[8] = {D11, D9, D0, D1, D6, A3, A2, A0}; // LED阵列

// ADXL345寄存器地址
const uint8_t DATA_FORMAT = 0x31;
const uint8_t POWER_CTL = 0x2D;
const uint8_t DATAX0 = 0x32;

void init_ADXL345() {
    cs = 1;
    acc.format(8, 3);          // 8位数据，SPI模式3
    acc.frequency(2000000);    // 2MHz时钟
    
    // 配置数据格式寄存器
    cs = 0;
    acc.write(DATA_FORMAT);    // 写寄存器地址
    acc.write(0x0B);           // 4线SPI, 全分辨率, ±16g
    cs = 1;
    thread_sleep_for(10);
    
    // 电源控制配置
    cs = 0;
    acc.write(POWER_CTL);
    acc.write(0x08);           // 测量模式
    cs = 1;
}

void read_accel(float &x, float &y, float &z) {
    uint8_t cmd = 0xF2;        // 多字节读 + 0x32地址
    uint8_t data[6];
    
    cs = 0;
    acc.write(cmd);            // 发送读取命令
    for(int i=0; i<6; i++) {
        data[i] = acc.write(0x00); // 获取6字节数据
    }
    cs = 1;
    
    // 转换为16位补码
    int16_t raw_x = (data[1] << 8) | data[0];
    int16_t raw_y = (data[3] << 8) | data[2];
    int16_t raw_z = (data[5] << 8) | data[4];
    
    // 转换为g值（±16g范围）
    x = raw_x * 0.004f;
    y = raw_y * 0.004f;
    z = raw_z * 0.004f;
}

void update_leds(float x, float y) 
    // 全部LED熄灭
  {
       for(auto &led : leds) led = 0;
      {
    // X轴控制
    if(x < -0.5f && y < -0.5f)      leds[3]=leds[7] = 1;// LXR2
    else if(x < -0.05f &&y < -0.05f) leds[2] =leds[6]= 1;  // LXR1
    else if(x > 0.5f && y > 0.5f)   leds[0] =leds[4]= 1; // LXG1
    else if(x > 0.05f && y > 0.05f)  leds[1] =leds[5] = 1;  // LXG2
    
  
      }
  }

int main() 
{
    init_ADXL345();
    float x, y, z;
    while(1) {
        read_accel(x, y, z);
        update_leds(x, y);
        
        // 启用标准printf输出浮点数
        printf("X:%.2f g\tY:%.2f g\tZ:%.2f g\n", x, y, z);
        thread_sleep_for(100);
    }
}