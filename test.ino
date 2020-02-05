/*
  ��������� ����� �� 4 �������
*/
//���������� ����������
#include <Q2HX711.h>
#include <Keypad.h>
#include <OLED_I2C.h>
//-------------------------------�

//������� ����� ������
const int DM_NASOS = 1;
//����������
const byte KOLVO = 4;

//��������� �������
boolean TEST = true;//����� ����
boolean KTEST = true;//���� ������
boolean VTEST = true;//���� ��������� ves
boolean STEST = true;//���� ����� ��������
boolean CTEST = true;//���� ����������
boolean NTEST = true;//���� ������
boolean HTEST = true;//���� ��������

//����� ����������
const word T_VERT = 3000;
//����� ������������
const word T_GORIZ = 1;
//---------------------------------

//���� ��������
Q2HX711 datch0(A9, A8);
Q2HX711 datch1(A4, A5);
Q2HX711 datch2(A2, A3);
Q2HX711 datch3(A0, A1);
//���� �������� (SDA,SCL)
OLED scrn(20, 21);
//���� ����
const byte rele[KOLVO] = {7, 6, 5, 4};
//������
const byte GORIZ = 39;
const byte VERT = 41;
const byte NASOS = 43;
const unsigned long T_HEAT = 3000;
//���� ����������
const byte rowPins[4] = {26 , 24, 22, 31};
const byte colPins[4] = {34, 32, 30, 28} ;
//-------------------------------�

//���������� ����������
byte rez = 0;//0-��������,1-�����,2-������,3-�������
byte stg = 0;//
unsigned long starttime;


String str, msg;
int num;
int znach[KOLVO] = {0, 0, 0, 0};
int mas[KOLVO] = {0, 0, 0, 0};
int k[KOLVO] = {0, 0, 0, 0};
boolean on[KOLVO] = {0, 0, 0, 0};
//���� �������
float koef[KOLVO] = {0.8138379, 0.783257, 0.818425, 0.784786};
//������������ 0
int nol[KOLVO] = {0, 0, 0, 0};

//-------------------------------�

//�������� ���������
extern uint8_t BigNumbers[];
extern uint8_t SmallFont[];
char keys[4][4] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, 4, 4 );
//---------------------------------------�


//======================================================================================
//======================================================================================
//======================================================================================


void setup() {
  //����������
  Serial.begin(9600);
  //-------------------------------�
  if (TEST) Serial.print("start");
  //��������� ��������
  initScreen(scrn);
  //��������� ����
  initRele();
  //��������� ���������
  rez = 3;
  starttime = 1;
  //-------------------------------�
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
  //���������
  char key;
  byte q;
  static int m_calibr;
  //��������� ��������
  readZnach(datch0, 0);
  readZnach(datch1, 1);
  readZnach(datch2, 2);
  readZnach(datch3, 3);
  if (TEST) Serial.print("rez=");
  if (TEST) Serial.println(rez);
  switch (rez) {
    //�����
    //==================================================================================
    case 1:
      for (byte i = 0; i < KOLVO; i++) {
        ves(i);
      }
      if (updateRele()) {
          if (NTEST) Serial.println("flow");
        }
        else {
          if (NTEST) Serial.println("stop flow");
          stopFlow();
          rez = 0;
        }
      
      if (NTEST) {
        for (byte i = 0; i < KOLVO; i++) {
          Serial.print("rel_");
          Serial.println(i + 1);
          if (on[i])
            Serial.println("on");
          else Serial.println("off");
        }
      }
      refreshScreen(scrn, num, "Flow");
      break;
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //����������
    //==================================================================================
    case 2:
      //���������� ����������
      key = keypad.getKey();
      if (key) {
        if (KTEST) Serial.println(key); // �������� ������� ������� � ������ ���� (����������)
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
      if (CTEST) {
        Serial.print("m_calibr=");
        Serial.println(m_calibr);
        Serial.print("mas1=");
        Serial.println(mas[0]);
        Serial.print("nol1=");
        Serial.println(nol[0]);
        Serial.print("koef1=");
        Serial.println(koef[0]);
        Serial.print("mas2=");
        Serial.println(mas[1]);
        Serial.print("nol2=");
        Serial.println(nol[1]);
        Serial.print("koef2=");
        Serial.println(koef[1]);
        Serial.print("mas3=");
        Serial.println(mas[2]);
        Serial.print("nol3=");
        Serial.println(nol[2]);
        Serial.print("koef3=");
        Serial.println(koef[2]);
        Serial.print("mas4=");
        Serial.println(mas[3]);
        Serial.print("nol4=");
        Serial.println(nol[3]);
        Serial.print("koef4=");
        Serial.println(koef[3]);
      }
      if (stg == 0) {
        refreshScreen(scrn, m_calibr, "Calibr. mas:");
      }
      else {
        refreshScreen(scrn, stg, "Calibr. stage:");
      };
      break;
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //�������
    //==================================================================================
    case 3:
      if (HTEST) Serial.println(millis());
      if (millis() < T_HEAT) {
        refreshScreen(scrn, 0, "Heat");
      } else {
        rez = 0;
      }
      break;
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //��������
    //==================================================================================
    default:
      //���������� ����������
      key = keypad.getKey();
      if (key) {
        if (KTEST) Serial.println(key); // �������� ������� ������� � ������ ���� (����������)
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
      //-------------------------------�
      if (TEST)Serial.println(' ');
      //���������� �������
      refreshScreen(scrn, num, "Wait");
      //-------------------------------�
  }
}



//======================================================================================
//======================================================================================
//======================================================================================


//======================================================================================

//��������� ���������� �������
void refreshScreen(OLED & myOLED, int i, String s) {
  if (TEST) Serial.print("refreshScreen");
  myOLED.clrScr();
  myOLED.setFont (BigNumbers);
  myOLED.printNumI(i, CENTER, 30);
  myOLED.setFont (SmallFont);
  myOLED.print(s, CENTER, 3);
  myOLED.update();
}

//======================================================================================

//��������� ��������� �������
void initScreen(OLED & myOLED) {
  if (TEST) Serial.print("initScreen");
  myOLED.begin();
  myOLED.clrScr();
  myOLED.setFont(BigNumbers);
}

//======================================================================================

//��������� ��������� ����
void initRele() {
  if (TEST) Serial.print("initRele");
  pinMode(GORIZ, OUTPUT);
  digitalWrite(GORIZ, 1);
  pinMode(VERT, OUTPUT);
  digitalWrite(VERT, 1);
  pinMode(NASOS, OUTPUT);
  digitalWrite(NASOS, 1);
  for (byte i = 0; i < KOLVO; i++) {
    pinMode(rele[i], OUTPUT);
    digitalWrite(rele[i], 1);
  }
}

//======================================================================================


//��������� ������������ ����
boolean updateRele() {  
  if (TEST) Serial.print("updateRele");
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

//��������� �����
void startFlow() {
  if (TEST) Serial.print("startFlow");
  for (byte i = 0; i < KOLVO; i++) {
    mas[i] = 0;
    k[i] =  0;
    on[i] = 1;
  }
  digitalWrite(GORIZ, 0);
  delay(T_GORIZ);//�������������� ��������
  if (NTEST) Serial.println("gorizont na meste");
  digitalWrite(VERT, 0);
  delay(T_VERT);//������������ ��������
  if (NTEST) Serial.println("vertical na meste");
  //����� ����
  k[0] = znach[0] - nol[0];
  k[1] = znach[1] - nol[1];
  k[2] = znach[2] - nol[2];
  k[3] = znach[3] - nol[3];
  if (NTEST) Serial.println("tara sbroshena");
  for (byte i = 0; i < KOLVO; i++) {
    digitalWrite(rele[i], 0);
  }
  if (NTEST) Serial.println("rele vkluch");
}

//======================================================================================

//��������� ����������
void stopFlow() {
  if (TEST) Serial.print("stopFlow");
  digitalWrite(NASOS, 1);
  if (NTEST)   Serial.println("nasos vikl");
  digitalWrite(VERT, 1);
  delay(T_VERT);
  if (NTEST)   Serial.println("vertical ubrana");
  digitalWrite(GORIZ, 1);
  delay(T_GORIZ);
  if (NTEST)   Serial.println("gorizont ubran");
}

//======================================================================================
//��������� �������� �������
void ves(int i) {
  mas[i] = (znach[i] - (nol[i] + k[i])) / koef[i]; //� ����� ������ ����
  if (VTEST) {
    Serial.print("pokazaniya=");
    Serial.println(znach[i]);
    Serial.print("mass=");
    Serial.println(mas[i]);//����� � ������ ���� � �.�.
    Serial.print("zadanniy ves=");
    Serial.println(num);
    Serial.print("nulevaya otmetka=");
    Serial.println(nol[i]);
    Serial.print("tara=");
    Serial.println(k[i]);
    Serial.print("koeff.=");
    Serial.println(koef[i]);
    if (on[i]) Serial.print("=on");
    else Serial.print("=off");
    Serial.println( );
  }
}

//======================================================================================
//��������� �������� �������
void readZnach(Q2HX711 & datch, int i) {
  znach[i] = (int(datch.read() / 1000));
  if (STEST) {
    Serial.print("pokazanie_");
    Serial.println(i + 1);
    Serial.println(znach[i]);
  }
}