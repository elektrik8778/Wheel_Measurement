#include "GyverTM1637.h"

#define CLK 6
#define DIO 5

GyverTM1637 disp(CLK, DIO);

unsigned long lastturn;  //переменные хранения времени
byte butt_gnd = 9;                  //земля кнопки
byte butt_pin = 10;                 //состояние кнопки

byte sens_vcc = 2;                  //питание датчика
byte sens_pin = 3;                  //выход датчика (принимает значение от 1 до 0)

volatile float DIST = 0.0;          //переменная хранения расстояния в виде десятичной дроби
volatile float w_length = 0.48;      //длина окружности колеса в метрах
byte n = 120;                        //число отверстий / магнитов
volatile boolean flag;              //флаг вывода на дисплей
boolean flag_over = false;
volatile int count = 0;
int cel=0, drb=0;


void setup() {
  disp.clear();
  disp.brightness(3);  // яркость, 0 - 7 (минимум - максимум)
  runningText();
  
  attachInterrupt(1, gap, FALLING);  //подключить прерывание на 3 пин при повышении сигнала
  Serial.begin(9600);
  pinMode(butt_gnd, OUTPUT);        //пин как выход
  pinMode(sens_vcc, OUTPUT);        //пин как выход
  pinMode(butt_pin, INPUT_PULLUP);  //пин кнопки подтягиваем

  //подаём напряжение на пины
  digitalWrite(butt_gnd, 0);
  digitalWrite(sens_vcc, 1);
  disp.displayClock(cel, drb);
  disp.point(true);

  //--------------
  w_length = (float)w_length / n;  //длина дуги колеса с учетом числа отверстий
}


void gap() {
    DIST = DIST + w_length; //прибавляем длину колеса к дистанции при каждом обороте оного
    count++;
    flag = 1; //вывести на дисплей
}


void loop() {
  
  if (flag) {
    cel = (int)DIST;
    drb = round(DIST*100)%100;
    if (cel==100 && !flag_over){
      cel = 0;
      drb = 0;
      flag_over = true;
    }
    disp.displayClock(cel, drb);
    flag = 0;
  }

  if (!digitalRead(butt_pin)) {       //если кнопка нажата
    DIST = 0;                         //обнулить расстояние
    count = 0;                        //обнулить счет импульсов
    flag = 1;                         //вывести на дисплей
    cel = 0;
    drb = 0;
  }
}

void runningText() {
  byte welcome_banner[] = { _H, _E, _L, _L, _O, _empty, _empty };
  disp.runningString(welcome_banner, sizeof(welcome_banner), 100);  // 200 это время в миллисекундах!
}
