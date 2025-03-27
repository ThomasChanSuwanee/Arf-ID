/**
 * R200 RFID demonstration
 * Copyright (c) 2023 Alastair Aitchison, Playful Technology
 */ 

// INCLUDES

#include "R200.h"

#define LOOP_INTERVAL 1000

// GLOBALS
unsigned long lastResetTime = 0;
R200 rfid;

void setup() 
{
  // Intitialise Serial connection (for debugging)
  Serial.begin(115200);
  Serial.println(__FILE__ __DATE__);

  rfid.begin(&Serial2, 115200, 16, 17);

  // Get info
  rfid.dumpModuleInfo();

  // Set multiple polling mode to true
    rfid.setMultiplePollingMode(0);
  
}

void loop() 
{
  rfid.loop();

  // Periodically re-send the read command
  if(millis() - lastResetTime > 1000)
  {
    //  digitalWrite(LED_BUILTIN, HIGH);
    
    //rfid.dumpUIDToSerial();
    //rfid.dumpModuleInfo();
    //  digitalWrite(LED_BUILTIN, LOW);

    //rfid.poll();
    //Serial.println("Starting getTransmit!");
    rfid.getTransmitPower();
    delay(100);
    //Serial.println("\n\n\n\n");
    //Serial.println("Starting setTransmit!");
    rfid.setTransmitPower(0x09, 0xC4); // 20 dBm

/*
    delay(1000);

    rfid.getTransmitPower();
    delay(100);
    rfid.setTransmitPower(0x09, 0xC4); // 25 dBm

    delay(1000);
*/  
    //rfid.setMultiplePollingMode(1);

    lastResetTime = millis();
  }
  delay(1000);
}