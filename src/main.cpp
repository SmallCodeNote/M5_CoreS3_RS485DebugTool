#include <Arduino.h>
#include <M5Unified.h>

// Core2 Port.C
// #define MODBUS_TX_PIN 14 //(HY2.0-4P/Yellow)Connect RS485 module Rx
// #define MODBUS_RX_PIN 13 //(HY2.0-4P/White)Connect RS485 module Tx

// CoreS3 series Port.C
#define MODBUS_TX_PIN 17 // Connect RS485 module Rx
#define MODBUS_RX_PIN 18 // Connect RS485 module Tx


#define MODBUS_BAUD 9600
// #define MODBUS_BAUD 1200

bool _direct = false;
byte request[] = {0x02, 0x03, 0x00, 0x00, 0x00, 0x0A, 0xC5, 0xFE};
int lastByteCount = 0;
long lastSerialAvailableMillis = -1;

void setup()
{
  auto cfg = M5.config();
  M5.begin(cfg);

  M5.Log.setLogLevel(m5::log_target_serial, ESP_LOG_VERBOSE);
  M5.setLogDisplayIndex(0);
  M5.Display.setTextScroll(true);

  Serial2.begin(MODBUS_BAUD, SERIAL_8E1, MODBUS_RX_PIN, MODBUS_TX_PIN);
  M5.Log.println("RS485 Test Ready");
}

void loop()
{
  M5.update();

  if (Serial2.available())
  {
    if (lastByteCount == 0)
    {
      M5.Log.printf("%d<: ", millis());
    }
    while (Serial2.available())
    {
      uint8_t b = Serial2.read();
      M5.Log.printf("%02X ", b);
      lastByteCount++;
      lastSerialAvailableMillis = millis();
    }
  }

  if (lastByteCount > 0 && millis() - lastSerialAvailableMillis > 200)
  {
    lastByteCount = 0;
    lastSerialAvailableMillis = millis();
    M5.Log.println("");
  }

  if (M5.BtnA.wasClicked())
  {
    Serial2.write(request, sizeof(request));
    Serial2.flush(); // wait tx buffer flush
    while(Serial2.available()){ Serial2.read();}; // echo data clear on rx buffer

    M5.Log.printf("%d>: ", millis());

    for (size_t i = 0; i < 8; i++)
    {
      M5.Log.printf("%02X ", request[i]);
    }

    M5.Log.println("");
  }

  vTaskDelay(1);
  yield();
}
