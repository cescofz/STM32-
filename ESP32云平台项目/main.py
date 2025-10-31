'''
云平台账号:15511896620
云平台密码:123456
项目ID:1233183
此设备ID :1331745
此设备标识符:SJZSTM32MQTT
'''
import time
import network
import ubinascii
import hashlib
from umqtt.simple import MQTTClient
import machine
import ujson
from machine import UART,Pin
LED = Pin(2,Pin.OUT)
# 配置参数
WIFI_SSID = "2203"
WIFI_PASSWORD = "000997xh"

PROJECT_ID = "1233183"
DEVICE_ID = "SJZSTM32MQTT"
SECRET_KEY = "69ab7b495bca49d88b9495bd4c03fff9"
SERVER = "mqtt.nlecloud.com"
PORT = 1883
KEEP_ALIVE = 60

PUBLISH_TOPIC = f"/sys/{PROJECT_ID}/{DEVICE_ID}/sensor/datas"
SUBSCRIBE_TOPIC = f"/sys/{PROJECT_ID}/{DEVICE_ID}/cmd"

# 串口配置
UART_PORT = 2
BAUD_RATE = 9600
DATA_BITS = 8
STOP_BITS = 1
PARITY = None
uart = UART(UART_PORT, baudrate=BAUD_RATE, bits=DATA_BITS, stop=STOP_BITS, parity=PARITY)


def connect_wifi():
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    
    if not wlan.isconnected():
        print(f"正在连接WiFi: {WIFI_SSID}...")
        wlan.connect(WIFI_SSID, WIFI_PASSWORD)
        
        timeout = 10
        while not wlan.isconnected() and timeout > 0:
            time.sleep(1)
            timeout -= 1
            
        if wlan.isconnected():
            print(f"WiFi连接成功，IP地址: {wlan.ifconfig()[0]}")
        else:
            print("WiFi连接超时，程序重启重试")
            machine.reset()
    else:
        print("WiFi已连接，IP地址:", wlan.ifconfig()[0])
        
    return wlan


def generate_token(client_id, username, secret_key, method="sha256"):
    timestamp = str(int(time.time() * 1000))
    sign_str = f"{client_id}{username}{method}{timestamp}{secret_key}"
    
    if method == "md5":
        sign = ubinascii.hexlify(hashlib.md5(sign_str.encode()).digest()).decode()
    elif method == "sha1":
        sign = ubinascii.hexlify(hashlib.sha1(sign_str.encode()).digest()).decode()
    elif method == "sha256":
        sign = ubinascii.hexlify(hashlib.sha256(sign_str.encode()).digest()).decode()
    else:
        raise ValueError("不支持的签名算法")
        
    return f"{client_id}&{username}&{method}&{timestamp}&{sign}"


def connect_mqtt():
    password = generate_token(DEVICE_ID, PROJECT_ID, SECRET_KEY, method="sha256")
    
    client = MQTTClient(
        client_id=DEVICE_ID,
        server=SERVER,
        port=PORT,
        user=PROJECT_ID,
        password=password,
        keepalive=KEEP_ALIVE,
        ssl=False
    )
    
    try:
        client.connect()
        print("MQTT连接新大陆云平台成功")
        uart.write(b"@Connection\r\n")
        return client
    except Exception as e:
        print(f"MQTT连接失败: {e}，10秒后重启重试...")
        uart.write(b"@Disconnect\r\n")
        time.sleep(10)
        machine.reset()


def read_uart_data():
    if uart.any():
        try:
            uart_data = uart.read().decode("utf-8").strip()
            print(uart_data)
            if uart_data:
                parsed_data = ujson.loads(uart_data)
                print(f"读取到串口数据: {parsed_data}")
                return parsed_data
            else:
                return None
        except Exception as e:
            print(f"串口数据解析失败: {e}")
            return None
    else:
        return None


def publish_data(client, uart_data):
    if not uart_data:
        return
        
    msg_id = str(int(time.time()))
    payload = {
        "datatype": 1,
        "datas": uart_data,
        "msgid": msg_id
    }
    
    payload_str = ujson.dumps(payload)
    
    try:
        client.publish(topic=PUBLISH_TOPIC, msg=payload_str, qos=0)
        print(f"[{time.localtime()[3]:02d}:{time.localtime()[4]:02d}:{time.localtime()[5]:02d}]")
        print(f"数据上报成功: {payload_str}")
    except Exception as e:
        print(f"数据上报失败: {e}")


def main():
    connect_wifi()
    mqtt_client = connect_mqtt()
    print("等待串口数据...")
    
    while True:
        uart_data = read_uart_data()
        
        if uart_data:
            publish_data(mqtt_client, uart_data)
            
        time.sleep(1)


if __name__ == "__main__":
    LED.on()
    main()



