#include <SoftwareSerial.h>
#include "RTClib.h"
RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };

#define PIN_TX 8
#define PIN_RX 9
#define BAUDRATE 9600
SoftwareSerial mySerial(10, 11);  // RX, TX
String receive;
boolean bp, bp_pres;
unsigned long temps;
int p;
void setup() {
  pinMode(6, INPUT_PULLUP);
  Serial.begin(115200);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Goodnight moon!");

  // set the data rate for the SoftwareSerial port
  mySerial.begin(57600);
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
   //   rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  //rtc.adjust(DateTime(2023, 1, 8, 18, 55, 0));
}

void loop() {  // run over and over
  DateTime now = rtc.now();
  String Date = String(now.day()) + "/" + String(now.month()) + "/" + String(now.year());
  //Serial.print("la date est :");
  // Serial.println(date);
  String Heure = String(now.hour()) + "H" + String(now.minute()) + "min ";
  /*Serial.print("l'heurs est : ");
  */Serial.println(Heure);
  /*/ while (mySerial.available()) {
    Serial.print("le message : ");
    */
  receive = mySerial.readString();
  Serial.println(mySerial.readString());
  // receive.trim();
  Serial.println(receive);
  int num_char = receive.length();
  Serial.println(num_char);
  bp = digitalRead(6);
  delay(500);

  if (bp == 1) {
    temps = millis();
    Serial.println("ok");
    mySerial.begin(57600);
   // delay(500);    
    mySerial.print("..D" + Date + ";" + Heure + "D");
    p = 1;
  }
  if ((bp == 1) and (millis() > (temps + 100) and (p == 1))) {

    Serial.print(Heure);
    p = 0;
  }
  // }
}