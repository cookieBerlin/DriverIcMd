/*
 IC-MD Communication

 Circuit Leonardo:
 NCS:  pin 7
 MOSI: pin ICSP-4
 MISO: pin ICSP-1
 SCK:  pin ICSP-3

 */

// the sensor communicates using SPI, so include the library:
#include <SPI.h>
#include <DriverIcMd.h>


const int chipSelectPin = 7;

DriverIcMd driverIcMd( chipSelectPin);

void setup() {
  Serial.begin(9600);

  // check Connection
  int result = driverIcMd.CheckDevice();
  if( result == 0)
  {
	  Serial.println( "IC-MD CheckDevice OK");
  }
  else
  {
	  Serial.print( "IC-MD CheckDevice Fail with ");
	  Serial.println( result);
  }

  // default Values
  driverIcMd.SetCounterMode(DriverIcMd::CounterMode::Counter0_16Bit_Counter1_16Bit);
  driverIcMd.SetIndexSignalZMode(DriverIcMd::IndexSignalZMode::A1B1);
  driverIcMd.SetInputMode( DriverIcMd::TTL);
  //driverIcMd.SetDifferentialInputMode( DriverIcMd::RS422);

  for( char i=0; i<2; i++)
  {
	driverIcMd.SetIndexClearedCounter( i, DriverIcMd::NotCleanedByZ);
	driverIcMd.SetCountingDirection( i, DriverIcMd::CWpositive);
	driverIcMd.SetIndexInvertedMode( i, DriverIcMd::NonInverted);
  }

  // reset Counter
  DriverIcMd::UnionInstructionByte instruction = {0};
  instruction.bits.ZCen_EnableZeroCodification = 1;
  instruction.bits.ABres0_ResetCounter0 = 1;
  instruction.bits.ABres1_ResetCounter1 = 1;
  instruction.bits.ABres2_ResetCounter2 = 1;
  driverIcMd.SendInstruction(instruction);
}

void loop()
{
	for( byte i =0; i<=0x7F; i++)
	{
		DriverIcMd::UnionCounterValues counter = driverIcMd.GetCounter();

		// display the temperature:

		Serial.print( "Counter0=");
		Serial.print( counter.Counter0_16Bit_Counter1_16Bit.Counter0, DEC);
		Serial.print( "\tCounter1=");
		Serial.print( counter.Counter0_16Bit_Counter1_16Bit.Counter1, DEC);
		Serial.print( "\tNERR=0b");
		Serial.print( counter.Counter0_16Bit_Counter1_16Bit.NERR, BIN);
		Serial.print( "\tNWARN=0b");
		Serial.print( counter.Counter0_16Bit_Counter1_16Bit.NWARN, BIN);


		if( (counter.Counter0_16Bit_Counter1_16Bit.NERR != 0) ||
		    (counter.Counter0_16Bit_Counter1_16Bit.NWARN != 0))
		{
			DriverIcMd::IcMdStatus status = driverIcMd.GetStatus();

			Serial.print( "\t");
			Serial.print( "\tStatus 0x48=0b");
			Serial.print( status.reg0.bByte, BIN);
			Serial.print( "\tStatus 0x49=0b");
			Serial.print( status.reg1.bByte, BIN);
			Serial.print( "\tStatus 0x4A=0b");
			Serial.print( status.reg2.bByte, BIN);
		}

		Serial.println( "");

		delay(100);
	}
}

