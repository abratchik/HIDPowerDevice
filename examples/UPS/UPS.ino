#include <HIDPowerDevice.h>

#define MINUPDATEINTERVAL   26
#define CHGDCHPIN           4
#define RUNSTATUSPIN        5
#define COMMLOSTPIN         10
#define BATTSOCPIN          A7

int iIntTimer=0;


// String constants 
const char STRING_DEVICECHEMISTRY[] PROGMEM = "PbAc";
const char STRING_OEMVENDOR[] PROGMEM = "MyCoolUPS";
const char STRING_SERIAL[] PROGMEM = "UPS10"; 

const byte bDeviceChemistry = IDEVICECHEMISTRY;
const byte bOEMVendor = IOEMVENDOR;

PresentStatus iPresentStatus = {}, iPreviousStatus = {};

byte bRechargable = 1;
byte bCapacityMode = 2;  // units are in %%

// Physical parameters
const uint16_t iConfigVoltage = 1380;
uint16_t iVoltage =1300, iPrevVoltage = 0;
uint16_t iRunTimeToEmpty = 0, iPrevRunTimeToEmpty = 0;
uint16_t iAvgTimeToFull = 7200;
uint16_t iAvgTimeToEmpty = 7200;
uint16_t iRemainTimeLimit = 600;
int16_t  iDelayBe4Reboot = -1;
int16_t  iDelayBe4ShutDown = -1;
byte iAudibleAlarmCtrl = 2; // 1 - Disabled, 2 - Enabled, 3 - Muted


// Parameters for ACPI compliancy
const byte iDesignCapacity = 100;
byte iWarnCapacityLimit = 10; // warning at 10% 
byte iRemnCapacityLimit = 5; // low at 5% 
const byte bCapacityGranularity1 = 1;
const byte bCapacityGranularity2 = 1;
byte iFullChargeCapacity = 100;

byte iRemaining =0, iPrevRemaining=0;

int iRes=0;


void setup() {

  Serial.begin(57600);
    
  PowerDevice.begin();
  
  // Serial No is set in a special way as it forms Arduino port name
  PowerDevice.setSerial(STRING_SERIAL); 
  
  // Used for debugging purposes. 
  PowerDevice.setOutput(Serial);
  
  pinMode(CHGDCHPIN, INPUT_PULLUP); // ground this pin to simulate power failure. 
  pinMode(RUNSTATUSPIN, OUTPUT);  // output flushing 1 sec indicating that the arduino cycle is running. 
  pinMode(COMMLOSTPIN, OUTPUT); // output is on once communication is lost with the host, otherwise off.


  PowerDevice.setFeature(HID_PD_PRESENTSTATUS, &iPresentStatus, sizeof(iPresentStatus));
  
  PowerDevice.setFeature(HID_PD_RUNTIMETOEMPTY, &iRunTimeToEmpty, sizeof(iRunTimeToEmpty));
  PowerDevice.setFeature(HID_PD_AVERAGETIME2FULL, &iAvgTimeToFull, sizeof(iAvgTimeToFull));
  PowerDevice.setFeature(HID_PD_AVERAGETIME2EMPTY, &iAvgTimeToEmpty, sizeof(iAvgTimeToEmpty));
  PowerDevice.setFeature(HID_PD_REMAINTIMELIMIT, &iRemainTimeLimit, sizeof(iRemainTimeLimit));
  PowerDevice.setFeature(HID_PD_DELAYBE4REBOOT, &iDelayBe4Reboot, sizeof(iDelayBe4Reboot));
  PowerDevice.setFeature(HID_PD_DELAYBE4SHUTDOWN, &iDelayBe4ShutDown, sizeof(iDelayBe4ShutDown));
  
  PowerDevice.setFeature(HID_PD_RECHARGEABLE, &bRechargable, sizeof(bRechargable));
  PowerDevice.setFeature(HID_PD_CAPACITYMODE, &bCapacityMode, sizeof(bCapacityMode));
  PowerDevice.setFeature(HID_PD_CONFIGVOLTAGE, &iConfigVoltage, sizeof(iConfigVoltage));
  PowerDevice.setFeature(HID_PD_VOLTAGE, &iVoltage, sizeof(iVoltage));

  PowerDevice.setStringFeature(HID_PD_IDEVICECHEMISTRY, &bDeviceChemistry, STRING_DEVICECHEMISTRY);
  PowerDevice.setStringFeature(HID_PD_IOEMINFORMATION, &bOEMVendor, STRING_OEMVENDOR);

  PowerDevice.setFeature(HID_PD_AUDIBLEALARMCTRL, &iAudibleAlarmCtrl, sizeof(iAudibleAlarmCtrl));

  PowerDevice.setFeature(HID_PD_DESIGNCAPACITY, &iDesignCapacity, sizeof(iDesignCapacity));
  PowerDevice.setFeature(HID_PD_FULLCHRGECAPACITY, &iFullChargeCapacity, sizeof(iFullChargeCapacity));
  PowerDevice.setFeature(HID_PD_REMAININGCAPACITY, &iRemaining, sizeof(iRemaining));
  PowerDevice.setFeature(HID_PD_WARNCAPACITYLIMIT, &iWarnCapacityLimit, sizeof(iWarnCapacityLimit));
  PowerDevice.setFeature(HID_PD_REMNCAPACITYLIMIT, &iRemnCapacityLimit, sizeof(iRemnCapacityLimit));
  PowerDevice.setFeature(HID_PD_CPCTYGRANULARITY1, &bCapacityGranularity1, sizeof(bCapacityGranularity1));
  PowerDevice.setFeature(HID_PD_CPCTYGRANULARITY2, &bCapacityGranularity2, sizeof(bCapacityGranularity2));

  PowerDevice.sendManufacturerDate(2024, 10, 12);
}

void loop() {
  
  
  //*********** Measurements Unit ****************************
  bool bCharging = digitalRead(CHGDCHPIN);
  bool bACPresent = bCharging;    // TODO - replace with sensor
  bool bDischarging = !bCharging; // TODO - replace with sensor
  int iBattSoc = analogRead(BATTSOCPIN);       // TODO - this is for debug only. Replace with charge estimation

  iRemaining = (byte)(round((float)iFullChargeCapacity*iBattSoc/1024));
  iRunTimeToEmpty = (uint16_t)round((float)iAvgTimeToEmpty*iRemaining/iFullChargeCapacity);

  // Charging
  iPresentStatus.Charging = bCharging;
  iPresentStatus.ACPresent = bACPresent;
  iPresentStatus.FullyCharged = (iRemaining == iFullChargeCapacity);
    
  // Discharging
  if(bDischarging) {
    iPresentStatus.Discharging = 1;
    // if(iRemaining < iRemnCapacityLimit) iPresentStatus.BelowRemainingCapacityLimit = 1;
    
    iPresentStatus.RemainingTimeLimitExpired = (iRunTimeToEmpty < iRemainTimeLimit);

  }
  else {
    iPresentStatus.Discharging = 0;
    iPresentStatus.RemainingTimeLimitExpired = 0;
  }

  // Shutdown requested
  if(iDelayBe4ShutDown > 0 ) {
      iPresentStatus.ShutdownRequested = 1;
      Serial.println("shutdown requested");
  }
  else
    iPresentStatus.ShutdownRequested = 0;

  // Shutdown imminent
  if((iPresentStatus.ShutdownRequested) || 
     (iPresentStatus.RemainingTimeLimitExpired)) {
    iPresentStatus.ShutdownImminent = 1;
    Serial.println("shutdown imminent");
  }
  else
    iPresentStatus.ShutdownImminent = 0;


  
  iPresentStatus.BatteryPresent = 1;

  

  //************ Delay ****************************************  
  delay(1000);
  iIntTimer++;
  digitalWrite(RUNSTATUSPIN, HIGH);   // turn the LED on (HIGH is the voltage level);
  delay(1000);
  iIntTimer++;
  digitalWrite(RUNSTATUSPIN, LOW);   // turn the LED off;

  //************ Check if we are still online ******************

  

  //************ Bulk send or interrupt ***********************

  if((iPresentStatus != iPreviousStatus) || (iRemaining != iPrevRemaining) || (iRunTimeToEmpty != iPrevRunTimeToEmpty) || (iIntTimer>MINUPDATEINTERVAL) ) {

    PowerDevice.sendReport(HID_PD_REMAININGCAPACITY, &iRemaining, sizeof(iRemaining));
    if(bDischarging) PowerDevice.sendReport(HID_PD_RUNTIMETOEMPTY, &iRunTimeToEmpty, sizeof(iRunTimeToEmpty));
    iRes = PowerDevice.sendReport(HID_PD_PRESENTSTATUS, &iPresentStatus, sizeof(iPresentStatus));

    if(iRes <0 ) {
      digitalWrite(COMMLOSTPIN, HIGH);
    }
    else
      digitalWrite(COMMLOSTPIN, LOW);
        
    iIntTimer = 0;
    iPreviousStatus = iPresentStatus;
    iPrevRemaining = iRemaining;
    iPrevRunTimeToEmpty = iRunTimeToEmpty;
  }
  

  Serial.println(iRemaining);
  Serial.println(iRunTimeToEmpty);
  Serial.println(iRes);
  
}
