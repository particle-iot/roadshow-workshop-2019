/*
 * Project lab2
 * Description: 
 * Author: Brandon Satrom <brandon@particle.io>
 * Date: July 22, 2019
 */
#include "Particle.h"
#include "Grove_Temperature_And_Humidity_Sensor.h"
#include "Grove_ChainableLED.h"
#include "DiagnosticsHelperRK.h"

const unsigned long UPDATE_INTERVAL = 2000;
unsigned long lastUpdate = 0;

// Private battery and power service UUID
const BleUuid serviceUuid("5c1b9a0d-b5be-4a40-8f7a-66b36d0a5176");

BleCharacteristic batStateCharacteristic("batState", BleCharacteristicProperty::NOTIFY, BleUuid("fdcf4a3f-3fed-4ed2-84e6-04bbb9ae04d4"), serviceUuid);
BleCharacteristic powerSourceCharacteristic("powerSource", BleCharacteristicProperty::NOTIFY, BleUuid("cc97c20c-5822-4800-ade5-1f661d2133ee"), serviceUuid);
BleCharacteristic batLevelCharacteristic("batLevel", BleCharacteristicProperty::NOTIFY, BleUuid("d2b26bf3-9792-42fc-9e8a-41f6107df04c"), serviceUuid);

DHT dht(D2);
ChainableLED leds(A4, A5, 1);

int toggleLed(String args);

int temp, humidity;
double currentLightLevel;

void configureBLE()
{
  BLE.addCharacteristic(batStateCharacteristic);
  BLE.addCharacteristic(powerSourceCharacteristic);
  BLE.addCharacteristic(batLevelCharacteristic);

  BleAdvertisingData advData;

  // Advertise our private service only
  advData.appendServiceUUID(serviceUuid);

  // Continuously advertise when not connected
  BLE.advertise(&advData);
}

void setup()
{
  Serial.begin(9600);

  dht.begin();

  leds.init();
  leds.setColorHSB(0, 0.0, 0.0, 0.0);

  pinMode(A0, INPUT);

  Particle.variable("temp", temp);
  Particle.variable("humidity", humidity);

  Particle.function("toggleLed", toggleLed);

  configureBLE();
}

void loop()
{
  unsigned long currentMillis = millis();

  if (currentMillis - lastUpdate >= UPDATE_INTERVAL)
  {
    lastUpdate = millis();

    temp = (int)dht.getTempFarenheit();
    humidity = (int)dht.getHumidity();

    Serial.printlnf("Temp: %f", temp);
    Serial.printlnf("Humidity: %f", humidity);

    double lightAnalogVal = analogRead(A0);
    currentLightLevel = map(lightAnalogVal, 0.0, 4095.0, 0.0, 100.0);

    if (currentLightLevel > 50)
    {
      Particle.publish("light-meter/level", String(currentLightLevel), PRIVATE);
    }

    if (BLE.connected())
    {
      uint8_t powerSource = (uint8_t)DiagnosticsHelper::getValue(DIAG_ID_SYSTEM_POWER_SOURCE);
      powerSourceCharacteristic.setValue(powerSource);

      uint8_t batState = (uint8_t)DiagnosticsHelper::getValue(DIAG_ID_SYSTEM_BATTERY_STATE);
      batStateCharacteristic.setValue(batState);

      uint8_t batLevel = (uint8_t)(DiagnosticsHelper::getValue(DIAG_ID_SYSTEM_BATTERY_CHARGE) >> 8);
      batLevelCharacteristic.setValue(batLevel);
    }
  }
}

int toggleLed(String args)
{
  leds.setColorHSB(0, 0.0, 1.0, 0.5);

  delay(1000);

  leds.setColorHSB(0, 0.0, 0.0, 0.0);

  return 1;
}