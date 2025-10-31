	所有文件UTF-8编码，如果乱码使用UTF-8编码打开文件
	此项目实现STM32读取SHT30温湿度传感器、烟雾传感器数据并将数据用0.96寸OLED显示屏显示。
	当烟雾传感器检测到烟雾时控制继电器吸合，继电器可以外接报警设备。
	传感器数据经过cJSON库处理为JSON用USART串口发送给ESP32并使用MQTT协议上报至新大陆物联网云平台(https://api.nlecloud.com/)该平台提供开发者免费注册使用 。
	可以实现手机APP调用API实时检测传感器数据
	云平台账号:15511896620
	云平台密码:123456
	项目ID:1233183
	ESP32设备ID :1331745
	ESP32设备标识符:SJZSTM32MQTT
	OLED显示字段说明:
	Temperature: 温度 单位:℃
	Humidity: 湿度 单位:%
	Smoke: 烟雾状态 0 是没烟 1是有烟
	Cloud: 云平台连接状态 Connection:连接 Disconnect断开连接
	
	ESP32使用Micropython语言开发，请用Thonny软件打开main.py
	Android APP使用Vue开发，请用HBuilder X打开项目文件夹

	硬件连接:
	所有设备使用统一供电，不得高于3.3V，不要反接!
	野火小智开发板接STLINK:
	3V3 -> VERF
	TMS -> SWDIO
	GND -> GND
	TCK -> SWCLK
	RST -> RST
	
	SHT30温湿度:
	VCC -> +3.3V
	GND -> GND
	SCL -> B10
	SDA -> B11
	
	OLED显示屏:
	VCC -> +3.3V
	GND -> GND
	SCL -> B6
	SDA -> B7
	
	烟雾传感器:
	VCC -> +3.3V
	GND -> GND
	DO -> A5
	
	继电器(低电平触发)：
	DC+ -> +3.3V
	DC- -> GND
	IN1 -> A6
	IN2 -> A7
	
	LED:
	野火小智开发板板载LED A2
	按键:
	野火小智开发板板载按键 A0
	
	接ESP32的串口
	A9(TX) -> ESP32的D17(RX)
	A10(RX) -> ESP32的D16(TX)
	串口配置:
	波特率:9600
	数据位:8
	停止位:1
	奇偶校验:无
	发送编码:UTF-8