#include <SoftwareSerial.h>
#include <SPI.h>
#include <MFRC522.h>
#include "RTClib.h"

RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };


SoftwareSerial Serial_ESP_Read_UID(2, 3);  // RX, TX
#define RST_PIN 9                          // Configurable, see typical pin layout above
#define SS_1_PIN 10                        // Configurable, take a unused pin, only HIGH/LOW required, must be different to SS 2
#define SS_2_PIN 7                         // Configurable, take a unused pin, only HIGH/LOW required, must be different to SS 1

#define NR_OF_READERS 2
#define read_0 8
#define read_1 4
String UIDs_save, UIDs, buf1, buf2, buf3, buf4;
byte ssPins[] = { SS_1_PIN, SS_2_PIN };
boolean send;
MFRC522 mfrc522[NR_OF_READERS];  // Create MFRC522 instance.

/**
 * Initialize.
 */
uint8_t reader = 0;
void setup() {
  Serial.begin(115200);
#ifndef ESP8266
  while (!Serial)
    ;  // wait for serial port to connect. Needed for native USB
#endif

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    //rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  //rtc.adjust(DateTime(2023, 1, 6, 20, 13, 0));
  //   rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  pinMode(6, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(read_0, OUTPUT);
  pinMode(read_1, OUTPUT);
  // Initialize serial communications with the PC
  while (!Serial)
    ;  // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)

  SPI.begin();  // Init SPI bus

  for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {
    mfrc522[reader].PCD_Init(ssPins[reader], RST_PIN);  // Init each MFRC522 card
    Serial.print(F("Reader "));
    Serial.print(reader);
    Serial.print(F(": "));
    mfrc522[reader].PCD_DumpVersionToSerial();
  }
  Serial_ESP_Read_UID.begin(9600);
  DateTime now = rtc.now();
  String Date = String(now.day()) + "/" + String(now.month()) + "/" + String(now.year());
  //Serial.print("la date est :");
  // Serial.println(date);
  String Heure = String(now.hour()) + "H " + String(now.minute()) + "min ";
  Serial.print("l'heurs est : ");
  Serial.println(Heure);
  // Serial_ESP_Read_UID.println("Communication etablie pour l'envoi des IDs des Etudiants!!!");
}

/**
 * Main loop.
 */
void loop() {
  /* DateTime now = rtc.now();
  String Date = String(now.day()) + "/" + String(now.month()) + "/" + String(now.year());
  //Serial.print("la date est :");
  // Serial.println(date);
  String Heure = String(now.hour()) + "H " + String(now.minute()) + "min ";
  Serial.print("l'heurs est : ");
  Serial.println(Heure);*/
  for (reader = 0; reader < NR_OF_READERS; reader++) {
    // Look for new cards
    if (mfrc522[reader].PICC_IsNewCardPresent() && mfrc522[reader].PICC_ReadCardSerial()) {
      Serial.print(F("Reader "));
      Serial.print(reader);
      // Show some details of the PICC (that is: the tag/card)
      Serial.print(F(": Card UID:"));
      dump_byte_array(mfrc522[reader].uid.uidByte, mfrc522[reader].uid.size);
      Serial.println();
      Serial.print(F("PICC type: "));
      MFRC522::PICC_Type piccType = mfrc522[reader].PICC_GetType(mfrc522[reader].uid.sak);
      Serial.println(mfrc522[reader].PICC_GetTypeName(piccType));

      // Halt PICC
      mfrc522[reader].PICC_HaltA();
      // Stop encryption on PCD
      mfrc522[reader].PCD_StopCrypto1();
    }  //if (mfrc522[reader].PICC_IsNewC
  }    //for(uint8_t reader
}

/**
 * Helper routine to dump a byte array as hex values to Serial.
 */
void dump_byte_array(byte *buffer, byte bufferSize) {
  for (int i = 0; i < 4; i++) {
    String buf;
    buf = String(buffer[i], HEX);
    int num_char = buf.length();
    if (num_char < 2) buf = "0" + buf;
    if (i == 0) buf1 = buf;
    if (i == 1) buf2 = buf;
    if (i == 2) buf3 = buf;
    if (i == 3) buf4 = buf;
  }
  UIDs = buf1 + buf2 + buf3 + buf4;
  if (UIDs != UIDs_save) {
    UIDs_save = UIDs;
    if (reader == 0) {
      digitalWrite(read_0, 1);
      tone(6, 1200, 500);
      // tone(5, 1200, 500);
    }
    if (reader == 1) {
      // tone(5, 1200, 500);
      digitalWrite(read_1, 1);
      tone(6, 1200, 500);
    }
    delay(500);
    for (byte i = 0; i < bufferSize; i++) {
      Serial.print(buffer[i] < 0x10 ? " 0" : " ");
      Serial.print(buffer[i], HEX);

      digitalWrite(read_1, 0);
      digitalWrite(read_0, 0);
    }
    Serial.println(" ");
    //if(UIDs=="f9:31:de:b3") Serial.println("good save");
    DateTime now = rtc.now();
    String Date = String(now.day()) + "/" + String(now.month()) + "/" + String(now.year());
    //Serial.print("la date est :");
    // Serial.println(date);
    String Heure = String(now.hour()) + "H" + String(now.minute()) + "min ";
    Serial.print("l'heurs est : ");
    Serial.println(Heure);
    UIDs = UIDs + ";" + Date + ";" + Heure + ";";
    Serial_ESP_Read_UID.print(UIDs);
    delay(1000);

    Serial.println();
    Serial.println(UIDs);
  } else Serial.println("...");
}
