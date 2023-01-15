
#include <WiFi.h>
#include <HTTPClient.h>
#include <HardwareSerial.h>
HardwareSerial Seriala(2);
HardwareSerial SIM900(1);

const char* ssid = "ESP32_soft";
const char* password = "123456789";
String mesg;
//Your IP address or domain name with URL path
const char* data = "http://192.168.4.1/data";
char Numero[10];
boolean serv, connect;
unsigned long previousMillis = 0;
unsigned long po = 0;
const long interval = 5000;
boolean suiva;
//char numero[10];
int res, p, y, j;
int z, v;
int donn;
String Message1, Message2;
String H;
String Min;
String mi;
String numero;
String Nom, Classe, situation, Heure, receive;
void setup() {
  Seriala.begin(9600);
  SIM900.begin(9600, SERIAL_8N1, 27, 26);
  Serial.println("configuration par defaut");
  pinMode(2, OUTPUT);
  SIM900.println("ATZ");  // Pour remettre les configuration par defaut
  digitalWrite(2, 1);
  GetSIM900();
  SIM900.println("AT+CMEE=1");  // Pour afficher le numero de l'erreur
  digitalWrite(2, 0);
  GetSIM900();
  Serial.println("Code PIN");
  SIM900.println("AT+CPIN=1234");  // Code PIN
  digitalWrite(2, 1);
  GetSIM900();
  digitalWrite(2, 0);
  for (int j = 0; j < 25; j++) {
    digitalWrite(2, !digitalRead(2));
    delay(500);
  }
  digitalWrite(2, 0);
  pinMode(4, INPUT_PULLUP);
  pinMode(23, INPUT_PULLUP);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    digitalWrite(2, !digitalRead(2));
    delay(500);
    Serial.print(".");
  }
  digitalWrite(2, 0);
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  serv = digitalRead(4);
  delay(500);
  if (connect == 1) {
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, password);
      digitalWrite(2, !digitalRead(2));
      delay(500);
      Serial.print(".");
    }
    digitalWrite(2, 0);
    connect = 0;
  }

  if (serv == 1) {
    if (WiFi.status() == WL_CONNECTED) {
      String receive = httpGETRequest(data);
      boolean etat_esv2 = digitalRead(23);
      //Serial.println(etat_esv2);
      if (receive != "disc") {
        receive.trim();
        int post = receive.length() + 1;
        char receiv[post];
        // int t = post - 9;
        // char message[t];

        // int post;
        //  char datas[50];
        //  char carlu;

        receive.toCharArray(receiv, post);
        Serial.println(receiv);
        donn = 0;
        String f;
        for (int y = 0; y < post; y++) {
          if (receiv[y] == ';') {
            // Serial.println(f);
            switch (donn) {
              case 0:
                numero = f;
                Serial.print("le numero : ");
                Serial.println(numero);
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
                    Min = mi;
                    if (situation.indexOf("222") >= 0) {
                      Message1 = "Cher parent, votre enfant " + Nom + " de la classe de " + Classe + " est sorti du college aujourd'hui a " + Heure;
                      Message2 = "Dear parent, your child " + Nom + " of form " + Classe + " left the college today at " + H + ":" + Min;
                    }
                    if (situation.indexOf("111") >= 0) {
                      Message1 = "Cher parent, votre enfant " + Nom + " de la classe de " + Classe + " est entre au college aujourd'hui a " + Heure;
                      Message2 = "Dear parent, your child " + Nom + " of form " + Classe + " has entered into the college today at " + H + ":" + Min;
                      //mesg = Message1 + "\n \n" + Message2;
                    }
                    suiva = 0;
                    mi = "";
                    f = "";
                    Nom = "";
                    Classe = "";
                    Heure = "";
                    H = "";
                    Min = "";
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
                mi = "";
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
      digitalWrite(2, 1);
      if (suiva == 1) {
        int pos1 = Message1.length() + 1;
        int pos2 = Message2.length() + 1;
        char ms1[pos1];
        char ms2[pos2];
        int pos3 = mesg.length() + 1;
        char msg[pos3];
        mesg.toCharArray(msg, pos3);
        numero.toCharArray(Numero, 10);
        Message1.toCharArray(ms1, pos1);
        Serial.println("");
        Serial.print("Numero :");
        Serial.println(numero);
        Serial.print("message1 :");
        Serial.println(ms1);
        Send_SMS(Numero, ms1);
        delay(10000);
        Serial.print("message2 :");
        Message2.toCharArray(ms2, pos2);
        Serial.println(ms2);
        Send_SMS(Numero, ms2);
        Serial.println("start!!");
        // Serial.println(msg);

        // GetSIM900();
        /* v = 1;
        po = millis();
        while (v == 1) {
          if (millis() > (po + 4000)) {
            v=0;
            Send_SMS(Numero, ms2);
            Serial.print("succes!!!");
            delay(1000);
            Serial.print("succes!!!");
          }
        }   */
        receive = "";
        /*       ms1="";
        ms2="";*/
        digitalWrite(2, 0);
      } else connect = 1;
    } else {
      Serial.println("WiFi Disconnected");
      connect = 1;
    }
  }
}

String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;

  // Your Domain name with URL path or IP address with path
  http.begin(client, serverName);

  // Send HTTP POST request
  int httpResponseCode = http.GET();

  String payload = "disc";

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}
void Send_SMS(const char* num, const char* sms) {
  Serial.println("Activation du mode SMS");
  SIM900.println("AT+CMGF=1");  // Activation du mode SMS
  GetSIM900();
  Serial.println("Envoi du numero de telephone");
  SIM900.println("AT+CMGS=\"" + String(num) + "\"");  // Envoi du numéro de téléphone
  GetSIM900();

  Serial.println("Corps du message");
  SIM900.println(sms);  // Corps du message
  Serial.println("CTR+Z");
  SIM900.write(0x1A);  // CTR+Z
  GetSIM900();
}
void GetSIM900() {
  delay(1000);

  if (SIM900.available()) {
    Serial.print("\n");
    delay(3000);
    while (SIM900.available()) {
      Serial.print((char)SIM900.read());  // On vide la mémoire tampon du SIM900
    }
    Serial.print("\n");
  }
}