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
    rfid.getTransmitPower();
    Serial.println("Transmit");
    
    //rfid.setMultiplePollingMode(1);

    lastResetTime = millis();
  }
  delay(1000);
}