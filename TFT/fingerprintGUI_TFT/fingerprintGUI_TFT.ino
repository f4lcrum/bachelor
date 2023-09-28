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

void print_command(char * cmd_name, char * description) {
    tft.setTextSize(2);
    tft.setTextColor(YELLOW);
    tft.print(cmd_name);
    tft.setTextColor(WHITE);
    tft.println(description);    
}


void commands() {
    clear_screen();
    tft.setCursor(40, 0);
    tft.setTextSize(3);
    tft.println("Commands");
    tft.println();
    tft.println();

    print_command("INSTRUCTIONS: ", "Write a command as Serial monitor input.");
    print_command("\'identify\':", "Verifies if your fingerprint is in database");
    print_command("\'commands\': ", "Show this commands list");
    print_command("\'enroll\': ", "Enroll a new fingerprint");
    print_command("\'erase\': ", "Erase database of fingerprints");
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

void identify() {
    clear_screen();
    tft.setCursor(0, 0);
    tft.setTextSize(1.5);
    tft.println("Put finger on scanner. To return to menu, type \'menu\' to Serial monitor input");
    tft.println();
    tft.println();
    tft.setTextSize(1);
    int id = -1; // implicitly it sets up to 0 
    while (true) {
        id = get_ID();
        if (Serial.available() > 0) break; // new input in serial monitor
        delay(50);       
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

void parse_cmd(String cmd) {
    if (cmd == "identify") {
       identify();
    } else if (cmd == "enroll") {
        enroll_fingerprint();
    } else if (cmd == "commands") {
       commands();        
    } else if (cmd == "erase") {
        erase();
    } else {
        commands();
        Serial.println("Invalid command, reroute to \'commands\'");    
    }

}

void erase() {
    clear_screen();
    tft.setCursor(40, 0); tft.setTextSize(3);
    tft.println("Erase");  
    tft.setTextSize(2);
    tft.println("Erasing in progress");
    finger.emptyDatabase();
    delay(500);
    tft.println("Erased!");
    delay(500);   
    commands(); // return to commands menu 
}

// returns -1 if failed, otherwise returns ID #
int get_ID() {
    uint8_t p = finger.getImage();
    switch (p) {
        case FINGERPRINT_OK:
            Serial.println("Image taken");
            break;
        case FINGERPRINT_NOFINGER:
            Serial.println("No finger detected");
            return p;
        case FINGERPRINT_PACKETRECIEVEERR:
            Serial.println("Communication error");
            return p;
        case FINGERPRINT_IMAGEFAIL:
            Serial.println("Imaging error");
            return p;
        default:
            Serial.println("Unknown error");
            return p;
    }

      // OK success!
    
    p = finger.image2Tz();
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
      p = finger.fingerSearch();
      if (p == FINGERPRINT_OK) {
          Serial.println("Found a print match!");
      } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
          Serial.println("Communication error");
          return p;
      } else if (p == FINGERPRINT_NOTFOUND) {
          tft.setTextColor(RED);
          tft.println("Did not find a match");
          return p;
      } else {
          Serial.println("Unknown error");
          return p;
      }
    tft.setTextColor(GREEN);
    // found a match!
    tft.print("Found ID #"); tft.print(finger.fingerID); 
    tft.print(" with confidence of "); tft.println(finger.confidence);
    return p;
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
