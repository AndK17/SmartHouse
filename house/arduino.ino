#include<math.h>
#include <DHT.h>
#include <DHT_U.h>
#include "DHT.h"
#define DHTPIN  8
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
// подключение dht11

#include <TroykaLight.h>
TroykaLight sensorLight(A0);
// подключение сенсора света


#define redPin  9
#define greenPin  10
#define bluePin  6
// пины для ргб-светодиода

#define ventPin 5
#define heatPin 12
// пины для индикации отопления и вентиляции

#define buzzPin 7
// пин для пьезопищалки

#include <Multiservo.h>
Multiservo multiservo;
constexpr uint8_t MULTI_SERVO_PIN = 11;
// подключение сервопривода

#define close_angle 125
#define open_angle 70
// угол открытия и закрытия двери

int hum;
int temp;
float light;
// переменные для показаний датчиков

int fire = 0; // переменная состояния пожара
int max_temp =40; // температура срабатывания пожарной сигнализации

float red; 
float blue;
float green;
float brightness;
// переменные для освещения

int k1 = 0;
int k2 = 0;
bool flag1 = false;
bool flag2 = false;

int code1[3] = {500, 1500, 1500}; // код для датчика вибрации
int code2[3] = {500, 1500, 1500}; // код для кнопки

int diff = 1000; // погрешность между срабатываниями датчиков

int start_time1[4];
int start_time2[4];
bool opened = false;
long long int open_time = 0;

bool automatic = false;
bool heating = false;
bool vent = false;

// функция для разделения строки
String getPart(String text, char del, int index)
{
  int found = 0;
  int sInd[] = {0, -1};
  int mInd = text.length() - 1;

  for (int i = 0; i <= mInd && found <= index; i++)
  {
    if (text.charAt(i) == del || i == mInd)
    {
      found++;
      sInd[0] = sInd[1] + 1;
      sInd[1] = (i == mInd) ? i + 1 : i;
    }
  }

  return found > index ? text.substring(sInd[0], sInd[1]) : "";
}



void setup()
{
  Serial.begin(9600); // подключения Serial-порта
  dht.begin();
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(heatPin, OUTPUT);
  pinMode(ventPin, OUTPUT);
  pinMode(buzzPin, OUTPUT);
  pinMode(13, OUTPUT);
  multiservo.attach(MULTI_SERVO_PIN);
  // иницилизация всех пинов
  
  attachInterrupt(INT0, button1, FALLING); // вызов функции при срабатывании датчика вибрации
  attachInterrupt(INT1, button2, FALLING); // вызов функции при срабатывании кнопки
  
  multiservo.write(close_angle); // закрытие двери при запуске системы
}

// функция проверки введенного пароля для датчика вибрации
void button1()
{
  
  if (not(opened))
  {
    tone(buzzPin, 2000, 50);
    start_time1[k1] = millis();
    if (k1 > 0 and start_time1[k1] - start_time1[k1 - 1] > 5000)
    {
      start_time1[0] = start_time1[k1];
      k1 = 0;
    }
    k1++;
    k2 = 0;
    if (k1 == 4)
    {
      flag1 = true;
      k1 = 0;
      k2 = 0;

      for (int i = 0; i < 3; i++)
      {
        int tmp = start_time1[i + 1] - start_time1[i];
        if (not(code1[i] - diff < tmp and tmp < code1[i] + diff))
        {
          flag1 = false;
          flag2 = false;
          multiservo.write(close_angle);
          tone(buzzPin, 440, 100);
          break;
        }
      }
    }
    if (k1 > 4)
    {
      k1 = 0;
    }
  }
}

// функция проверки введенного пароля для кнопки
void button2()
{
  if (not(opened))
  {
    tone(buzzPin, 2000, 50);
    start_time2[k2] = millis();
    if (k2 > 0)
    {
      if (start_time2[k2] - start_time2[k2 - 1] > 5000)
      {
        start_time2[0] = start_time2[k2];
        k2 = 1;
      }
    }
    k2++;
    k1 = 0;
    if (k2 == 4)
    {
      flag2 = true;
      k1 = 0;
      k2 = 0;
      for (int i = 0; i < 3; i++)
      {
        int tmp = start_time2[i + 1] - start_time2[i];
        if (not(code2[i] - diff < tmp and tmp < code2[i] + diff))
        {
          flag1 = false;
          flag2 = false;
          multiservo.write(close_angle);
          tone(buzzPin, 440, 100);
          break;
        }

      }
    }
    if (k2 > 4)
    {
      k1 = 0;
      k2 = 0;
    }
  }
}

void loop() {
  if (flag1 and flag2 and not(opened)) // открытие двери
  {
    multiservo.write(open_angle);
    flag1 = false;
    flag2 = false;
    opened = true;
    open_time = millis();
  }
  
  if (opened and millis() - open_time >= 15000 and opened and millis() - open_time <= 17000) // закрытие двери по истечении таймера
  {
    multiservo.write(close_angle);
    //Serial.println("closed");
    k1 = 0;
    k2 = 0;
    opened = false;
    open_time = millis();
  }
  
  // снятие показаний с датчиков
  hum = dht.readHumidity();
  temp = dht.readTemperature();
  light = sensorLight.getLightLux() / 5;
  fire = 0;
  if (temp > max_temp) fire = 1;
  if (light > 100) light = 100;
  String a = String(String(temp) + " " + String(hum) + " " + String(light) + " " + String(fire));
  sensorLight.read();
  if (millis()%500 >= 0 and millis()%500 <= 50) Serial.println(a);
  if (automatic)
  {
    brightness = (100 - light) / 100;
  }
  
  // сигнализация
  if (fire)
  {
    tone(buzzPin, 700, 5);
  }
  // управление освещением
  analogWrite(redPin, red * brightness);
  analogWrite(greenPin, green * brightness);
  analogWrite(bluePin, blue * brightness);
  
  digitalWrite(heatPin, heating);
  digitalWrite(ventPin, vent);
  while (Serial.available())
  {
    String data = Serial.readStringUntil('\n');
    if (data != " ")
    {
      red = getPart(data, '/', 1).toInt();
      green = getPart(data, '/', 2).toInt();
      blue = getPart(data, '/', 3).toInt();
      brightness = getPart(data, '/', 4).toInt();
      if (brightness == -1)
      {
        automatic = true;
      }
      else
      {
        brightness = brightness / 100;
        automatic = false;
      }
      heating = getPart(data, '/', 5).toInt();
      vent = getPart(data, '/', 6).toInt();
      
    }
  }
}
