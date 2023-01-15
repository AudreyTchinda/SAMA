
#include <GPRS_Shield_Arduino.h>
#include <SoftwareSerial.h>
#include <MFRC522.h>
#include <Wire.h>
#include <SPI.h>
int donn;
#define send 11
#define PIN_TX 8
#define PIN_RX 9
#define BAUDRATE 9600
SoftwareSerial portOne(2, 3);
GPRS gprs(PIN_TX, PIN_RX, BAUDRATE);  //RX,TX,BaudRate
boolean suiva;
char numero[10];
int res, p, y, j;
int z;
String Message1, Message2;
String H, min, mi;
String Numero, Nom, Classe, situation, Heure, receive;
void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  gprs.checkPowerUp();
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }
  pinMode(send, OUTPUT);
  // Start each software serial port
  portOne.begin(9600);
  //digitalWrite(11, 1);
  delay(10000);
  portOne.print("fine");
}

void loop() {
  digitalWrite(11, 1);
  portOne.listen();
  if (res == 0) {
    delay(100);
    digitalWrite(send, 1);
    res = 3;
  }
  if (res == 3) {
    while (portOne.available()) {
      digitalWrite(send, 0);
      String receive = portOne.readString();
      Serial.println(receive);
      int num_char = receive.length();
      if ((num_char > 4) and (num_char < 13)) {
        if (receive.indexOf("send?") >= 0) {
          portOne.print("send");
          // Serial.println("reception accordé!!");
          p = 1;
        }
      } else if (p == 1) {
        p = 0;
        /* etat_esv2 = digitalRead(12);
      //Serial.println(etat_esv2);
      if (etat_esv2 == 0) {*/

        if (num_char > 20) {
          receive.trim();
          int post = receive.length() + 1;
          char receiv[post];
          // int t = post - 9;
          // char message[t];

          // int post;
        //  char datas[50];
        //  char carlu;

          receive.toCharArray(receiv, post);
          //Serial.println(receiv);
          donn = 0;
          String f;
          for (int y = 0; y < post; y++) {
            if (receiv[y] == ';') {
              // Serial.println(f);
              switch (donn) {
                case 0:
                  Numero = f;
                  Serial.print("le numero : ");
                  Serial.println(Numero);
                  donn = 1;
                  break;
                case 1:
                  Nom = f;
                  Serial.print("le Nom : ");
                  Serial.println(Nom);
                  donn = 2;
                  break;
                case 2:
                  Classe = f;
                  Serial.print("la classe : ");
                  Serial.println(Classe);
                  donn = 3;
                  break;
                case 3:
                  situation = f;
                  Serial.print("Situation : ");
                  Serial.println(situation);
                  donn = 4;
                  break;
                case 4:
                  Heure = f;
                  f = "";
                  int num_1 = Heure.length() + 1;
                  char temp[num_1];
                  Heure.toCharArray(temp, num_1);
                  Serial.println(num_1);
                  z = 0;
                  for (int t = 0; t < num_1; t++) {
                    //  Serial.print( temp[t]);
                    if (temp[t] == 'H') {
                      H = f;
                      Serial.print("les Heures : ");
                      Serial.println(H);
                      z = 1;

                    } else if (temp[t] == 'm') {
                      Serial.print("les minut :");
                      Serial.println(mi);
                      min = mi;
                      if (situation.indexOf("222") >= 0) {
                        Message1 = "Cher parent, votre enfant " + Nom + " de la classe de " + Classe + " est sorti du college aujourd'hui a " + Heure;
                        Message2 = "Dear parent, your child " + Nom + " of form " + Classe + " left the college today at " + H + ":" + min;
                      }
                      if (situation.indexOf("111") >= 0) {
                        Message1 = "Cher parent, votre enfant " + Nom + " de la classe de " + Classe + " est entre au college aujourd'hui a " + Heure;
                        Message2 = "Dear parent, your child " + Nom + " of form " + Classe + " has entered into the college today at " + H + ":" + min;
                      }
                      suiva = 0;
                      mi="";
                      f = "";
                      Nom = "";
                      Classe = "";
                      Heure = "";
                      H = "";
                      min = "";
                      situation = "";
                      z = 5;
                      suiva = 1;
                    } else {
                      f += temp[t];
                      if (z > 0) {
                        mi += temp[t];
                        z += 1;
                      }
                    }
                  }
                  mi="";
                  Serial.print("fin");
                  donn = 5;
                  break;
              }
              // donn = 1;
              f = "";

            } else f += receiv[y];
          }
        }
        z = 0;
        if (suiva == 1) {
          int pos1 = Message1.length() + 1;
          int pos2 = Message2.length() + 1;
          char ms1[pos1];
          char ms2[pos2];
          Numero.toCharArray(numero, 10);
          Message1.toCharArray(ms1, pos1);
          Message2.toCharArray(ms2, pos2);
          Serial.println("");
          Serial.print("Numero :");
          Serial.println(numero);
          Serial.print("message1 :");
          Serial.println(ms1);
          Serial.print("message2 :");
          Serial.println(ms2);
          GPRS gprs(8, 9, 9600);
          gprs.checkPowerUp();
          while (!gprs.init()) {
            digitalWrite(send, 0);
            Serial.print(".");
          }
          delay(100);
          Serial.println(" ");
          Serial.println("start!!");
          send_1(numero, ms1);
          delay(1000);
          send_1(numero, ms2);
          Serial.print("succes!!!");
          portOne.begin(9600);
          delay(1000);
        }
        portOne.print("fine");
      }
    }
    res = 0;
    receive = "";
  }
}
void send_1(const char* num, const char* mes) {
  gprs.sendSMS(num, mes);
}