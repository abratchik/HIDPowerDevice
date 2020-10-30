/*
  HIDPowerDevice.h

  Copyright (c) 2020, Aleksandr Bratchik
  Original code (pre-library): Copyright (c) 2020, Aleksandr Bratchik

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef HIDPOWERDEVICE_h
#define HIDPOWERDEVICE_h

#define _USING_HID



#include "HID/HID.h"
#include "WString.h"


#if !defined(_USING_HID)

#warning "Using legacy HID core (non pluggable)"

#else

//================================================================================
//================================================================================

#define HID_PD_IPRODUCT              0x01
#define HID_PD_SERIAL                0x02
#define HID_PD_MANUFACTURER          0x03

#define HID_PD_RECHARGEABLE          0x06
#define HID_PD_PRESENTSTATUS         0x07
#define HID_PD_REMAINTIMELIMIT       0x08
#define HID_PD_MANUFACTUREDATE       0x09
#define HID_PD_CONFIGVOLTAGE         0x0A
#define HID_PD_VOLTAGE               0x0B
#define HID_PD_REMAININGCAPACITY     0x0C
#define HID_PD_RUNTIMETOEMPTY        0x0D
#define HID_PD_FULLCHRGECAPACITY     0x0E 
#define HID_PD_WARNCAPACITYLIMIT     0x0F
#define HID_PD_CPCTYGRANULARITY1     0x10
#define HID_PD_REMNCAPACITYLIMIT     0x11
#define HID_PD_DELAYBE4SHUTDOWN      0x12
#define HID_PD_DELAYBE4REBOOT        0x13
#define HID_PD_AUDIBLEALARMCTRL      0x14
#define HID_PD_CURRENT               0x15
#define HID_PD_CAPACITYMODE          0x16
#define HID_PD_DESIGNCAPACITY        0x17
#define HID_PD_CPCTYGRANULARITY2     0x18


class HIDPowerDevice_ 
{
private:
  // KeyReport _keyReport;
  //void sendReport(KeyReport* keys);
public:
  HIDPowerDevice_(void);
  void begin(void);
  void end(void);
  
  void sendDate(uint8_t id, uint16_t year, uint8_t month, uint8_t day);
  void sendByte(uint8_t id, uint8_t bval);
  void sendInt16(uint8_t id, uint16_t bval);
  void sendInt32(uint8_t id, uint32_t bval);
};

extern HIDPowerDevice_ PowerDevice;


#endif
#endif

