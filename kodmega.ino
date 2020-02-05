//Подключаем библиотеки
#include <Q2HX711.h>
#include <Keypad.h>
#include <OLED_I2C.h>
//количество
const byte kolvo = 7;
const int goriz = 3000;//время горизонталки
const int vert = 3000;//время вертикалки
//-------------------------------—
//пины датчиков
Q2HX711 hx7110(A0, A1);
Q2HX711 hx7111(A2, A3);
Q2HX711 hx7112(A4, A5);
Q2HX711 hx7113(A6, A7);
//-------------------------------—
//пины дисплеев (SDA,SCL)
OLED myOLED0(20, 21);
//-------------------------------—
//пины реле
byte rele[kolvo] = {41, 43, 45, 47, 49, 51, 53};
const byte knop = 52;
const byte resetPin = 13;
//-------------------------------—
//обьявления переменных
String str;
int num,q;
int mas[4];
int k[4];
int koef[4]= {0.81, 0.81, 0.81, 0.81};//коэф расчета
int nol[4] = {8260, 8260, 8260, 8260};//коэффициенты 0
boolean on[4] = {0, 0, 0, 0};
boolean flag;
//-------------------------------—
//ненужные настройки

extern uint8_t BigNumbers[];
const byte ROWS = 4; // 4 строки
const byte COLS = 4; // 4 столбца
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
//-------------------------------—

//пины клавиатуры
byte rowPins[ROWS] = {7 , 6, 5, 4};
byte colPins[COLS] = {11, 10, 9, 8} ;
//-------------------------------—

//ненужные настройки
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
//---------------------------------------—


//======================================================================================
//======================================================================================
//======================================================================================


void setup() {
  pinMode(knop, INPUT);
  //активация дисплеев
  screen_activate(myOLED0);
  //-------------------------------—
  //активация реле
  rele_activate();
  //-------------------------------—
  //отладочное
  Serial.begin(9600);
  //-------------------------------—
}


//======================================================================================
//======================================================================================
//======================================================================================

void loop() {
  //кнопка
  if (digitalRead(knop) == HIGH && flag == 0) //если кнопка нажата
    // и перемення flag равна 0 , то ...
  {
    poehali();
    flag = 1;
    //это нужно для того что бы с каждым нажатием кнопки
    //происходило только одно действие
    // плюс защита от "дребезга"  100%
  }
  if (digitalRead(knop) == LOW && flag == 1) //если кнопка НЕ нажата
    //и переменная flag равна - 1 ,то ...
  {
    flag = 0; //обнуляем переменную flag
  }
  //==========================================================================

  //считывание клавиатуры
  char key = keypad.getKey();
  if (key) {
    // Serial.println(key); // Передаем нажатую клавишу в сериал порт (отладочное)
    switch (key) {
      case 'A':
        /*  for (byte i = 0; i < 4; i++) {
            on[i] = !on[i];
          }
          delay(100);*/
        break;
      case 'B':
        test(myOLED0);
        break;
      case 'C':
        str = "";
        num = 0;
        for (byte i = 0; i < 4; i++) {
          k[i] = 0;
        }
        break;
      case 'D':
        /*  k[0] = int(hx7110.read() / 1000) - nol[0];
          k[1] = int(hx7111.read() / 1000) - nol[1];
          k[2] = int(hx7112.read() / 1000) - nol[2];
          k[3] = int(hx7113.read() / 1000) - nol[3];*/
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
  //Serial.println(' ');
  //обновление экранов
  screen_refresh(myOLED0, num);
  //-------------------------------—
}

//======================================================================================
//======================================================================================
//======================================================================================

//процедура налива
void poehali()
{
  digitalWrite(rele[4], 0);
  delay(goriz);//горизонтальная задержка
      Serial.println("горизонт на месте");
  digitalWrite(rele[5], 0);
  delay(vert);//вертикальная задержка
   Serial.println("вертикаль  на месте");
  //сброс веса
  k[0] = int(hx7110.read() / 1000) - nol[0];
  k[1] = int(hx7111.read() / 1000) - nol[1];
  k[2] = int(hx7112.read() / 1000) - nol[2];
  k[3] = int(hx7113.read() / 1000) - nol[3];
   Serial.println("тара сброшена");
  delay(100);

  //обработка релешек
  digitalWrite(rele[0], 0);
  digitalWrite(rele[1], 0);
  digitalWrite(rele[2], 0);
  digitalWrite(rele[3], 0);
 
  
  while(1)
  {
   on[0]= ves(hx7110, 0);
    on[1]=  ves(hx7111, 1);
     on[2]= ves(hx7112, 2);
     on[3]= ves(hx7113, 3);
  if (on[0] && on[1] && on[2] && on[3])
{
      digitalWrite(rele[6], 0);
      Serial.println("работает");
    }
    else 
    {
      digitalWrite(rele[6], 1);
      Serial.println("готово");
      break;
    }
    q=digitalRead(rele[6]);
    Serial.println(q);
  }
    Serial.println("залито");
delay(2000);
  digitalWrite(rele[5], 1);
  delay(vert);
   Serial.println("вертикаль убрана");
  digitalWrite(rele[4], 1);
  delay(goriz);
   Serial.println("горизонт убран");
}

//======================================================================================

//процедура обновления экранов
void screen_refresh(OLED & myOLED, int i)
{
  myOLED.clrScr();
  myOLED.printNumI(i, CENTER, 30);
  myOLED.update();
}

//======================================================================================

//процедура активации экранов
void screen_activate(OLED & myOLED)
{
  myOLED.begin();
  myOLED.setFont(BigNumbers);
}

//======================================================================================

//процедура активации реле
void rele_activate()
{
  for (byte i = 0; i < kolvo; i++) {
    pinMode(rele[i], OUTPUT);
    digitalWrite(rele[i], 1);
  }
}

//======================================================================================

//процедура проверки
void test(OLED & myOLED)
{
  screen_refresh(myOLED, 8888);
  for (byte i = 0; i < kolvo; i++) {
    delay(1000);
    digitalWrite(rele[i], 1);
    delay(1000);
    digitalWrite(rele[i], 0);
  }

  for (byte i = 0; i < kolvo; i++) {
    screen_refresh(myOLED, 1111);
    screen_refresh(myOLED, 8888);
    screen_refresh(myOLED, 1111);
    screen_refresh(myOLED, 8888);
    digitalWrite(rele[i], 1);
    screen_refresh(myOLED, 1111);
    screen_refresh(myOLED, 8888);
    screen_refresh(myOLED, 1111);
    screen_refresh(myOLED, 8888);
    digitalWrite(rele[i], 0);
  }
  delay(3000);
  for (byte i = 0; i < kolvo; i++) {
    delay(5000);
    digitalWrite(rele[i], 1);
    delay(5000);
    digitalWrite(rele[i], 0);
  }
  screen_refresh(myOLED, 0);
  for (byte i = 0; i < kolvo; i++) {
    digitalWrite(rele[i], 1);
  }
}

//======================================================================================
//процедура просчёта релешек
boolean ves(Q2HX711 & hx711, int i)
{
  //   Serial.println(int(hx711.read()/1000));
  mas[i] = (int(hx711.read() / 1000) - (nol[i] + k[i])) / koef[i]; //в конце нужный коэффф
  // Serial.println(  mas[i]);
  //для самих релешек
  if ((num > mas[i]))
  { //если масса меньше
    digitalWrite(rele[i], 0);
  }
  else
  { //если масса больше
    digitalWrite(rele[i], 1);
    if (mas[i] > 10)
    {
      on[i] = false;
    }
  }
  //для общего насоса
  if ((num > (mas[i] + 100)))//МАССА ТУТА
  { //если масса меньше
    return false;
  }
  else
  { //если масса больше
    return true;
  }
  //-------------------------------—
}

