
//Most of the print to the serial monitor was commented because it was interfering with the database. We left it however, to show that 
 //those information can be uncommented and printed to the serial monitor in the absence of an lcd.

//The following codes were written using the Adafruit Fingerprint Sensor Library as foundation
// This code can be run to verify fingerprints.

#include <Adafruit_Fingerprint.h>


#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
// For UNO and others without hardware serial, we must use software serial...
// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)
// Set up the serial port to use softwareserial..
SoftwareSerial mySerial(2, 3);
 
#else
// On Leonardo/M0/etc, others with hardware serial, use hardware serial!
// #0 is green wire, #1 is white
#define mySerial Serial1

#endif

//This is used to store students' name and their corresponding fingerIDs so that the students' names can be matched and displayed on the lcd.
int studentID[50] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38};

char* studentName[50] = {"Ewura Ama", "Randy", "Marvellous", "Emmanuel", "Kelvin", "Greta", "Salia", "Ayeyi", "Angela", "Daniel T.", "Isabel", "Nhyira", "Nyameye", "Hawa",
"Ben Charles", "Isaac A.", "Gislaine", "Ekow", "Joyeuse", "Leeroy", "Aline", "Kirsty", "Samuel", "Martha", "Rovina", "Adwoa A.", "Juliann", "Ewura Abena", "Thomas",
"McKayla", "Elikem", "Julia", "Augustus", "Obed", "Alice", "Jonathan", "Ronnel", "Ama"
};


int relayPin=11;
int ledPinR =9;
int ledPinG =8;
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DS3231.h>
LiquidCrystal_I2C lcd(0x27,16,2);
DS3231  rtc(SDA, SCL);


Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

void setup()
{
  pinMode(relayPin, OUTPUT);
  pinMode(ledPinR, OUTPUT);
  pinMode(ledPinG, OUTPUT);
  rtc.begin();
  lcd.init();
  lcd.backlight();

 // The following lines can be uncommented to set the date and time
 
// rtc.setDOW(TUESDAY);     
// rtc.setTime(11, 50, 0);     
// rtc.setDate(31, 5,2022);   
  
  Serial.begin(9600);
  while (!Serial);  
  delay(100);
  Serial.println("\n\nAdafruit finger detect test");

  // set the data rate for the sensor serial port
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
  Serial.print(F("Security level: ")); Serial.println(finger.security_level);
  Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);

  finger.getTemplateCount();

  if (finger.templateCount == 0) {
    Serial.print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
  }
  else {
    Serial.println("Waiting for valid finger...");
      Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  }
   // Print date to the serial monitor
  Serial.println(rtc.getDateStr()); 
}


void loop()                     // Codes in the void loop run over and over again
{
  getFingerprintID();
  delay(50);            //Delaying the time between two consecutive runs
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
//      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
//      Serial.println("No finger detected");
      digitalWrite(relayPin, HIGH);
      lcd.clear();
      lcd.setCursor(3,0);
      lcd.print("Scan finger");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
//      Serial.println("Communication error");
       lcd.clear();
       lcd.setCursor(5,0);
       lcd.println("ERROR!!!");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

 
  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
//      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
//      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
//      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
//      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
//      Serial.println("Could not find fingerprint features");
      return p;
    default:
//      Serial.println("Unknown error");
      return p;

     
  }

  // The following should be executed if the fingerprint finds a match
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
//    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
//    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
//    Serial.println("Did not find a match");
     digitalWrite(ledPinR, HIGH);
     delay(1000);
     digitalWrite(ledPinR,LOW);
     lcd.clear();
     lcd.setCursor(1,1);
     lcd.print("Match not found");
     delay(1000);
    
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  // The following should be executed if the fingerprint sensor finds a match.

  bool found = false;
  for (byte n = 0; n < 50; n++)
  {
    if (finger.fingerID == studentID[n]){
    digitalWrite(ledPinG, HIGH);
    delay(1000);
    digitalWrite(ledPinG,LOW);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Hello ");
    lcd.print(studentName[n]);
    lcd.setCursor(0,1);
    lcd.print("Time: ");
    lcd.print(rtc.getTimeStr());
    delay(2000);
    Serial.print(studentName[n]);
    Serial.print(" ");
  Serial.println(rtc.getTimeStr()); 
   digitalWrite(relayPin, LOW);
   delay(5000);
    digitalWrite(relayPin, HIGH);

  
  // Wait one second before repeating :)
  delay (1000);
    
//    Serial.println("Found ID #"); 
//    Serial.print(finger.fingerID);
//    Serial.print(" with confidence of "); Serial.println(finger.confidence);

    found = true;
       }
     }

//     if (!found)
//     Serial.println("Fingerprint owner unknown");
//   
//     return finger.fingerID;
   
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
//  Serial.print("Found ID #");
//Serial.print(finger.fingerID);
//  Serial.print(" with confidence of "); Serial.println(finger.confidence);

   bool found = false;
  for (byte n = 0; n < 50; n++)
  {
    if (finger.fingerID == studentID[n]){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Hello ");
    lcd.print(studentName[n]);
    lcd.setCursor(0,1);
    lcd.print("Time: ");
    lcd.print(rtc.getTimeStr());
    delay(2000);
//    Serial.print("Welcome, ");
    Serial.print(studentName[n]);
//    Serial.print("Found ID #"); Serial.print(finger.fingerID);
//    Serial.print(" with confidence of "); Serial.println(finger.confidence);
    found = true;
       }
     }

     if (!found)
     Serial.println("Fingerprint owner unknown");
     return finger.fingerID;


  
}
