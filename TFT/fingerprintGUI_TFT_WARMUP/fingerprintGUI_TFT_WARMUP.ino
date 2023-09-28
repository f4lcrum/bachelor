/*
 Code in fingerprint related functions were taken from examples of Adafruit Fingerprint library.
 Modified by Kristián Oravec 
*/

#include <Adafruit_GFX.h>
#include "Arduino-STM32-8bitTFT.h"
#include <Adafruit_Fingerprint.h>
#define FAIL -1


HardwareSerial Serial3(PB11, PB10);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial3);
STM32_TFT_8bit tft;
uint32_t ID;
uint8_t enroll_id = 1;

void clear_screen() {
    tft.fillScreen(BLACK);
    tft.setTextColor(WHITE);        
}

void print_command(char * cmd_name, char * description) {
    tft.setTextSize(2);
    tft.setTextColor(YELLOW);
    tft.print(cmd_name);
    tft.setTextColor(WHITE);
    tft.println(description);    
}

void setup(void) {
    delay(1000);
    Serial.begin(115200);
    while (!Serial);  // For Yun/Leo/Micro/Zero/...
    delay(100);

    ID = tft.readID();
    tft.begin(ID);
    tft.fillScreen(BLACK);
    tft.setCursor(0, 0);
    tft.setTextColor(WHITE);  tft.setTextSize(2);
    tft.println("Finger detect test");

    // set the data rate for the sensor serial port
    finger.begin(57600);
    delay(5);
    if (finger.verifyPassword()) {
        tft.println("\n\nFound fingerprint sensor!");
    } else {
        tft.println("\n\nDid not find fingerprint sensor");
        while (1) { delay(1); }
    }
    finger.getTemplateCount();
    delay(200);
    tft.println("Booting...");
    if (finger.templateCount == 0) {
        tft.setTextColor(YELLOW);
        tft.println("Sensor doesn't contain any fingerprint data. Please run the 'enroll' command.");     
    } else {
        tft.print("Sensor contains "); tft.print(finger.templateCount); tft.println(" templates");    
    }
    delay(2000);
    
    
    uint16_t ID = tft.readID();
    commands();
}

void commands() {
    clear_screen();
    tft.setCursor(40, 0);
    tft.setTextSize(3);
    tft.println("Commands");
    tft.println();
    tft.println();

    print_command("INSTRUCTIONS: ", "Write a command as Serial monitor input.");
    print_command("\'enroll\': ", "Enroll a new fingerprint");
}

uint8_t readnumber(void) {
  uint8_t num = 0;

  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}

void enroll_fingerprint() {
    clear_screen();
    tft.setCursor(40, 0); tft.setTextSize(3);
    tft.println("Enroll");  
    tft.setTextSize(1);
    tft.println("Ready to enroll a fingerprint!");
    tft.println("Open Serial monitor");
    Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as...");
    enroll_id = readnumber();
    tft.print("Enrolling ID #");
    tft.println(enroll_id);

    while (getFingerprintEnroll() != true); 
    tft.setTextSize(3);
    tft.setTextColor(GREEN);
    tft.println("\n  Success!");
    
    delay(2000);       
}

uint8_t getFingerprintEnroll() {
    int p = -1;
    tft.print("Waiting for valid finger to enroll as #"); tft.println(enroll_id);
    while (p != FINGERPRINT_OK) {
        p = finger.getImage();
        switch (p) {
            case FINGERPRINT_OK:
                Serial.println("Image taken");
                break;
            case FINGERPRINT_NOFINGER:
                Serial.println(".");
                break;
            case FINGERPRINT_PACKETRECIEVEERR:
                Serial.println("Communication error");
                break;
            case FINGERPRINT_IMAGEFAIL:
                Serial.println("Imaging error");
                break;
            default:
                Serial.println("Unknown error");
                break;
        }
    }

    // OK success!

    p = finger.image2Tz(1);
    switch (p) {
        case FINGERPRINT_OK:
            Serial.println("Image converted");
            break;
        case FINGERPRINT_IMAGEMESS:
            Serial.println("Image too messy");
            return p;
        case FINGERPRINT_PACKETRECIEVEERR:
            Serial.println("Communication error");
            return p;
        case FINGERPRINT_FEATUREFAIL:
            Serial.println("Could not find fingerprint features");
            return p;
        case FINGERPRINT_INVALIDIMAGE:
            Serial.println("Could not find fingerprint features");
            return p;
        default:
            Serial.println("Unknown error");
            return p;
    }

    tft.println("Remove finger");
    delay(2000);
    p = 0;
    while (p != FINGERPRINT_NOFINGER) {
        p = finger.getImage();
    }
    Serial.print("ID "); Serial.println(enroll_id);
    p = -1;
    tft.println("Place same finger again");
    while (p != FINGERPRINT_OK) {
        p = finger.getImage();
        switch (p) {
        case FINGERPRINT_OK:
            Serial.println("Image taken");
            break;
        case FINGERPRINT_NOFINGER:
            Serial.print(".");
            break;
        case FINGERPRINT_PACKETRECIEVEERR:
            Serial.println("Communication error");
            break;
        case FINGERPRINT_IMAGEFAIL:
            Serial.println("Imaging error");
            break;
        default:
            Serial.println("Unknown error");
        break;
    }
  }

  // OK success!

    p = finger.image2Tz(2);
    switch (p) {
    case FINGERPRINT_OK:
        Serial.println("Image converted");
        break;
    case FINGERPRINT_IMAGEMESS:
        Serial.println("Image too messy");
        return p;
    case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        return p;
    case FINGERPRINT_FEATUREFAIL:
        Serial.println("Could not find fingerprint features");
        return p;
    case FINGERPRINT_INVALIDIMAGE:
        Serial.println("Could not find fingerprint features");
        return p;
    default:
        Serial.println("Unknown error");
        return p;
  }

  // OK converted!
    tft.print("Creating model for #");  tft.println(enroll_id);

    p = finger.createModel();
    if (p == FINGERPRINT_OK) {
        Serial.println("Prints matched!");
    } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
        Serial.println("Communication error");
        return p;
    } else if (p == FINGERPRINT_ENROLLMISMATCH) {
        Serial.println("Fingerprints did not match");
        return p;
    } else {
        Serial.println("Unknown error");
        return p;
    }

    Serial.print("ID "); Serial.println(enroll_id);
    p = finger.storeModel(enroll_id);
    if (p == FINGERPRINT_OK) {
        tft.println("Stored!");
    } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
        Serial.println("Communication error");
        return p;
    } else if (p == FINGERPRINT_BADLOCATION) {
        Serial.println("Could not store in that location");
        return p;
    } else if (p == FINGERPRINT_FLASHERR) {
        Serial.println("Error writing to flash");
        return p;
    } else {
        Serial.println("Unknown error");
        return p;
    }
  return true;
}
void parse_cmd(String cmd) {
    if (cmd == "enroll") {
        enroll_fingerprint();
    
    } else {
        commands();
        Serial.println("Invalid command, reroute to \'commands\'");    
    }
}

void loop() {
   String input;
    if (Serial.available() > 0) {
        delay(10); 
        input = Serial.readStringUntil('\n'); 
        parse_cmd(input);   
    }
}
