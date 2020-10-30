#include <HIDPowerDevice.h>

void setup() {
  // put your setup code here, to run once:
  PowerDevice.begin();
  pinMode(4, INPUT_PULLUP);

  Serial.begin(9600);
  
}

void loop() {
  //put your main code here, to run repeatedly:

  delay(2000);
  
  uint8_t raw[USB_EP_SIZE]={0};
  // Serial.println(USB_Available(HID_RX));
  if(Serial.available()) {
    
    int incomingByte = Serial.read();
    Serial.println(incomingByte, DEC);
  }
    
  if(USB_Available(HID_RX)) {
    Serial.print("USB_Available\t"); Serial.println(USB_Available(HID_RX));
  }

  // return;
  
  PowerDevice.sendByte(HID_PD_IPRODUCT, IPRODUCT);
  PowerDevice.sendByte(HID_PD_MANUFACTURER, IMANUFACTURER);
  PowerDevice.sendByte(HID_PD_SERIAL, ISERIAL);
  
  PowerDevice.sendByte(HID_PD_RECHARGEABLE, 1); // should be 1 (Rechargable). Equivalent to "Battery Technology" in ACPI. 
  PowerDevice.sendByte(HID_PD_CAPACITYMODE, 0);
  PowerDevice.sendInt32(HID_PD_FULLCHRGECAPACITY, 43200); //12 Ah (in Asec)
  PowerDevice.sendInt32(HID_PD_DESIGNCAPACITY, 43200); //12 Ah (in Asec)
  PowerDevice.sendInt16(HID_PD_CONFIGVOLTAGE, 13800); //13.8V
  PowerDevice.sendInt32(HID_PD_REMNCAPACITYLIMIT, 2160); //5% is set to threshold
  PowerDevice.sendByte(HID_PD_CPCTYGRANULARITY1, 100);
  PowerDevice.sendByte(HID_PD_CPCTYGRANULARITY2, 50);

  byte bRemaining = 75;
  
  PowerDevice.sendByte(HID_PD_REMAININGCAPACITY, bRemaining);

  int iPresentStatus = 0;
  
  // Charging
  bool bCharging = digitalRead(4);
  if(bCharging) {
    bitSet(iPresentStatus,0);
    // Fully Charged
    if(bRemaining == 100) bitSet(iPresentStatus, 12);

  }
  // Dischargig
  else {
    bitSet(iPresentStatus,1);
    // Fully Discharged
    if(bRemaining = 1) bitSet(iPresentStatus, 13);
  }

  // Need Replacement
  bitSet(iPresentStatus, 9);
  // Overload
  bitSet(iPresentStatus, 10);


  PowerDevice.sendInt16(HID_PD_PRESENTSTATUS, iPresentStatus);
  if(bCharging) {
    PowerDevice.sendInt16(HID_PD_CURRENT, 500);
  }
  else {
    PowerDevice.sendInt16(HID_PD_CURRENT, -500);
    PowerDevice.sendInt16(HID_PD_RUNTIMETOEMPTY, 3600);
  }
    
  PowerDevice.sendInt16(HID_PD_VOLTAGE, 12000);
  
}
