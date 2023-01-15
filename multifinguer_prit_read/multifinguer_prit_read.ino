#include <Adafruit_Fingerprint.h>
#include <HardwareSerial.h>
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"
const char* ssid = "ESP32_soft";
const char* password = "123456789";
uint8_t id_pres;
boolean enroll_stor;
int add;
unsigned long temp;
AsyncWebServer server(80);
#define libresv 35
#define etat_serv 4
#define etat 14
HardwareSerial Seriala(1);
HardwareSerial data(0);  // RX, TX
Adafruit_Fingerprint finger1 = Adafruit_Fingerprint(&Seriala);
Adafruit_Fingerprint finger2 = Adafruit_Fingerprint(&Serial2);
String Receive_ID, Date, Heure, Data_ID_save, Nom_ID_Receive, Name_Read;
boolean fin, libre_esv, libre_esvp, send_exist, i;
boolean pause_1;
unsigned long temps;
String ID_cmp, send_id_esclave, Save, Classe, Nom, Numero, Matricule, Situa, ID_read;
String situation, save_situation;
char dt_read[500];
int Cuiseur, init_save;
int receive, y, ID_cmpINT, p;
int pos = 0;
char carlu;
uint8_t id;
boolean Save_Send_GSM, Mode_leccture, lecture, mode;
int Zone;
File file;
String Send_data() {
  send_id_esclave = "";
  digitalWrite(etat_serv, 0);
  send_exist = 1;
  recherche(SD, "/ID_Read", 1);
  if (fin == 1) {
    i = 1;
    Zone = 2;
    lecture_data(SD, Name_Read);
    if (fin == 1) {
      read_situation();
      // read_time();
      send_id_esclave = Numero + ";" + Nom + ";" + Classe + ";" + situation + ";" + Heure + ";";
      Save = Matricule + ";" + Nom + ";" + Situa + ";" + Heure + ";" + Date;
      Matricule = "/F_Suivis/" + String(Matricule) + ".csv";
      fiche_de_suivis(SD, Matricule, Save);
      // delay(50);
      //Send_Stud.print(Numero);
      Serial.println(send_id_esclave);
      //delay(1000);
    }
    pause_1 = 0;
    Numero="";
    Nom="";
    Classe="";
    situation="";
    Heure="";
    digitalWrite(etat_serv, 0);
  }
  return send_id_esclave;
}
int num_fing;
String enroll() {
  String sends;
  if (num_fing != 0) {
    mode = 1;
    sends = "enroll...";
  } else sends = "num_finguer invalide";
  return sends;
}
String plus() {
  if (num_fing < 127) num_fing += 1;
  String send_plus = "numero de l'eleve : " + String(num_fing);
  return send_plus;
}
String moins() {
  if (num_fing > 1) num_fing -= 1;
  String send_moins = "numero de l'eleve : " + String(num_fing);
  return send_moins;
}
void recherche(fs::FS& fs, const char* dirname, uint8_t levels) {
  //Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  if (file) {
    // Serial.print(file.name());
    if (send_exist == 1) {

      send_exist = 0;
      Name_Read = "/Data_bs/lst_elv.csv";
      ID_cmp = String(file.name());
      ID_cmpINT = ID_cmp.toInt();
      String read_ID_E = "/ID_Read/" + ID_cmp;
      Zone = 1;
      i = 1;
      Serial.print("send");
      lecture_data(SD, read_ID_E);
      fin = 1;
      fs.remove("/ID_Read/" + ID_cmp);


    } else {
      digitalWrite(etat_serv, 1);
      //Serial.println("Sssssssssssssssssssssssssss");
    }
  } else {
    pause_1 = 1;
    digitalWrite(etat_serv, 0);
    lecture = 1;
    // Serial.println("Stoppppppppppppppppppppppppppppp");
    fin = 0;
  }
  Serial.println("recherche...............");
}
void lecture_data(fs::FS& fs, String noms_file) {
  /* Serial.print("Mode_leccture :");
  Serial.println(Mode_leccture);
  Serial.print("init_save :" );
  Serial.println(init_save  );
  Serial.print("Cuiseur :" );
  Serial.println(Cuiseur  );
  Serial.print("Save_Send_GSM :" );
  Serial.println(Save_Send_GSM  );
  Serial.print("i :" );
  Serial.println(i  );*/
  file = fs.open(noms_file);
  // Serial.println("ok2");
  if (Zone == 1) {
    if (file) {
      Cuiseur = 0;
      do {
        carlu = file.read();
        if (carlu == ';') {
          switch (Cuiseur) {
            case 0:
              for (y = 0; y < 51; y++) dt_read[y] = 0;
              pos = 0;
              Cuiseur = 1;
              break;
            case 1:
              Date = dt_read;
              Cuiseur = 2;
              break;
            case 2:
              Heure = dt_read;
              Cuiseur = 0;
              i = 0;
              Zone = 0;
              break;
          }
          for (y = 0; y < 51; y++) dt_read[y] = 0;
          pos = 0;
        } else {
          dt_read[pos++] = carlu;
          dt_read[pos] = 0;
        }

      } while (i == 1);
      file.close();
      // Serial.println("succes...");
      //get_data();
    } else {
      // if the file didn't open, print an error:
      Serial.println("erro");
    }
  } else if (Zone == 2) {
    if (Mode_leccture == 0) {
      if (file) {
        //Serial.println(noms_file);


        do {
          carlu = file.read();  // essaie de lire un caractère
                                // Serial.print(carlu);
          if (init_save >= 1) {
            if ((carlu == ';') or ((Cuiseur == 3) and (carlu == '.'))) {

              if (Save_Send_GSM == 1) {
                switch (Cuiseur) {

                  case 0:
                    Matricule = dt_read;
                    Cuiseur = 1;
                    break;
                  case 1:
                    Nom = dt_read;
                    Cuiseur = 2;
                    break;
                  case 2:
                    Classe = dt_read;
                    Cuiseur = 3;
                    break;
                  case 3:
                    Numero = dt_read;
                    Mode_leccture = 1;
                    Cuiseur = 0;
                    /*  Serial.println("");
                    Serial.print("Matricule : ");
                    Serial.println(Matricule);*/
                    Serial.print("Nom : ");
                    Serial.println(Nom);
                    /*  Serial.print("Classe : ");
                    Serial.println(Classe);
                    Serial.print("Numero : ");
                    Serial.println(Numero);*/
                    i = 0;
                    //fin = 1;
                    init_save = 0;
                    Save_Send_GSM = 0;
                    file.close();
                    break;
                }
                /*for (y = 0; y < 51; y++) dt_read[y] = 0;
                pos = 0;*/
              } else {
                if (init_save % 2 != 0) {
                  init_save += 1;
                  ID_read = dt_read;
                  int ID_readINT = ID_read.toInt();
                  Serial.println("ID read : ");
                  Serial.println(ID_read);
                  if (ID_readINT == ID_cmpINT) {
                    Save_Send_GSM = 1;
                  }
                }
                //pos = 0;
              }
              for (y = 0; y < 51; y++) dt_read[y] = 0;
              pos = 0;
            } else if (carlu == '.') {
              for (y = 0; y < 51; y++) dt_read[y] = 0;
              pos = 0;
              Serial.println("fin de la ligne");
              init_save += 1;
            } else if (carlu == ')') {
              for (y = 0; y < 51; y++) dt_read[y] = 0;
              pos = 0;
              Serial.println("l'eleve n'existe pas");
              file.close();
              fin = 0;
              i = 0;
              //fin = 1;
              init_save = 0;
              Save_Send_GSM = 0;
              Cuiseur = 0;
              Zone = 0;
            } else {
              dt_read[pos++] = carlu;  // ajoute le caractère dans prenom
              dt_read[pos] = 0;
            }


          } else {
            if (carlu == '.') {
              //Serial.println("fin de la ligne");
              init_save += 1;
            }
          }
        } while (i == 1);
        file.close();
        // Serial.println("succes...");
      } else {
        Serial.println("erro");
      }
      //delay(10);
    } else {
      if (file) {
        //Serial.println(noms_file);
        int pos = 0;
        char carlu;

        do {
          carlu = file.read();
          if (carlu == '.')
            ;  //Serial.println("no Read");
          else if (carlu == ';') {
            situation = dt_read;
            i = 0;
            for (int y = 0; y < 21; y++) dt_read[y] = 0;
            pos = 0;
          } else {
            dt_read[pos++] = carlu;  // ajoute le caractère dans prenom
            dt_read[pos] = 0;
          }

        } while (i == 1);
        file.close();
        Zone = 0;
        Mode_leccture = 0;
        //Serial.println("succes...");
      } else {
        // if the file didn't open, print an error:
        Serial.println("erro");
      }
      //delay(10);
    }
  }
}

void read_situation() {
  // Serial.println("situation ok");
  Creation_dossier(SD, "/Stt");
  Creation_dossier(SD, "/F_Suivis");
  String nom_file = "/Stt/" + String(ID_cmp) + ".bin";
  if (!SD.exists(nom_file)) {
    save_situation = "222.;";
    //SerialBT.println(nom_poste);
    Save_Data(SD, nom_file, save_situation);
    //Serial.println("regist situation init ok");
    Situa = "ARRIVER";
    situation = "111";
  } else {
    i = 1;
    lecture_data(SD, nom_file);
     if (situation == "111") {
      Situa = "ARRIVER";
      save_situation = "222.;";
      Save_Data(SD, nom_file, save_situation);
    }
    if (situation == "222") {
      i = 1;
      Situa = "DEPART";
      save_situation = "111.;";
      Save_Data(SD, nom_file, save_situation);
    }
        
  }
}
void fiche_de_suivis(fs::FS& fs, String path, String message) {
  if (!fs.exists(path)) {
    file = fs.open(path, FILE_APPEND);
    file.println(" MATRICULE ; NOM & PRENOM ; SITUATION ; HEURS ; DATE ");
    file.println(" ");
  } else file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if (file.println(message)) {
    ;  //Serial.println("succes...");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}
#define led_f1 12
#define led_f2 15
void setup() {
  pinMode(libresv, INPUT_PULLUP);
  pinMode(etat_serv, OUTPUT);
  pinMode(etat, OUTPUT);
  pinMode(led_f1, OUTPUT);
  pinMode(led_f2, OUTPUT);
  Serial.begin(57600);
  Serial2.begin(57600);
  data.begin(57600);
  Seriala.begin(57600, SERIAL_8N1, 33, 32);
  while (!Serial)
    ;  // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\n\nAdafruit finger detect test");
  finger1.begin(57600);
  delay(5);
  finger1.verifyPassword();
  finger1.getTemplateCount();
  finger2.begin(57600);
  delay(5);
  finger2.verifyPassword();
  finger2.getTemplateCount();
  if (!SD.begin()) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }
  fin = 1;
  Name_Read = "/Data_bs/lst_elv.csv";
  // put your setup code here, to run once:
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  server.on("/data", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send_P(200, "text/plain", Send_data().c_str());
  });
  server.on("/enroll", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send_P(200, "text/plain", enroll().c_str());
  });
  server.on("/plus", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send_P(200, "text/plain", plus().c_str());
  });
  server.on("/moins", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send_P(200, "text/plain", moins().c_str());
  });
  server.begin();
  pinMode(25, OUTPUT);
}

void loop() {
  digitalWrite(etat, 0);
  while (mode == 0) {
    read_finguer();
    libre_esv = digitalRead(libresv);
    if (pause_1 == 0) {
      send_exist = 0;
      // Serial.println("ok..................................................................");
      //Serial.println("ok..................................................................");
      recherche(SD, "/ID_Read", 1);
      pause_1 = 1;
    }
    libre_esvp = libre_esv;
  }
  while (mode == 1) {
    Serial.println("enroll");
    for (int j = 0; j < 8; j++) {
      digitalWrite(led_f1, !digitalRead(led_f1));
      digitalWrite(led_f2, !digitalRead(led_f2));
      delay(250);
    }
    digitalWrite(led_f1, 1);
    digitalWrite(led_f2, 0);
    getFingerprintEnroll1();
    delay(5000);
    mode = 0;
  }
  // put your main code here, to run repeatedly:
}

void read_finguer() {
  id = num_fing;
  getFingerprintID1();
  getFingerprintID2();
  //Serial.print(libre_esv);
  delay(50);
}
//finguerprint read......................................................................
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////
uint8_t getFingerprintID1() {
  uint8_t p = finger1.getImage();
  if (p == FINGERPRINT_OK)
    Serial.println("Image taken");
  else {
    //SerialBT.print(".");
    Serial.print(".");
    //if (post==1){SerialBT.print("");post=0;}
    return p;
  }

  // OK success!

  p = finger1.image2Tz();
  if (p == FINGERPRINT_OK)
    ;  //Serial.println("Image converted");
  else {
    tone(25, 150, 100);
    delay(500);
    tone(25, 150, 100);
    return p;
  }

  // OK converted!
  p = finger1.fingerSearch();
  if (p == FINGERPRINT_OK) Serial.println("Found a print match!");
  else {
    tone(25, 150, 100);
    delay(500);
    tone(25, 150, 100);
    Serial.println("Did not find a match");
    return p;
  }
  Serial.println("emprinte reconnu");
  if (finger1.fingerID != id_pres) {
    tone(25, 1200, 100);
    digitalWrite(led_f1, 1);
    delay(500);
    tone(25, 1200, 100);
  } else {
    for (int j = 0; j < 8; j++) {
      digitalWrite(led_f1, !digitalRead(led_f1));
      //digitalWrite(led_f2, !digitalRead(led_f2));
      delay(250);
    }
  }
  save_fing(finger1.fingerID);
  digitalWrite(led_f1, 0);
  return finger1.fingerID;
}
uint8_t getFingerprintID2() {
  uint8_t p = finger2.getImage();
  if (p == FINGERPRINT_OK)
    Serial.println("Image taken");
  else {
    //SerialBT.print(".");
    Serial.print(".");
    //if (post==1){SerialBT.print("");post=0;}
    return p;
  }

  // OK success!

  p = finger2.image2Tz();
  if (p == FINGERPRINT_OK)
    ;  //Serial.println("Image converted");
  else {
    tone(25, 150, 100);
    delay(500);
    tone(25, 150, 100);
    return p;
  }

  // OK converted!
  p = finger2.fingerSearch();
  if (p == FINGERPRINT_OK) Serial.println("Found a print match!");
  else {
    tone(25, 150, 100);
    delay(500);
    tone(25, 150, 100);
    Serial.println("Did not find a match");
    return p;
  }
  Serial.println("emprinte reconnu");
  delay(500);
  if (finger2.fingerID != id_pres) {
    digitalWrite(led_f2, 1);
    tone(25, 1200, 100);
    //Serial.println(finger.fingerID);
    //digitalWrite(6, 1);
    delay(500);
    tone(25, 1200, 100);
  } else {
    for (int j = 0; j < 8; j++) {
      // digitalWrite(led_f1, !digitalRead(led_f1));
      digitalWrite(led_f2, !digitalRead(led_f2));
      delay(250);
    }
  }
  save_fing(finger2.fingerID);
  digitalWrite(led_f2, 0);
  return finger2.fingerID;
}

void save_fing(uint8_t fing) {
  if (id_pres != fing) {
    pause_1 = 0;
    Receive_ID = String(fing);
    Creation_dossier(SD, "/ID_Read");
    Nom_ID_Receive = "/ID_Read/" + String(Receive_ID);
    if (!SD.exists(Nom_ID_Receive)) {
      add = 1;
      Serial.println("Send");
      read_time();
      // Serial.print
      Data_ID_save = Receive_ID + ";" + Date + ";" + Heure + ";";
      fin = 1;
      Save_Data(SD, Nom_ID_Receive, Data_ID_save);
    } else
      Serial.println("informations déjà enregistré");
    digitalWrite(etat_serv, 1);
  }
  id_pres = fing;
}
////////////////////////////////////////////
////////////////////////////////////////////////////////////
//fin finguerprint read........................................................................................
void Creation_dossier(fs::FS& fs, String path) {
  //Serial.printf("Creating Dir: %s\n", path);
  if (fs.mkdir(path))
    ;  //Serial.println("Dir created");
  else
    ;  //Serial.println("mkdir failed");
}

void Save_Data(fs::FS& fs, String path, String message) {
  //Serial.println("Writing file:" + path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    ;  // Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}


void read_time() {
  data.begin(57600);
  //delay(500);
  digitalWrite(etat, HIGH);
  while (add == 1) {

    Date = "";
    Heure = "";
    /*  if (millis() > (temp + 4000)) {
      data.println("Send");
      temp = millis();
    }*/
    if (data.available()) {
      String receive = data.readString();
      Serial.println(receive);
      int num_char = receive.length();
      Serial.print(num_char);
      if ((num_char < 100) and (num_char > 15)) {
        receive.trim();
        int pos = receive.length() + 1;
        char receiv[pos];
        char date[15];
        char heure[15];
        int z;
        receive.toCharArray(receiv, pos);

        for (int y = 0; y < pos; y++) {
          int d = 0;
          //Serial.println(receiv[y]);
          // delay(1000);
          if ((receiv[y] == 'D') and (d < 3)) {
            d += 1;
            if (z == 2) z = 3;
            else z = 1;
            Serial.println(z);
          } else if ((receiv[y] == ';') and (d == 1)) {
            d += 1;
            z = 2;
            Serial.println("");
            Serial.println(receiv[y]);
          } else if (z == 1) {
            Date += receiv[y];

          } else if (z == 2) {
            Heure += receiv[y];
          }
        }
        Serial.println(" ");
        Serial.println(Date);
        Serial.println(Heure);
        Serial.println(" fin............");
        add = 0;
      }
      add = 0;
      Serial.println(Heure);
    }
  }
}

uint8_t getFingerprintEnroll1() {
enroll1:
  p = -1;
  enroll_stor = 0;
  Serial.print("Waiting for valid finger to enroll as #");
  Serial.println(id);
  Serial.print("poser le Doigt sur le lecteur ");
  while (p != FINGERPRINT_OK) {
    p = finger1.getImage();
    if (p == FINGERPRINT_OK) Serial.println("Image prise");
    else {  //SerialBT.print(".");
      Serial.print(".");
    }
  }

  // OK success!

  p = finger1.image2Tz(1);
  if (p == FINGERPRINT_OK) {
    //Serial.println("Image converted");
  } else {
    ;  //Serial.println("Unknown error");
    //return p;
  }
  Serial.println("Remove finger");
  digitalWrite(led_f1, 0);
  digitalWrite(led_f2, 0);
  //SerialBT.println("Retirer le Doigt");
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger1.getImage();
  }
  //Serial.println(id);
  delay(2000);
  digitalWrite(led_f1, 1);
  digitalWrite(led_f2, 0);
  p = -1;
  Serial.println("Place same finger again");
  // SerialBT.println("Replacer le Doigt");
  while (p != FINGERPRINT_OK) {
    p = finger1.getImage();
    if (p == FINGERPRINT_OK) {
      Serial.println("Image prise");
    } else {
      Serial.print(".");
      //SerialBT.print(".");
    }
  }

  // OK success!

  p = finger1.image2Tz(2);
  if (p == FINGERPRINT_OK) {
    //Serial.println("Image converted");
  } else {
    //Serial.println("Unknown error");
    return p;
  }
  //return p;
  // OK converted!
  //Serial.print("Creating model for #");
  //Serial.println(id);
  p = finger1.createModel();
  if (p == FINGERPRINT_OK) {
    //Serial.println("Prints matched!");
  } else {
    //Serial.println("Unknown error");
    return p;
  }
  //SerialBT.print("ID ");
  //SerialBT.println(id);
  p = finger1.storeModel(id);
  if (p == FINGERPRINT_OK) {
    enroll_stor = 1;
    for (int j = 0; j < 8; j++) {
      // digitalWrite(led_f1, !digitalRead(led_f1));
      digitalWrite(led_f1, !digitalRead(led_f1));
      delay(250);
    }
    digitalWrite(led_f1, 1);
    digitalWrite(led_f2, 0);
    Serial.println("Stored!");
  } else {
    // digitalWrite(6, 0);
    return p;
  }
  if (enroll_stor == 0) goto enroll1;
  Serial.println(" ");
  Serial.println("fin1");
  getFingerprintEnroll2();

  return true;
}
uint8_t getFingerprintEnroll2() {
enroll2:
  p = -1;
  enroll_stor = 0;
  Serial.print("Waiting for valid finger to enroll as #");
  Serial.println(id);
  digitalWrite(led_f1, 1);
  digitalWrite(led_f2, 1);
  Serial.print("poser le Doigt sur le lecteur ");
  while (p != FINGERPRINT_OK) {
    p = finger2.getImage();
    if (p == FINGERPRINT_OK) Serial.println("Image prise");
    else {  //SerialBT.print(".");
      Serial.print(".");
    }
  }

  // OK success!

  p = finger2.image2Tz(1);
  if (p == FINGERPRINT_OK) {
    //Serial.println("Image converted");
  } else {
    ;  //Serial.println("Unknown error");
    //return p;
  }
  Serial.println("Remove finger");
  digitalWrite(led_f1, 1);
  digitalWrite(led_f2, 0);
  //SerialBT.println("Retirer le Doigt");
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger2.getImage();
  }
  //Serial.println(id);
  delay(2000);
  p = -1;
  digitalWrite(led_f1, 1);
  digitalWrite(led_f2, 1);
  Serial.println("Place same finger again");
  // SerialBT.println("Replacer le Doigt");
  while (p != FINGERPRINT_OK) {
    p = finger2.getImage();
    if (p == FINGERPRINT_OK) {
      Serial.println("Image prise");
    } else {
      Serial.print(".");
      //SerialBT.print(".");
    }
  }

  // OK success!

  p = finger2.image2Tz(2);
  if (p == FINGERPRINT_OK) {
    //Serial.println("Image converted");
  } else {
    //Serial.println("Unknown error");
    return p;
  }
  //return p;
  // OK converted!
  //Serial.print("Creating model for #");
  //Serial.println(id);
  p = finger2.createModel();
  if (p == FINGERPRINT_OK) {
    //Serial.println("Prints matched!");
  } else {
    //Serial.println("Unknown error");
    return p;
  }
  //SerialBT.print("ID ");
  //SerialBT.println(id);
  p = finger2.storeModel(id);
  if (p == FINGERPRINT_OK) {
    enroll_stor = 1;
    for (int j = 0; j < 8; j++) {
      // digitalWrite(led_f1, !digitalRead(led_f1));
      digitalWrite(led_f2, !digitalRead(led_f2));
      delay(250);
    }
    digitalWrite(led_f1, 1);
    digitalWrite(led_f2, 1);
    delay(3000);
    Serial.println("Stored!");
  } else {
    // digitalWrite(6, 0);
    return p;
  }
  if (enroll_stor == 0) goto enroll2;
  Serial.println(" ");
  Serial.println("fin2");
  digitalWrite(led_f1, 0);
  digitalWrite(led_f2, 0);
  return true;
}