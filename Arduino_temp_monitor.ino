#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
 
#define Led_pin 5  //led pin 口
#define Buzzer_pin 6 //蜂鸣器 pin 口
#define ONE_WIRE_BUS 2 //oneWire 总线 pin 口
#define button_switch 8 //切换按键
#define button_up 9 //温度增按键
#define button_down 10 //温度减按键

OneWire oneWire(ONE_WIRE_BUS); //创建 onewire 总线对象

DallasTemperature Ds18B20_Sensors(&oneWire); //创建 ds18b20 传感器对象

Adafruit_SSD1306 display(128, 64, &Wire, 4); //创建显示屏对象

float Ds18B20_Data[4];      //创建 DS18B20 数组储存温度
bool Alarm_flag=false;      //报警标志
uint8_t DeviceNum = 0;      //设备数量
int8_t HighAlarmTemp = 28;  //上限报警温度
int8_t LowAlarmTemp = 0;    //下限报警温度

unsigned long LastTime=0;   // 记时用  每750ms更新一次温度
unsigned long NowTime=0;    //


typedef struct //按键操作
{
  unsigned char index;
  void (*operation)(void);
} KEY_TABLE;

unsigned char keyValue = 0; //按键值
unsigned char funIndex = 1; //初始界面索引
void (*current)(void);
void menu0(void);
void menu1(void);
void menu2(void);
KEY_TABLE table[3] = {
    {0,  (*menu0)},
    {1,  (*menu1)},
    {2,  (*menu2)}};

//界面0
void menu0(void)
{
  keyValue = keyScan();
  if (keyValue == 2)  //按下温度增加键
    LowAlarmTemp += 1;
  else if (keyValue == 1) //按下温度减
    LowAlarmTemp -= 1;
  display.clearDisplay();
  display.setTextColor(WHITE); //设置字体颜色
  display.setTextSize(2);      //设置字体大小
  display.setCursor(0, 25);    //显示坐标
  display.print("LOW: ");
  display.print(LowAlarmTemp);
  display.print((char)247); // degree symbol
  display.print("C");
  display.display();
}

//界面1
void menu1(void)
{
  //*************************OLED****************************//
  display.clearDisplay();
  display.setTextColor(WHITE); //设置字体颜色
  display.setTextSize(1);      //设置字体大小

  display.setCursor(0, 0);
  display.print("Real-time temperature");

  display.setCursor(0, 14);
  display.print("Alarm Temp: ");
  display.print(LowAlarmTemp);
  display.print((char)247); // degree symbol
  display.print("C ");
  display.print(HighAlarmTemp);
  display.print((char)247); // degree symbol
  display.print("C");

  display.setCursor(5, 30);
  display.print("T1: ");
  display.print(Ds18B20_Sensors.getTempCByIndex(0));
  Ds18B20_Data[0]=Ds18B20_Sensors.getTempCByIndex(0);
  Serial.print(Ds18B20_Sensors.getTempCByIndex(0));

  display.setCursor(70, 30);
  display.print("T2: ");
  display.print(Ds18B20_Sensors.getTempCByIndex(1));
  Ds18B20_Data[1]=Ds18B20_Sensors.getTempCByIndex(1);
  Serial.print(Ds18B20_Sensors.getTempCByIndex(1));

  display.setCursor(5, 50);
  display.print("T3: ");
  display.print(Ds18B20_Sensors.getTempCByIndex(2));
  Ds18B20_Data[2]=Ds18B20_Sensors.getTempCByIndex(2);
  Serial.print(Ds18B20_Sensors.getTempCByIndex(2));

  display.setCursor(70, 50);
  display.print("T4: ");
  display.print(Ds18B20_Sensors.getTempCByIndex(3));
  Ds18B20_Data[3]=Ds18B20_Sensors.getTempCByIndex(3);
  Serial.print(Ds18B20_Sensors.getTempCByIndex(3));

  display.display();

  //*********************************************************//
}

//界面2
void menu2(void)
{
  keyValue = keyScan();
  if (keyValue == 2)
    HighAlarmTemp += 1;
  else if (keyValue == 1)
    HighAlarmTemp -= 1;
  display.clearDisplay();
  display.setTextColor(WHITE); //设置字体颜色
  display.setTextSize(2);      //设置字体大小
  display.setCursor(0, 25);
  display.print("HIGH: ");
  display.print(HighAlarmTemp);
  display.print((char)247); // degree symbol
  display.print("C");
  display.display();
}

//按键扫描函数
uint8_t keyScan(void)
{
  uint8_t buttonStateup=HIGH;
  buttonStateup=digitalRead(button_up);
  if(buttonStateup == LOW)
  {
    delay(20);
    buttonStateup=digitalRead(button_up);
    if(buttonStateup == LOW)
    {
      Serial.println("温度减");
      return 1;
    }
  }
  uint8_t buttonStatedown=HIGH;
  buttonStatedown=digitalRead(button_down);
  if(buttonStatedown == LOW)
  {
    delay(20);
    buttonStatedown=digitalRead(button_down);
    if(buttonStatedown == LOW)
    {
      return 2;
      Serial.println("温度加");
    }
  }  
  return 0;
}


void setup()
{
  Serial.begin(9600);                         //串口初始化
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // OLED 屏幕初始化

  //led 蜂鸣器 初始化
  pinMode(Buzzer_pin, OUTPUT);
  digitalWrite(Buzzer_pin, 0);
  pinMode(Led_pin, OUTPUT);
  digitalWrite(Led_pin, 0);
  analogWrite(Buzzer_pin, 0);
  digitalWrite(Led_pin, 0);

    //按键初始化
  pinMode(button_switch, INPUT_PULLUP); //设置按键管脚上拉输入模式 
  pinMode(button_down, INPUT_PULLUP); //设置按键管脚上拉输入模式 
  pinMode(button_up, INPUT_PULLUP); //设置按键管脚上拉输入模式 
  
  Ds18B20_Sensors.begin();          // DS18B20 初始化
  DeviceNum = Ds18B20_Sensors.getDeviceCount(); //获取挂载在 OneWire 总线的设备数量
  Serial.print("The number of devices is ");
  Serial.println(DeviceNum);
  Serial.println("初始化！");
  NowTime=LastTime=millis();
//  menu1();
}

void loop()
{
  if (funIndex == 1)      //当显示界面为界面1, 即显示界面为主界面时 
  {
    NowTime=millis();
    if(NowTime - LastTime > 750)   //每750ms更新一次温度
    {
      Ds18B20_Sensors.requestTemperatures(); //向 ds18b20 发起获取温度请求
      NowTime=LastTime=millis();
    }

    //当任何一个传感器所测温度不在设定范围内，把警报标志置 1 
    uint8_t i;
    for(i=0;i<4;i++)
    {
      if(Ds18B20_Data[i]>HighAlarmTemp || Ds18B20_Data[i]<LowAlarmTemp)
      break;
    }
    if(i<4) Alarm_flag=true;
    else Alarm_flag=false;
  }

  if(Alarm_flag)
  {
    Serial.println("There is at least one alarm on the bus.");
    analogWrite(Buzzer_pin, 500);  //蜂鸣器响
    digitalWrite(Led_pin, 1);     //灯亮
  }
  else
  {
     Serial.println("No alarm on the bus.");
     analogWrite(Buzzer_pin, 0);  //蜂鸣器关
     digitalWrite(Led_pin, 0);    //灯灭
  }
  
  (*table[funIndex].operation)();     //执行当前界面对应的函数
//
//  uint8_t buttonState=HIGH;
//  buttonState=digitalRead(button_switch);
//  if(buttonState == LOW)
//  {
//    delay(20);
//    buttonState=digitalRead(button_switch);
//    if(buttonState == LOW)
//    {
//      Serial.print("切换键按下！");
//      if(funIndex==1) funIndex=0;
//      else if(funIndex==0) funIndex=2;
//      else if(funIndex==2) funIndex=1;
//      (*table[funIndex].operation)();
//    }
//  }


  //切换按键检测
  uint8_t buttonState=HIGH;          
  static unsigned char keyUp = 1;
  buttonState=digitalRead(button_switch);
  if(buttonState == LOW && keyUp)
  {
    delay(20);
    buttonState=digitalRead(button_switch);
    if(buttonState == LOW)
    {
      Serial.print("切换键按下！");
      keyUp = 0;
      if(funIndex==1) funIndex=0;
      else if(funIndex==0) funIndex=2;
      else if(funIndex==2) funIndex=1;
      (*table[funIndex].operation)();
    }
  }
  buttonState=digitalRead(button_switch);
  if(buttonState == HIGH)
  keyUp = 1;

  Serial.print("keyup 的值为：");
  Serial.println(keyUp);
  Serial.print("当前界面为：");
  Serial.println(funIndex);
//  delay(100);
}
