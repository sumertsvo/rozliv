/*
  лайт модульный варик на 4 датчика
*/
//Подключаем библиотеки
#include <Q2HX711.h>
#include <Keypad.h>
#include <OLED_I2C.h>
//-------------------------------—
//количество
const byte KOLVO = 4;
//время вертикалки
const word T_VERT = 3000;
//---------------------------------
//пины датчиков
Q2HX711 datch0(A9, A8);
Q2HX711 datch1(A4, A5);
Q2HX711 datch2(A2, A3);
Q2HX711 datch3(A0, A1);
//пины дисплеев (SDA,SCL)
OLED scrn(20, 21);
//пины реле
const byte rele[KOLVO] = {7, 6, 5, 4};
//кнопка
const byte VERT = 41;
const unsigned long T_HEAT = 3000;
//пины клавиатуры
const byte rowPins[4] = {26 , 24, 22, 31};
const byte colPins[4] = {34, 32, 30, 28} ;
//-------------------------------—
//обьявления переменных
byte rez = 0;//0-ожидание,1-налив,2-калибр,3-прогрев
byte stg = 0;
unsigned long starttime;
String str, msg;
int num;
int znach[KOLVO] = {0, 0, 0, 0};
int mas[KOLVO] = {0, 0, 0, 0};
int k[KOLVO] = {0, 0, 0, 0};
boolean on[KOLVO] = {0, 0, 0, 0};
float koef[KOLVO] = {0.8138379, 0.783257, 0.818425, 0.784786};
int nol[KOLVO] = {0, 0, 0, 0};

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

//======================================================================================
//======================================================================================


void setup() {
  //активация дисплеев
  initScreen(scrn);
  //активация реле
  initRele();
  //начальные параметры
  rez = 3;
  starttime = 1;
  //-------------------------------—
  readZnach(datch0, 0);
  readZnach(datch1, 1);
  readZnach(datch2, 2);
  readZnach(datch3, 3);
  nol[0] = znach[0];
  nol[1] = znach[1];
  nol[2] = znach[2];
  nol[3] = znach[3];
}

//======================================================================================

void loop() {
  //переменые
  char key;
  byte q;
  static int m_calibr;
  //считываем значения
  readZnach(datch0, 0);
  readZnach(datch1, 1);
  readZnach(datch2, 2);
  readZnach(datch3, 3);
  switch (rez) {
    //налив
    //==================================================================================
    case 1:
      for (byte i = 0; i < KOLVO; i++) {
        ves(i);
      }
      if (!updateRele()) {
           stopFlow();
          rez = 0;
        }      
     refreshScreen(scrn, num, "Flow");
      break;
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //калибровка
    //==================================================================================
    case 2:
      //считывание клавиатуры
      key = keypad.getKey();
      if (key) {
       switch (key) {
          case 'A':
          stg = 5;
            break;
          case 'B':
            stg++;
            switch (stg) {
              case 0:
                break;
              case 1:
                q = stg - 1;
                mas[q] = znach[q];
                koef[q] = (float(mas[q] - nol[q]) / m_calibr);
                break;

              case 2:
                q = stg - 1;
                mas[q] = znach[q];
                koef[q] = (float(mas[q] - nol[q]) / m_calibr);
                break;
              case 3:
                q = stg - 1;
                mas[q] = znach[q];
                koef[q] = (float(mas[q] - nol[q]) / m_calibr);
                break;
              case 4:
                q = stg - 1;
                mas[q] = znach[q];
                koef[q] = (float(mas[q] - nol[q]) / m_calibr);
                break;
              default:
                rez = 0;
                stg = 0;
                str = '0';
                break;
            }

            break;
          case 'C':
            stg = 0;
                for (byte i = 0; i < KOLVO; i++) {
              mas[i] = 0;
            }
            break;
          case 'D':
            nol[0] = znach[0];
            nol[1] = znach[1];
            nol[2] = znach[2];
            nol[3] = znach[3];
            break;
          case '*':
            stg++;
            break;
          case '#':
            stg--;
            break;
          default:
            str = str + key;
            if (stg == 0) m_calibr = str.toInt();
            break;
        }
      }
      if (stg == 0) {
        refreshScreen(scrn, m_calibr, "Calibr. mas:");
      }
      else {
        refreshScreen(scrn, stg, "Calibr. stage:");
      };
      break;
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //прогрев
    //==================================================================================
    case 3:
      if (millis() < T_HEAT) {
        refreshScreen(scrn, 0, "Heat");
      } else {
        rez = 0;
      }
      break;
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //ожидание
    //==================================================================================
    default:
      //считывание клавиатуры
      key = keypad.getKey();
      if (key) {
               switch (key) {
          case 'A':
            rez = 2;
            stg = 0;
            str = '0';
            for (byte i = 0; i < KOLVO; i++) {
              mas[i] = 0;
            }
            for (byte i = 0; i < KOLVO; i++) {
              nol[i] = znach[i];
            }
            break;

          case 'B':
            startFlow();
            rez = 1;
            break;

          case 'C':
            str = "";
            num = 0;
            for (byte i = 0; i < KOLVO; i++) {
              mas[i] = 0;
              k[i] =  0;
              on[i] = 0;
            }
            break;

          case 'D':
            nol[0] = znach[0];
            nol[1] = znach[1];
            nol[2] = znach[2];
            nol[3] = znach[3];
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
      //обновление экранов
      refreshScreen(scrn, num, "Wait");
      //-------------------------------—
  }
}

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
  pinMode(VERT, OUTPUT);
  digitalWrite(VERT, 1);
  for (byte i = 0; i < KOLVO; i++) {
    pinMode(rele[i], OUTPUT);
    digitalWrite(rele[i], 1);
  }
}

//======================================================================================


//процедура переключения реле
boolean updateRele() {  
  int delta[KOLVO] ;
  boolean buf = false;
   for (byte i = 0; i < KOLVO; i++) {
    delta[i] = num - mas[i];
    if (delta[i] <= 0)  on[i] = 0;
    digitalWrite(rele[i], !on[i]);
    buf += on[i];
  }
  return buf;
}



//======================================================================================

//процедура пуска
void startFlow() {
  for (byte i = 0; i < KOLVO; i++) {
    mas[i] = 0;
    k[i] =  0;
    on[i] = 1;
  }
  digitalWrite(VERT, 0);
  delay(T_VERT);//вертикальная задержка
  //сброс тары
  k[0] = znach[0] - nol[0];
  k[1] = znach[1] - nol[1];
  k[2] = znach[2] - nol[2];
  k[3] = znach[3] - nol[3];
  for (byte i = 0; i < KOLVO; i++) {
    digitalWrite(rele[i], 0);
  }
}

//======================================================================================

//процедура завершения
void stopFlow() {
  digitalWrite(VERT, 1);
  delay(T_VERT);
}

//======================================================================================
//процедура просчёта релешек
void ves(int i) {
  mas[i] = (znach[i] - (nol[i] + k[i])) / koef[i]; //в конце нужный коэф
}

//======================================================================================
//процедура просчёта релешек
void readZnach(Q2HX711 & datch, int i) {
  znach[i] = (int(datch.read() / 1000));
}