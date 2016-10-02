#include <CurieBLE.h>
#include <Servo.h>

BLEPeripheral blePeripheral;       // BLE Peripheral Device (the board you're programming)
BLEService phonethrottleService("180F"); // BLE Battery Service
BLEUnsignedCharCharacteristic phonethrottleChar("2A19", BLERead | BLENotify); 

int currentThrottleLevel = 0;
long previousMillis = 0; 
Servo motor; 

void setup() {
  Serial.begin(9600);
  motor.attach(6);

  blePeripheral.setLocalName("PhoneThrottleSketch");
  blePeripheral.setAdvertisedServiceUuid(phonethrottleService.uuid());  // add the service UUID
  blePeripheral.addAttribute(phonethrottleService);   // Add the BLE Battery service
  blePeripheral.addAttribute(phonethrottleChar); // add the battery level characteristic

  blePeripheral.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  blePeripheral.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);

  phonethrottleChar.setEventHandler(BLEWritten, switchCharacteristicWritten);
  phonethrottleChar.setValue(0);
  
  blePeripheral.begin();
  Serial.println("Bluetooth device active, waiting for connections...");
}

void loop() {
  blePeripheral.poll();
}

void blePeripheralConnectHandler(BLECentral& central) {
  // central connected event handler
  Serial.print("Connected event, central: ");
  Serial.println(central.address());

  delay(2000); //Delay arming sequence
  motor.write(40); //Arm the ESC
}

void blePeripheralDisconnectHandler(BLECentral& central) {
  // central disconnected event handler
  Serial.print("Disconnected event, central: ");
  Serial.println(central.address());
}

void switchCharacteristicWritten(BLECentral& central, BLECharacteristic& characteristic) {
  // central wrote new value to characteristic, update LED
  Serial.print("Updated Throttle Level");
  motor.write(phonethrottleChar.value());
}
