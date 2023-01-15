#include <HardwareSerial.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "RTClib.h"

RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
HardwareSerial Send_Stud(2);  // RX, TX
HardwareSerial Read_ID(1);
String Data_ID_save;
String Name_Read, ID_cmp, ID_read;
String Classe, Nom, Numero, Matricule, Save, Situa;
String send_id_esclave;
String situation, save_situation;
String Time, Date, Heure;
File file;
int jeton, Zone;
char dt_read[500];
int Cuiseur, init_save;
int receive, y, fini, paus, fin;
int pos = 0;
char carlu;
boolean Save_Send_GSM, i, Mode_leccture, etat_esv1, etat_esv1_p, save_etat, save_etat2, etat_esv2, etat_esv2_p;
unsigned long temps, temps_2;
#define led 2
#define esclave_1_send 14
#define esclave_2_send 34
void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  Send_Stud.begin(9600);
  //init_Horl();
  pinMode(led, OUTPUT);
  pinMode(esclave_1_send, INPUT_PULLUP);
  pinMode(esclave_2_send, INPUT_PULLUP);
  Read_ID.begin(9600, SERIAL_8N1, 27, 26);
  if (!SD.begin()) {
    Serial.println("Card Mount Failed");
    return;
  }
  while (!Serial)
    ;
  Serial.println("Goodnight moon!");

  // set the data rate for the SoftwareSerial port
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
  i = 1;
  Name_Read = "/Data_bs/lst_elv.csv";
  //f931deb3
  //a72910b3
  // ID_cmp = "a72910b3";
  /*lecture_data(SD, Name_Read);
  read_situation();
  String send_id_esclave = "Bonjour cher parent. Votre enfant " + Nom + " de " + Classe + " vient " + situation + " l'établissement. ";
  Send_Stud.println(send_id_esclave);
  Send_Stud.print(Numero);*/
  fin = 1;
  // delay(2000);
  Send_Stud.println("send?");
}
void init_Horl() {

#ifndef ESP8266
  while (!Serial)
    ;  // wait for serial port to connect. Needed for native USB
#endif

  if (!rtc.begin()) {
    // Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  if (!rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  //rtc.adjust(DateTime(2023, 1, 5, 9, 17, 0));
}

void loop() {  // run over and over
  receive_UID();
  if (Send_Stud.available()) {
    String receive = Send_Stud.readString();
    Serial.println("esclave de sortie vient d'envoyer : ");
    receive.trim();
    Serial.println(receive);
    int num_char = receive.length();
    if ((num_char > 3) and (num_char < 13)) {
      if (receive.indexOf("send") >= 0) fini = 4;
      if (receive.indexOf("fine") >= 0) {
        Send_Stud.println("send?");
        SD.remove("/ID_Read/" + ID_cmp);
      }
    }
    Serial.println(jeton);
  }
  // read_time();

  // etat_esv1 = digitalRead(esclave_1_send);
  //etat_esv2 = digitalRead(esclave_2_send);
  /* if ((etat_esv1 == 1) and (etat_esv1_p == 0)) temps = millis();
  if ((etat_esv1 == 1) and (save_etat == 0) and (millis() > (temps + 15000))) {
    jeton = 1;
    Send_ID();
  }*/
  //if(etat_esv2 == 1){ Serial.println(etat_esv2);}
  /* if ((etat_esv2 == 1) and (etat_esv2_p == 0)) {
    temps_2 = millis();
    Serial.println(etat_esv2_p);
  }*/
  //etat_esv2_p =digitalRead(esclave_2_send);
  if (fini == 4) {
    Serial.println("send :");
    jeton = 1;
    Send_ID();
  }
  /* Serial.print("etat_esv2_p : ");
  Serial.println(etat_esv2_p);
  delay(1000);  //etat_esv1_p = etat_esv1;*/
}
void receive_UID() {
  if (Read_ID.available()) {
    digitalWrite(led, 1);
    String Receive_ID = Read_ID.readString();
    Serial.println(Receive_ID);
    Creation_dossier(SD, "/ID_Read");
    Receive_ID.trim();
    String IDs = Receive_ID.substring(0, 8);
    String Nom_ID_Receive = "/ID_Read/" + String(IDs);
    if (!SD.exists(Nom_ID_Receive)) {
      /* read_time();
      Data_ID_save = Receive_ID + ";" + Date + ";" + Heure + ";";
      Data_ID_save = "";*/
      Date = "";
      Heure = "";
      fin = 1;
      Save_Data(SD, Nom_ID_Receive, Receive_ID);
    } else
      Serial.println("informations déjà enregistré");
    /* if (Serial.available()) {
    mySerial.write(Serial.read());
  }*/
    digitalWrite(led, 0);
    save_etat = 0;
    save_etat2 = 0;
    if (paus == 4) fini = 4;
  }

  fin = 1;
}
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

void Send_ID() {
  recherche(SD, "/ID_Read", 1);
  delay(100);
  Serial.println("..........");
  Serial.print(fin);
  if (fin == 1) {
    i = 1;
    Zone = 2;
    Serial.println("........................");
    while (fin == 1) {
      Zone = 2;
      lecture_data(SD, "/Data_bs/lst_elv.csv");
    }
    delay(100);
    Serial.println(".....................................");
    Serial.print(fin);

    if (fin == 2) {
      read_situation();
      delay(100);
      // read_time();
      send_id_esclave = Numero + ";" + Nom + ";" + Classe + ";" + situation + ";" + Heure + ";";
      Save = Matricule + ";" + Nom + ";" + Situa + ";" + Heure + ";" + Date;
      Matricule = "/F_Suivis/" + String(Matricule) + ".csv";
      fiche_de_suivis(SD, Matricule, Save);
      Numero = "";
      Nom = "";
      Classe = "";
      situation = "";
      Heure = "";
      Matricule = "";
      Date = "";
      Situa = "";
      if (jeton == 1) {
        Serial.print("Send 1 :");
        Serial.println(send_id_esclave);
        Send_Stud.println(send_id_esclave);
        delay(100);
        send_id_esclave = "";
        delay(900);
        jeton = 0;
        fin = 0;
        Zone = 0;
      }
      /*else if (jeton == 2) {
        Serial.println("Send2 : ");
        Read_ID.println(send_id_esclave);
        delay(100);
        send_id_esclave = "";
        delay(900);
        jeton = 0;
      }*/
      // delay(50);
      //Send_Stud.print(Numero);
      Serial.println(send_id_esclave);
      fin = 0;
      delay(1000);
    }
  }
  //temps = millis();
  // String send_id_esclave = "" Send_Stud.print();
}

void recherche(fs::FS& fs, const char* dirname, uint8_t levels) {

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
    Name_Read = "/Data_bs/lst_elv.csv";
    ID_cmp = String(file.name());
    Serial.println(ID_cmp);
    String read_ID_E = "/ID_Read/" + ID_cmp;
    Zone = 1;
    i = 1;
    Serial.print("recherche1..");
    lecture_data(SD, read_ID_E);
    Serial.print("fine1..");
    fin = 1;
    paus = 0;
    fini = 0;
  } else {
    Serial.println("nommmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm");
    paus = 4;
    fini = 0;
    i = 0;
    //  save_etat = 1;
    fin = 0;
  }
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
    file = fs.open(noms_file);
    if (Mode_leccture == 0) {
      if (file) {
        Serial.println(noms_file);
        i = 1;
        do {
          carlu = file.read();  // essaie de lire un caractère
                             // Serial.print(carlu);
          if (init_save >= 1) {
              Serial.print(carlu); 
            if ((carlu == ';') or ((Cuiseur == 3) and (carlu == '.'))) {

              if (Save_Send_GSM == 1) {
             
                switch (Cuiseur) {

                  case 0:
                    Matricule = dt_read;
                    Cuiseur = 1;
                  
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
                    Serial.println("");
                    Serial.print("Matricule : ");
                    Serial.println(Matricule);
                    Serial.print("Nom : ");
                    Serial.println(Nom);
                    Serial.print("Classe : ");
                    Serial.println(Classe);
                    Serial.print("Numero : ");
                    Serial.println(Numero);
                    i = 0;
                    fin = 2;
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
                  //Serial.println(ID_read);
                  if (ID_read.indexOf(ID_cmp) >= 0) {
                    Save_Send_GSM = 1;
                  }
                }
                //pos = 0;
              }
              for (y = 0; y < 51; y++) dt_read[y] = 0;
              pos = 0;
            } else if (carlu == '.') {
             ;  // Serial.println("fin de la ligne");
              init_save += 1;
            } else if (carlu == ')') {
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
void read_time() {
  DateTime now = rtc.now();
  Date = String(now.day()) + "/" + String(now.month()) + "/" + String(now.year());
  //Serial.print("la date est :");
  // Serial.println(date);
  Heure = String(now.hour()) + "H " + String(now.minute()) + "min ";
  Serial.print("l'heurs est : ");
  Serial.println(Heure);
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
