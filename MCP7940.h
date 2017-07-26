/*******************************************************************************************************************
** Class definition header for the MCP7940 from Microchip. Both the MCP7940N (with battery backup pin= and the    **
** MCP7940M are supported with this library and they both use the same I2C address. This chip is a Real-Time-Clock**
** with an I2C interface. The data sheet located at http://ww1.microchip.com/downloads/en/DeviceDoc/20002292B.pdf **
** describes the functionality used in this library                                                               **
**                                                                                                                **
** Use is made of portions of Adafruit's RTClib Version 1.2.0 at https://github.com/adafruit/RTClib which is a    **
** a fork of the original RTClib from Jeelabs. The code encompasses simple classes for time and date.             **
**                                                                                                                **
** Although programming for the Arduino and in c/c++ is new to me, I'm a professional programmer and have learned,**
** over the years, that it is much easier to ignore superfluous comments than it is to decipher non-existent ones;**
** so both my comments and variable names tend to be verbose. The code is written to fit in the first 80 spaces   **
** and the comments start after that and go to column 117 - allowing the code to be printed in A4 landscape mode. **
**                                                                                                                **
** This program is free software: you can redistribute it and/or modify it under the terms of the GNU General     **
** Public License as published by the Free Software Foundation, either version 3 of the License, or (at your      **
** option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY     **
** WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the   **
** GNU General Public License for more details. You should have received a copy of the GNU General Public License **
** along with this program.  If not, see <http://www.gnu.org/licenses/>.                                          **
**                                                                                                                **
** Vers.  Date       Developer           Comments                                                                 **
** ====== ========== =================== ======================================================================== **
** 1.0.1  2017-07-25 Arnd@SV-Zanshin.Com Added overloaded Calibrate() to manually set the trim factor             **
** 1.0.0  2017-07-23 Arnd@SV-Zanshin.Com Cleaned up code, initial github upload                                   **
** 1.0.2b 2017-07-20 Arnd@SV-Zanshin.Com Added alarm handling                                                     **
** 1.0.1b 2017-07-19 Arnd@SV-Zanshin.Com Added methods                                                            **
** 1.0.0b 2017-07-17 Arnd@SV-Zanshin.Com Initial coding                                                           **
**                                                                                                                **
*******************************************************************************************************************/
#include "Arduino.h"                                                          // Arduino data type definitions    //
#include <Wire.h>                                                             // Standard I2C "Wire" library      //
#ifndef MCP7940_h                                                             // Guard code definition            //
  #define MCP7940_h                                                           // Define the name inside guard code//
  /*****************************************************************************************************************
  ** Declare clases used in the class                                                                             **
  *****************************************************************************************************************/
  class TimeSpan;
  /*****************************************************************************************************************
  ** Declare constants used in the class                                                                          **
  *****************************************************************************************************************/
  const uint8_t  MCP7940_ADDRESS                 = 0x6F;                      // Device address, fixed value      //
  const uint8_t  MCP7940_I2C_DELAY               =    0;                      // Microseconds wait time for I2C   //
  const uint8_t  MCP7940_RTCSEC                  = 0x00;                      // Register definitions             //
  const uint8_t  MCP7940_RTCMIN                  = 0x01;                      //                                  //
  const uint8_t  MCP7940_RTCHOUR                 = 0x02;                      //                                  //
  const uint8_t  MCP7940_RTCWKDAY                = 0x03;                      //                                  //
  const uint8_t  MCP7940_RTCDATE                 = 0x04;                      //                                  //
  const uint8_t  MCP7940_RTCMTH                  = 0x05;                      //                                  //
  const uint8_t  MCP7940_RTCYEAR                 = 0x06;                      //                                  //
  const uint8_t  MCP7940_CONTROL                 = 0x07;                      //                                  //
  const uint8_t  MCP7940_OSCTRIM                 = 0x08;                      //                                  //
  const uint8_t  MCP7940_ALM0SEC                 = 0x0A;                      //                                  //
  const uint8_t  MCP7940_ALM0MIN                 = 0x0B;                      //                                  //
  const uint8_t  MCP7940_ALM0HOUR                = 0x0C;                      //                                  //
  const uint8_t  MCP7940_ALM0WKDAY               = 0x0D;                      //                                  //
  const uint8_t  MCP7940_ALM0DATE                = 0x0E;                      //                                  //
  const uint8_t  MCP7940_ALM0MTH                 = 0x0F;                      //                                  //
  const uint8_t  MCP7940_ALM1SEC                 = 0x11;                      //                                  //
  const uint8_t  MCP7940_ALM1MIN                 = 0x12;                      //                                  //
  const uint8_t  MCP7940_ALM1HOUR                = 0x13;                      //                                  //
  const uint8_t  MCP7940_ALM1WKDAY               = 0x14;                      //                                  //
  const uint8_t  MCP7940_ALM1DATE                = 0x15;                      //                                  //
  const uint8_t  MCP7940_ALM1MTH                 = 0x16;                      //                                  //
  const uint8_t  MCP7940_RAM_ADDRESS             = 0x20;                      // Start address for SRAM           //
  const uint32_t SECONDS_PER_DAY                 = 86400;                     // 60 secs * 60 mins * 24 hours     //
  const uint32_t SECONDS_FROM_1970_TO_2000       = 946684800;                 //                                  //
  const uint8_t  MCP7940_CONTROL_OUT             =    7;                      // Bit 7 is "OUT" in control reg    //
  const uint8_t  MCP7940_RTCSEC_SC               =    7;                      // Bit 7 is "ST" in seconds register//
  const uint8_t  MCP7940_RTCWKDAY_OSCRUN         =    5;                      //                                  //
  /*****************************************************************************************************************
  ** Simple general-purpose date/time class (no TZ / DST / leap second handling). Copied from RTClib.             **
  *****************************************************************************************************************/
  class DateTime {                                                            //                                  //
    public:                                                                   //----------------------------------//
      DateTime (uint32_t t=0);                                                // Constructor                      //
      DateTime (uint16_t year,uint8_t month,uint8_t day,uint8_t hour=0,       // Overloaded Constructors          //
                uint8_t min=0,uint8_t sec=0);                                 //                                  //
      DateTime (const DateTime& copy);                                        //                                  //
      DateTime (const char* date, const char* time);                          //                                  //
      DateTime (const __FlashStringHelper* date,                              //                                  //
                const __FlashStringHelper* time);                             //                                  //
      uint16_t year()         const { return 2000 + yOff; }                   // Return the year                  //
      uint8_t  month()        const { return m; }                             // Return the month                 //
      uint8_t  day()          const { return d; }                             // Return the day                   //
      uint8_t  hour()         const { return hh; }                            // Return the hour                  //
      uint8_t  minute()       const { return mm; }                            // Return the minute                //
      uint8_t  second()       const { return ss; }                            // Return the second                //
      uint8_t  dayOfTheWeek() const;                                          // Return the DOW                   //
      long     secondstime()  const;                                          // times as seconds since 1/1/2000  //
      uint32_t unixtime(void) const;                                          // times as seconds since 1/1/1970  //
      DateTime operator+(const TimeSpan& span);                               // addition                         //
      DateTime operator-(const TimeSpan& span);                               // subtraction                      //
      TimeSpan operator-(const DateTime& right);                              // subtraction                      //
    protected:                                                                //----------------------------------//
      uint8_t yOff, m, d, hh, mm, ss;                                         // private variables                //
  }; // of class DateTime definition                                          //                                  //
  /*****************************************************************************************************************
  ** Timespan class which can represent changes in time with seconds accuracy. Copied from RTClib.                **
  *****************************************************************************************************************/
  class TimeSpan {                                                            //                                  //
    public:                                                                   //----------------------------------//
      TimeSpan (int32_t seconds = 0);                                         //                                  //
      TimeSpan (int16_t days, int8_t hours, int8_t minutes, int8_t seconds);  //                                  //
      TimeSpan (const TimeSpan& copy);                                        //                                  //
      int16_t  days() const         { return _seconds / 86400L; }             //                                  //
      int8_t   hours() const        { return _seconds / 3600 % 24; }          //                                  //
      int8_t   minutes() const      { return _seconds / 60 % 60; }            //                                  //
      int8_t   seconds() const      { return _seconds % 60; }                 //                                  //
      int32_t  totalseconds() const { return _seconds; }                      //                                  //
      TimeSpan operator+(const TimeSpan& right);                              //                                  //
      TimeSpan operator-(const TimeSpan& right);                              //                                  //
    protected:                                                                //----------------------------------//
      int32_t _seconds;                                                       // internal seconds variable        //
  }; // of class TimeSpan definition                                          //                                  //
  /*****************************************************************************************************************
  ** Main MCP7940 class for the Real-Time clock                                                                   **
  *****************************************************************************************************************/
  class MCP7940_Class {                                                       // Class definition                 //
    public:                                                                   // Publicly visible methods         //
      MCP7940_Class();                                                        // Class constructor                //
      ~MCP7940_Class();                                                       // Class destructor                 //
      bool     begin();                                                       // Start I2C Comms with device      //
      bool     deviceStatus();                                                // return true when MCP7940 is on   //
      bool     deviceStart();                                                 // Start the MCP7940 clock          //
      bool     deviceStop();                                                  // Stop the MCP7940 clock           //
      DateTime now();                                                         // return the current time          //
      void     adjust();                                                      // Set the date and time to compile //
      void     adjust(const DateTime& dt);                                    // Set the date and time            //
      int8_t   calibrate();                                                   // Reset clock calibration offset   //
      int8_t   calibrate(const int8_t);                                       // Reset clock calibration offset   //
      int8_t   calibrate(const DateTime& dt);                                 // Calibrate the clock              //
      int8_t   getCalibrationTrim();                                          // Get the trim register value      //
      uint8_t  weekdayRead();                                                 // Read weekday from RTC            //
      uint8_t  weekdayWrite(const uint8_t dow);                               // Write weekday to RTC             //
      void     readRAM(const uint8_t address,uint8_t* buf,const uint8_t size);// Read multiple bytes from RAM     //
      uint8_t  readRAM(const uint8_t address);                                // Read single byte from RAM        //
      void     writeRAM(const uint8_t address,uint8_t* buf,const uint8_t size);// Write multiple bytes to RAM     //
      uint8_t  writeRAM(const uint8_t address,const uint8_t data);            //                                  //
      bool     setMFP(const bool value);                                      // Set the MFP pin state            //
      bool     getMFP();                                                      // Get the MFP pin state            //
      bool     setAlarm(const uint8_t alarmNumber, const uint8_t alarmType,   // Set an Alarm                     //
                        const DateTime dt );                                  //                                  //
      bool     clearAlarm(const uint8_t alarmNumber);                         // Clear an Alarm                   //
    private:                                                                  // Private methods                  //
      uint8_t  readByte(const uint8_t addr);                                  // Read 1 byte from address on I2C  //
      void     writeByte(const uint8_t addr, const uint8_t data);             // Write 1 byte at address to I2C   //
      uint8_t  bcd2int(const uint8_t bcd);                                    // convert BCD digits to integer    //
      uint8_t  int2bcd(const uint8_t dec);                                    // convert integer to BCD           //
      uint16_t readWord(const uint8_t addr);                                  // Read 2 bytes from address on I2C //
      uint8_t  _TransmissionStatus = 0;                                       // Status of I2C transmission       //
      bool     _CrystalStatus      = false;                                   // True if RTC is turned on         //
      bool     _OscillatorStatus   = false;                                   // True if Oscillator on and working//
      uint32_t _SetUnixTime        = 0;                                       // UNIX time when clock last set    //
  }; // of MCP7940 class definition                                           //                                  //
#endif                                                                        //----------------------------------//
