/*
  DriverIcMd.h - Library for iC-Haus IC-MD (48-BIT QUADRATURE COUNTER).
  Created by Angelo Cuccato, 06.12.2017.
  Released into the public domain.
*/

#include "Arduino.h"
#include "DriverIcMd.h"
#include "SPI.h"


const byte READ = 0b10000000;    // read command
const byte WRITE = 0b00000000;   // write command


DriverIcMd::DriverIcMd(int chipSelectPin)
{
	this->chipSelectPin = chipSelectPin;
	this->counterMode = CounterMode::Counter0_24Bit;

	// start the SPI library:
	SPI.begin();

	// initalize the chip select pin:
	pinMode(chipSelectPin, OUTPUT);
	digitalWrite(chipSelectPin, HIGH);
}

int DriverIcMd::CheckDevice()
{
	byte data[4] = {0};

	WriteReadAdress( BissProfileI_Base, READ, data, 2 );
	if( (data[0]!=0x33) || (data[1]!=0x18))
	{
		return -1;
	}

	WriteReadAdress( BissDeviceId_Base, READ, data, 2 );
	if( (data[0]!='M') || (data[1]!='D'))
	{
		return -2;
	}

	WriteReadAdress( BissDeviceRevision_Base, READ, data, 4 );
	if( (data[0]!='X') || (data[1]!=0x00) || (data[2]!=0x00) || (data[3]!=0x00))
	{
		return -3;
	}

	WriteReadAdress( BissDeviceManufacturerId_Base, READ, data, 2 );
	if( (data[0]!=0x69) || (data[1]!=0x43))
	{
		return -4;
	}

	return 0;
}

uint32_t DriverIcMd::GetRefRegister()
{
	UnionConvertBytesToInt data = {0};

	WriteReadAdress( MD_Ref_Base, READ, data.array, 3 );

	return data.values;
}

uint32_t DriverIcMd::GetUdpRegister()
{
	UnionConvertBytesToInt data = {0};

	WriteReadAdress( MD_UpdNaberrNuPdvall, READ, data.array, 3 );

	return data.values;
}

DriverIcMd::UnionCounterValues DriverIcMd::GetCounter()
{
	UnionCounterValues data = {0};
	byte values[sizeof(UnionCounterValues)] = {0};

	int len = 0;

	switch( this->counterMode)
	{
		case Counter0_24Bit:
			len = 4;
			break;
		case Counter0_24Bit_Counter1_24Bit:
		case Counter0_48Bit:
		case Counter0_32Bit_Counter1_16Bit:
		case Counter0_16Bit_Counter1_16Bit_Counter2_16Bit:
			len = 7;
			break;
		case Counter0_16Bit:
			len = 3;
			break;
		case Counter0_32Bit:
		case Counter0_16Bit_Counter1_16Bit:
			len = 5;
			break;
	}

	WriteReadAdress( MD_AbSpichNerrNwarn, READ, values, len );

	for( int i=0 ; i<len; i++)
	{
		data.array[i] = values[len-1-i];
	}

	return data;
}

void DriverIcMd::SendInstruction(UnionInstructionByte instruction)
{
	WriteReadAdress( InstructionByte, WRITE, &(instruction.bByte), 1 );
}

DriverIcMd::IcMdStatus DriverIcMd::GetStatus( void)
{
	IcMdStatus value = {0};

	WriteReadAdress( Status0, READ, &(value.reg0.bByte), 3 );

	return value;
}

////////////////////////////////////////
//	Counter Configuration
////////////////////////////////////////

DriverIcMd::CounterMode DriverIcMd::GetCounterMode( void)
{
	UnionConfig0 value = {0};

	WriteReadAdress( Config0, READ, &(value.bByte), 1 );

	return value.bits.CntCfg_CounterMode;
}
void DriverIcMd::SetCounterMode( CounterMode counterMode)
{
	UnionConfig0 value = {0};

	WriteReadAdress( Config0, READ, &(value.bByte), 1 );

	value.bits.CntCfg_CounterMode = counterMode;
	this->counterMode = counterMode;

	WriteReadAdress( Config0, WRITE, &(value.bByte), 1 );
}


DriverIcMd::IndexSignalZMode DriverIcMd::GetIndexSignalZMode( void)
{
	UnionConfig1 value = {0};

	WriteReadAdress( Config1, READ, &(value.bByte), 1 );

	return value.bits.CfgZ;
}
void DriverIcMd::SetIndexSignalZMode( IndexSignalZMode indexSignalZMode)
{
	UnionConfig1 value = {0};

	WriteReadAdress( Config1, READ, &(value.bByte), 1 );

	value.bits.CfgZ = indexSignalZMode;

	WriteReadAdress( Config1, WRITE, &(value.bByte), 1 );
}

DriverIcMd::CleanedCounterWithZ DriverIcMd::GetIndexClearedCounter( char counter)
{
	UnionConfig1 value = {0};

	WriteReadAdress( Config1, READ, &(value.bByte), 1 );

	return (counter == 0)? value.bits.CbZ0 : value.bits.CbZ1;
}
void DriverIcMd::SetIndexClearedCounter( char counter, DriverIcMd::CleanedCounterWithZ CkearedByZ)
{
	UnionConfig1 value = {0};

	WriteReadAdress( Config1, READ, &(value.bByte), 1 );

	if(counter == 0)
	{
		value.bits.CbZ0 = CkearedByZ;
	}
	else
	{
		value.bits.CbZ1 = CkearedByZ;
	}

	WriteReadAdress( Config1, WRITE, &(value.bByte), 1 );
}

DriverIcMd::InputMode DriverIcMd::GetInputMode()
{
	UnionConfig1 value = {0};

	WriteReadAdress( Config1, READ, &(value.bByte), 1 );

	return value.bits.Ttl;
}
void DriverIcMd::SetInputMode( DriverIcMd::InputMode mode)
{
	UnionConfig1 value = {0};

	WriteReadAdress( Config1, READ, &(value.bByte), 1 );

	value.bits.Ttl = mode;

	WriteReadAdress( Config1, WRITE, &(value.bByte), 1 );
}

DriverIcMd::DifferentialInputMode DriverIcMd::GetDifferentialInputMode()
{
	UnionConfig3 value = {0};

	WriteReadAdress( Config3, READ, &(value.bByte), 1 );

	return value.bits.Lvds;
}
void DriverIcMd::SetDifferentialInputMode( DriverIcMd::DifferentialInputMode mode)
{
	UnionConfig3 value = {0};

	WriteReadAdress( Config3, READ, &(value.bByte), 1 );

	value.bits.Lvds = mode;

	WriteReadAdress( Config3, WRITE, &(value.bByte), 1 );
}

DriverIcMd::CountingDirection DriverIcMd::GetCountingDirection( char counter)
{
	UnionConfig0 value = {0};

	WriteReadAdress( Config0, READ, &(value.bByte), 1 );

	switch( counter)
	{
		case 0:
			return value.bits.Exch0;
			break;
		case 1:
			return value.bits.Exch1;
			break;
		case 2:
			return value.bits.Exch2;
			break;
	}

	return (CountingDirection)0;
}
void DriverIcMd::SetCountingDirection( char counter, CountingDirection direction)
{
	UnionConfig0 value = {0};

	WriteReadAdress( Config0, READ, &(value.bByte), 1 );

	switch( counter)
	{
		case 0:
			value.bits.Exch0 = direction;
			break;
		case 1:
			value.bits.Exch1 = direction;
			break;
		case 2:
			value.bits.Exch2 = direction;
			break;
	}

	WriteReadAdress( Config0, WRITE, &(value.bByte), 1 );
}

DriverIcMd::IndexInvertedMode DriverIcMd::GetIndexInvertedMode( char counter)
{
	UnionConfig0 value = {0};

	WriteReadAdress( Config0, READ, &(value.bByte), 1 );

	return (counter == 0)? value.bits.InvZ0 : value.bits.InvZ1;
}
void DriverIcMd::SetIndexInvertedMode( char counter, DriverIcMd::IndexInvertedMode mode)
{
	UnionConfig0 value = {0};

	WriteReadAdress( Config0, READ, &(value.bByte), 1 );

	if(counter == 0)
	{
		value.bits.InvZ0 = mode;
	}
	else
	{
		value.bits.InvZ1 = mode;
	}

	WriteReadAdress( Config0, WRITE, &(value.bByte), 1 );
}

//###############################################################################
//	Private
//###############################################################################


void DriverIcMd::WriteReadAdress( RegisterAddr registerAddr, byte readWriteOperation, byte *data, int valueLen)
{
	byte adr = (registerAddr & 0x7F) | readWriteOperation;

	digitalWrite(chipSelectPin, LOW);

	SPI.transfer(adr);
	for( int i=0; i<valueLen; i++)
	{
		data[i] = SPI.transfer(data[i]);
	}

	digitalWrite(chipSelectPin, HIGH);
}
