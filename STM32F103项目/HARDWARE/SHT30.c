/**
	********************************************************************************
	*FileName:  SHT30.C
	*Author:  石家庄市电子信息学校24级13班李佳澍
	*Version:  1.0.0
	*Date:  2025/10/6
	*Description: SHT30温湿度传感器驱动文件
	*********************************************************************************
*/
/*硬件I2C驱动，请务必正确连接
	SCL -> PB10
	SDA -> PB11
*/
#include "SHT30.h"
#include "Delay.h"
#include "stm32f10x.h"                  // Device header
#include "stm32f10x_i2c.h"
#define SHT30_ADDR_WRITE (0x44 << 1)      // 从机地址左移一位为默认的地址
#define SHT30_ADDR_READ ((0x44 << 1) | 1) // 读地址
//周期性测量模式
#define SHT30_MEASURE_CMD_H 0x24
#define SHT30_MEASURE_CMD_L 0x00

void SHT30_IIC_Init(void)
{
	//时钟配置
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	//GPIO配置
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD; //配置GPIOB10 B11为开漏输出模式
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10 |GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	//I2C配置
	I2C_InitTypeDef I2C_InitStructure;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2; 
	I2C_InitStructure.I2C_OwnAddress1 = 0x00; //主机地址
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;//启用ACK应答
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = 100000; //100kHz
	
	I2C_Init(I2C2,&I2C_InitStructure);
	
	I2C_Cmd(I2C2,ENABLE);//启用I2C2
}

static void SHT30_SendCmd(uint8_t cmd_h,uint8_t cmd_l)
{
	while(I2C_GetFlagStatus(I2C2,I2C_FLAG_BUSY));
	
	I2C_GenerateSTART(I2C2,ENABLE); //硬件IIC发送起始信号
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT)); //等待准备就绪
	
	I2C_Send7bitAddress(I2C2,SHT30_ADDR_WRITE,I2C_Direction_Transmitter); //发送从机地址,方向:发送
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));//等待从机收到ACK应答
	
	I2C_SendData(I2C2,cmd_h);//发送高位
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));//等待发送
	
	I2C_SendData(I2C2,cmd_l);//发送低位
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));//等待发送
	
	I2C_GenerateSTOP(I2C2,ENABLE);//STOP结束信号完成发送
}

static uint8_t SHT30_CheckCrc(uint8_t data[], uint8_t nbrOfBytes, uint8_t checksum) //CRC校验
{
    uint8_t crc = 0xFF;  // CRC初始值
    uint8_t bit, byteCtr;
    
    // 计算每个数据字节
    for (byteCtr = 0; byteCtr < nbrOfBytes; ++byteCtr) {
        crc ^= data[byteCtr];  // 与当前数据字节异或
        
        // 处理每个字节的8个位
        for (bit = 8; bit > 0; --bit) {
            if (crc & 0x80) {
                // 最高位为1，执行多项式计算右移
                crc = (crc << 1) ^ 0x31;  // 多项式0x31
            } else {
                // 最高位为0，只进行左移
                crc = (crc << 1);
            }
        }
    }
	return (crc == checksum) ? 0 : 1;
}

uint8_t SHT30_ReadData(float *temp, float *hum)
{
	uint8_t data[6];
	uint8_t crc;
	uint16_t temp_raw;
	uint16_t hum_raw;
	
	SHT30_SendCmd(SHT30_MEASURE_CMD_H,SHT30_MEASURE_CMD_L);//发送测量命令
	
	Delay_ms(20);//等待测量
	
	// 发送START信号
    I2C_GenerateSTART(I2C2, ENABLE);
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));
	
	//发送设备地址
    I2C_Send7bitAddress(I2C2, SHT30_ADDR_READ, I2C_Direction_Receiver);
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	
    for (int i = 0; i < 6; i++) {
        // 在读取最后一个字节前发送NACK
        if (i == 5) {
            I2C_AcknowledgeConfig(I2C2, DISABLE);  // 关闭ACK表示最后一个字节
        }
        // 等待接收到数据
        while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED));
        // 读取接收到的数据
        data[i] = I2C_ReceiveData(I2C2);
    }
	
	I2C_GenerateSTOP(I2C2,ENABLE);//STOP结束信号
	
	I2C_AcknowledgeConfig(I2C2, ENABLE);//重新启用ACK应答
	crc = SHT30_CheckCrc(&data[0], 2, data[2]);
    if (crc != 0) {
        return 1;  // 温度数据CRC校验失败
    }
	crc = SHT30_CheckCrc(&data[3], 2, data[5]);
    if (crc != 0) {
        return 1;  // 湿度数据CRC校验失败
    }
	//原始数据转换
	temp_raw = (data[0] << 8) | data[1];
	hum_raw = (data[3] << 8) | data[4];
	
	*temp= -45.0 + 175.0 * temp_raw / 65535.0; //根据转换公式
	*hum = 100.0 * hum_raw / 65535.0;
	
	return 0;
}

void SHT30_Init(void)
{
	SHT30_IIC_Init();
	Delay_ms(20);
	SHT30_SendCmd(0x30, 0xA2);//复位SHT30的所有系统数据的代数初始状态
	Delay_ms(20);
}