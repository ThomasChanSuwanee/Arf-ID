#include "M5Capsule.h"
#include "R200.h"
#include <M5Unified.h> // M5Stack
#include <esp_log.h>
#include <WiFi.h> // Arduino
#include "sdcard.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <WiFiManager.h> // Tzapu
#include "Arf_NTP.h"

/*** Make sure to set Tools -> Partition Scheme -> Huge APP(3MB no OTA) ***/
// Otherwise you run into a spacing issue

/*** Test Commit ***/

extern std::string outputStringBuffer;
static bool isRecording = true;

// rtc
// const char *ntpServer = "time.cloudflare.com";
// set GPIO46 while on to make it not sleep, otherwise set it to 0 to make it sleep

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define SERIAL2_RX 15
#define SEIRAL2_TX 13

const int deepsleep_pin = 46;
std::string logFile = "";
unsigned long lastResetTime = 0;
R200 rfid;

BLEServer *pServer;
BLEService *pService;
BLECharacteristic *pCharacteristic;
// BLECharacteristic *pControlCharacteristic; // for controlling recording state

// TODO refactor into enums
int deviceConnected = 0; // 0 = not connected at all, 1 = connected, 2 = disconnected, 3 = advertisement state set
class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    deviceConnected = 1;
  };
  void onDisconnect(BLEServer *pServer)
  {
    deviceConnected = 2;
  }
};

// class CharactisticCallBack : public BLECharacteristicCallbacks
// {
//   void onWrite(BLECharacteristic *characteristic)
//   {
//     std::string value = characteristic->getValue();
//     pControlCharacteristic->setValue(value); // echo back the value written
//   }
// };

void BLEsetup()
{
  BLEDevice::init("MyESP32"); // set the device name
  pServer = BLEDevice::createServer();
  pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ);
  pCharacteristic->setValue("");

  // // create another characteristic for controlling the recording state
  // pControlCharacteristic = pService->createCharacteristic(
  //     "d1e3f1a2-4c5b-4f8e-9c6b-7f3e1a2b3c4d", // unique UUID for control characteristic
  //     BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  // pControlCharacteristic->setCallbacks(new CharactisticCallBack());
  // pControlCharacteristic->setValue("0"); // initial state is not recording
  // // set up the write callback to toggle recording state

  pServer->setCallbacks(new MyServerCallbacks());
  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);

  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}

void rfidSetup()
{
  rfid.begin(&Serial2, 115200, SERIAL2_RX, SEIRAL2_TX);
  // Get info
  rfid.dumpModuleInfo();
  rfid.setMultiplePollingMode(0);
  // set Tx power
  // rfid.setTransmitPower(0x00, 0x00);
  Serial.println("RFID setup complete.");
}

void rfidLoop()
{
  // Periodically re-send the read command
  if (millis() - lastResetTime > 500)
  {
    rfid.poll();
    // rfid.dumpUIDToSerial();
    // rfid.getModuleInfo();
    lastResetTime = millis();
  }
}

void wifiSetup()
{
  {
    // WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
    // it is a good practice to make sure your code sets wifi mode how you want it.

    // put your setup code here, to run once:
    Serial.begin(115200);

    // WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wm;

    // reset settings - wipe stored credentials for testing
    // these are stored by the esp library
    // wm.resetSettings();

    // Automatically connect using saved credentials,
    // if connection fails, it starts an access point with the specified name ( "AutoConnectAP"),
    // if empty will auto generate SSID, if password is blank it will be anonymous AP (wm.autoConnect())
    // then goes into a blocking loop awaiting configuration and will return success result

    bool res;
    // res = wm.autoConnect(); // auto generated AP name from chipid
    res = wm.autoConnect("AutoConnectAP"); // anonymous ap
    // res = wm.autoConnect("AutoConnectAP","password"); // password protected ap

    if (!res)
    {
      Serial.println("Failed to connect");
      // ESP.restart();
    }
    else
    {
      // if you get here you have connected to the WiFi
      Serial.println("connected...yeey :)");
    }
  }
}

void setupRTC()
{
  if (!M5Capsule.Rtc.isEnabled())
  {
    Serial.println("RTC not found.");
    for (;;)
    {
      vTaskDelay(500);
    }
  }
  time_t t = time(nullptr) + 1; // Advance one second.
  while (t > time(nullptr))
    ; /// Synchronization in seconds
  M5Capsule.Rtc.setDateTime(gmtime(&t));
}

void waitforBLEconnection()
{
  Serial.println("Waiting for connection...");
  while (!deviceConnected)
  {
    delay(1000);
  }
  Serial.println("Connected to client, waiting for button press");
}

void initalizeRTC()
{
  // get date from RTC
  auto dt = M5Capsule.Rtc.getDateTime();

  if (dt.date.year == 2000)
  {
    Serial.println("RTC not initialized, setting time from NTP.");
    wifiSetup();
    arf_ntp_setup();
    setupRTC(); // sets the date obtained from WIFI
    // disconnect from wifi to allow BLE
    WiFi.disconnect();
  }
}

void setupSDCard()
{
  std::string current_date = getDateString();
  logFile = SD_setup(current_date);
}

void setup()
{
  pinMode(deepsleep_pin, OUTPUT); // don't fall asleep
  digitalWrite(deepsleep_pin, HIGH);
  auto cfg = M5.config();
  M5Capsule.begin(cfg);
  Serial.begin(115200);
  Serial.println(__FILE__ __DATE__);
  Serial.println("setup");

  rfidSetup();
  initalizeRTC();

  BLEsetup();
  setupSDCard();
  Serial.println("start/stop recording...");
}

void playBeep()
{
}

void loop()
{
  // std::string rxValue = pControlCharacteristic->getValue();
  // if (rxValue.length() > 0)
  // {
  //   Serial.print("Received Value: ");
  //   for (int i = 0; i < rxValue.length(); i++)
  //     Serial.print(rxValue[i]);
  // }

  if (isRecording) // initally false, toggle by BLE
  {
    rfid.loop();

    if (outputStringBuffer != "")
    {
      // this is super hacky but it works
      outputStringBuffer.erase(std::find(outputStringBuffer.begin(), outputStringBuffer.end(), '\0'), outputStringBuffer.end());
      Serial.println(outputStringBuffer.c_str());
      // get the current time
      time_t now = time(nullptr);
      struct tm *timeinfo = gmtime(&now);
      char timeBuffer[40];
      strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", timeinfo);
      saveDataSD(logFile, outputStringBuffer, std::string(timeBuffer)); // save data to sd card

      pCharacteristic->setValue(outputStringBuffer + "," + std::string(timeBuffer));
      // playBeep();
      outputStringBuffer = "";
    }
  }

  // check if the device is still connected
  if (deviceConnected == 2) // device disconnected
  {
    Serial.println("Device disconnected, broadcasting...");
    BLEDevice::startAdvertising();
    deviceConnected = 3;
  }

  M5Capsule.update();
  // read the p control characteristic to see if recording state has changed
  // if (pControlCharacteristic->getValue() == "1") // start recording
  // {
  //   isRecording = !isRecording; // toggle recording state
  //   Serial.println("Recording started from BLE control.");
  //   pControlCharacteristic->setValue("0"); // reset the control characteristic to "0" to avoid re-triggering
  //   if (isRecording)
  //   {
  //     Serial.println("Recording started.");
  //     pCharacteristic->setValue("");
  //   }
  //   else
  //   {
  //     Serial.println("Recording stopped.");
  //     pCharacteristic->setValue("");
  //   }
  // }
  // else if (pControlCharacteristic->getValue() == "2")
  // {
  //   // start a new trial
  //   Serial.println("Starting a new trial...");
  //   setupSDCard();                         // re-initialize the SD card for a new trial
  //   pControlCharacteristic->setValue("0"); // reset the control characteristic to "0" to avoid re-triggering
  // }

  rfidLoop();
}