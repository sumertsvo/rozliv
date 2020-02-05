/*
  модульный варик на 4 датчика
*/
//Подключаем библиотеки
#include <Q2HX711.h>
#include <Keypad.h>
#include <OLED_I2C.h>
//-------------------------------—

//разница массы насоса
const int DM_NASOS = 100;
//разница массы насоса
const int M_CALIBR = 1000;
//количество
const byte KOLVO = 4;
//включение отладки
boolean TEST = true;

//время вертикалки
const word T_VERT = 3000;
//время горизонталки
const word T_GORIZ = 3000;
//---------------------------------

//пины датчиков
Q2HX711 datch0(A7, A6);
Q2HX711 datch1(A5, A4);
Q2HX711 datch2(A3, A2);
Q2HX711 datch3(A1, A0);
//пины дисплеев (SDA,SCL)
OLED scrn(20, 21);
//пины реле
const byte rele[KOLVO] = {41, 43, 45, 47};
//кнопка
const byte KNOP = 52;
const byte GORIZ = 49;
const byte VERT = 51;
const byte NASOS = 53;
const unsigned long T_HEAT = 60000;
//пины клавиатуры
const byte rowPins[4] = {7 , 6, 5, 4};
const byte colPins[4] = {11, 10, 9, 8} ;
//-------------------------------—

//обьявления переменных
byte rez = 0;//0-ожидание,1-налив,2-калибр,3-прогрев
byte stg = 0;//0-нет,1-пусто,2-контрольное
unsigned long starttime;


String str, msg;
int num;
int mas[KOLVO] = {0, 0, 0, 0};
int k[KOLVO] = {0, 0, 0, 0};
boolean on[KOLVO] = {0, 0, 0, 0};
//коэф расчета
float koef[KOLVO] = {0.8138379, 0.783257, 0.818425, 0.784786};
//коэффициенты 0
int nol[KOLVO] = {8930, 9158, 8780, 8950};

//-------------------------------—

//ненужные настройки
extern uint8_t BigNumbers[];
extern uint8_t SmallFont[];
char keys[4][4] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, 4, 4 );
//---------------------------------------—


//======================================================================================
//======================================================================================
//======================================================================================


void setup() {
  //кнопка вкл
  pinMode(KNOP, INPUT);
  //активация дисплеев
  initScreen(scrn);
  //активация реле
  initRele();
  //начальные параметры
  rez = 3;
  starttime = 0;
  //-------------------------------—
  //отладочное
  if (TEST) {
    Serial.begin(9600);
  }
  //-------------------------------—
}


//======================================================================================

void loop() {
  static boolean flag;
  char key;
  static int mc1, mc2;
  switch (rez) {

    //налив
    case 1:
      ves(datch0, 0);
      ves(datch1, 1);
      ves(datch2, 2);
      ves(datch3, 3);
      if (dm >= DM_NASOS) {
        if (updateRele) {
          if (TEST) Serial.println("работает");
        }
        else {
          stopFlow;
          rez = 0;
        }
      } else {
        digitalWrite(NASOS, 0);
        if (TEST) Serial.println("насос вкл");
      }
      refreshScreen(scrn, num, "Flow");
      break;



    //калибровка
    case 2:
      //кнопка
      if (digitalRead(KNOP) == HIGH && flag == 0) //если кнопка нажата
      {
        flag = 1;

        switch (stg) {

          case 1:
            ves(datch0, 0);
            mc1 = mas[0];
            break;

          case 2:
            ves(datch0, 0);
            mc2 = mas[0];
            koef[0] = mc2 - mc1 / M_CALIBR;
            break;

          case 3:
            ves(datch1, 1);
            mc1 = mas[1];
            break;

          case 4:
            ves(datch1, 1);
            mc2 = mas[1];
            koef[1] = mc2 - mc1 / M_CALIBR;
            break;

          case 5:
            ves(datch2, 2);
            mc1 = mas[2];
            break;

          case 6:
            ves(datch2, 2);
            mc2 = mas[2];
            koef[2] = mc2 - mc1 / M_CALIBR;
            break;

          case 7:
            ves(datch3, 3);
            mc1 = mas[3];
            break;

          case 8:
            ves(datch3, 3);
            mc2 = mas[3];
            koef[3] = mc2 - mc1 / M_CALIBR;
            break;

          default:
            rez = 0;
            break;
        }

      }
      if (digitalRead(KNOP) == LOW && flag == 1) //если кнопка отжата
      {
        flag = 0; //обнуляем переменную flag
      }

      refreshScreen(scrn, stg, "Calibr. stage:");
      break;


    //прогрев
    case 3:
      if (millis < starttime + T_HEAT) {
        ves(datch0, 0);
        ves(datch1, 1);
        ves(datch2, 2);
        ves(datch3, 3);
        refreshScreen(scrn, 0, "Heat");
      }
      break;

    //ожидание
    default:
      //кнопка
      if (digitalRead(KNOP) == HIGH && flag == 0) //если кнопка нажата
      {
        flag = 1;
        // защита от "дребезга" 100%
        startFlow;
        rez = 1;
      }
      if (digitalRead(KNOP) == LOW && flag == 1) //если кнопка отжата
      {
        flag = 0; //обнуляем переменную flag
      }
      //==========================================================================

      //считывание клавиатуры
      key = keypad.getKey();
      if (key) {
        if (TEST) Serial.println(key); // Передаем нажатую клавишу в сериал порт (отладочное)
        switch (key) {
          case 'A':
            for (byte i = 0; i < 4; i++) {
              on[i] = !on[i];
            }
            delay(100);
            break;
          case 'B':

            break;
          case 'C':
            str = "";
            num = 0;
            for (byte i = 0; i < 4; i++) {
              mas[i] = 0;
              k[i] =  0;
              on[i] = 0;
            }
            break;
          case 'D':
            k[0] = int(datch0.read() / 1000) - nol[0];
            k[1] = int(datch1.read() / 1000) - nol[1];
            k[2] = int(datch2.read() / 1000) - nol[2];
            k[3] = int(datch3.read() / 1000) - nol[3];
            break;
          case '*':
            break;
          case '#':
            break;
          default:
            str = str + key;
            num = str.toInt();
            break;
        }
      }
      //-------------------------------—
      if (TEST)Serial.println(' ');
      //обновление экранов
      refreshScreen(scrn, num, "Wait");
      //-------------------------------—
  }
}



//======================================================================================
//======================================================================================
//======================================================================================


//======================================================================================

//процедура обновления экранов
void refreshScreen(OLED & myOLED, int i, String s) {
  myOLED.clrScr();
  myOLED.setFont (BigNumbers);
  myOLED.printNumI(i, CENTER, 30);
  myOLED.setFont (SmallFont);
  myOLED.print(s, CENTER, 3);
  myOLED.update();
}

//======================================================================================

//процедура активации экранов
void initScreen(OLED & myOLED) {
  myOLED.begin();
  myOLED.clrScr();
  myOLED.setFont(BigNumbers);
}

//======================================================================================

//процедура активации реле
void initRele() {
  for (byte i = 0; i < KOLVO; i++) {
    pinMode(rele[i], OUTPUT);
    digitalWrite(rele[i], 1);
  }
}

//======================================================================================

//процедура переключения реле
boolean updateRele() {
  boolean buf = false;
  for (byte i = 0; i < KOLVO; i++) {
    digitalWrite(rele[i], !on[i]);
    buf += on[i];
  }
  return buf;
}

//======================================================================================

//процедура для насоса
int dm() {
  static int delta[KOLVO] = {0, 0, 0, 0};
  int dx = 0;
  for (byte i = 0; i < KOLVO; i++) {
    delta[i] = num - mas[i];
    if (delta[i] <= 0)  on[i] = 0;
  }
  for (byte i = 0; i < KOLVO; i++) {
    dx += delta[i];
  }
  return dx;
}

//======================================================================================

//процедура пуска
void startFlow() {
  for (byte i = 0; i < 4; i++) {
    mas[i] = 0;
    k[i] =  0;
    on[i] = 0;
  }
  digitalWrite(GORIZ, 0);
  delay(T_GORIZ);//горизонтальная задержка
  if (TEST) Serial.println("горизонт на месте");
  digitalWrite(VERT, 0);
  delay(T_VERT);//вертикальная задержка
  if (TEST) Serial.println("вертикаль на месте");
  //сброс веса
  k[0] = int(datch0.read() / 1000) - nol[0];
  k[1] = int(datch1.read() / 1000) - nol[1];
  k[2] = int(datch2.read() / 1000) - nol[2];
  k[3] = int(datch3.read() / 1000) - nol[3];
  if (TEST) Serial.println("тара сброшена");
}

//======================================================================================

//процедура завершения
void stopFlow() {
  digitalWrite(NASOS, 1);
  if (TEST)   Serial.println("насос выкл");
  digitalWrite(VERT, 1);
  delay(T_VERT);
  if (TEST)   Serial.println("вертикаль убрана");
  digitalWrite(GORIZ, 1);
  delay(T_GORIZ);
  if (TEST)   Serial.println("горизонт убран");
}

//======================================================================================
//процедура просчёта релешек
void ves(Q2HX711 & datch, int i) {
  mas[i] = (int(datch.read() / 1000) - (nol[i] + k[i])) / koef[i]; //в конце нужный коэф
  if (TEST) {
    Serial.print("показания датчика=");
    Serial.println(int(datch.read() / 1000));
    Serial.print("масса=");
    Serial.println( mas[i]);//масса с учетом тары и т.п.
    Serial.print("заданный вес=");
    Serial.println( num);
    Serial.print("нулевая отметка=");
    Serial.println(nol[i]);
    Serial.print("добавка тары=");
    Serial.println(k[i]);
    Serial.print("коэфф.=");
    Serial.println(koef[i]);
    Serial.println( );
  }
}