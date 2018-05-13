/*
  DriverIcMd.h - Library for iC-Haus IC-MD (48-BIT QUADRATURE COUNTER).
  Created by Angelo Cuccato, 06.12.2017.
  Released into the public domain.
*/

#ifndef DRIVERICMD_H_
#define DRIVERICMD_H_

#include "Arduino.h"

class DriverIcMd
{
  public:

	// CNTCFG Addr. 0x00; bit (2:0) Default = 0b000
	enum CounterMode
	{
		Counter0_24Bit									= 0b000,	//< 1 counter: TTL, RS422 or LVDS
		Counter0_24Bit_Counter1_24Bit					= 0b001,	//< 2 counter: TTL only
		Counter0_48Bit									= 0b010,	//< 1 counter: TTL, RS422 or LVDS
		Counter0_16Bit									= 0b011,	//< 1 counter: TTL, RS422 or LVDS
		Counter0_32Bit									= 0b100,	//< 1 counter: TTL, RS422 or LVDS
		Counter0_32Bit_Counter1_16Bit					= 0b101,	//< 2 counter: TTL only
		Counter0_16Bit_Counter1_16Bit					= 0b110,	//< 2 counter: TTL only
		Counter0_16Bit_Counter1_16Bit_Counter2_16Bit	= 0b111		//< 3 counter: TTL only
	};

	enum IndexSignalZMode
	{
		A1B1 = 0b00,	//< Z active: when A = 1 B = 1
		A1B0 = 0b01,	//< Z active: when A = 1 B = 0
		A0B1 = 0b10,	//< Z active: when A = 0 B = 1
		A0B0 = 0b11		//< Z active: when A = 0 B = 0
	};

	enum CountingDirection
	{
		CWpositive	= 0b0,	//< Exchange AB CNT (CW positive)
		CCWpositive = 0b1	//< Exchange AB CNT (CCW positive)
	};

	enum InputMode
	{
		Differential	= 0b0,	//< Differential inputs
		TTL 			= 0b1	//< TTL inputs
	};

	enum DifferentialInputMode
	{
		RS422			= 0b0,	//< Differential RS-422 inputs
		LVDS 			= 0b1	//< Differential LVDS inputs
	};

	enum CleanedCounterWithZ
	{
		NotCleanedByZ	= 0b0,	//< CNT not cleared by Z signal
		CleanedByZ		= 0b1	//< CNT cleared by Z signal
	};

	enum IndexInvertedMode
	{
		NonInverted	= 0b0,	//< Non inverted Z on CNT
		Inverted	= 0b1	//< inverted Z on CNT (Z=0 active)
	};

	union UnionConfig0
	{
		 byte bByte;
		 struct
		 {
			 CounterMode		CntCfg_CounterMode:3;
			 CountingDirection	Exch0:1;
			 CountingDirection	Exch1:1;
			 CountingDirection	Exch2:1;
			 IndexInvertedMode	InvZ0:1;
			 IndexInvertedMode	InvZ1:1;
		 } bits;
	};

	union UnionConfig1
	{
		 byte bByte;
		 struct
		 {
			 byte					Prior:1;
			 byte					TpCfg:2;
			 IndexSignalZMode		CfgZ:2;
			 CleanedCounterWithZ	CbZ0:1;
			 CleanedCounterWithZ	CbZ1:1;
			 InputMode				Ttl:1;
		 } bits;
	};

	union UnionConfig2
	{
		 byte bByte;
		 struct
		 {
			 byte Mask07:8;
		 } bits;
	};

	union UnionConfig3
	{
		 byte bByte;
		 struct
		 {
			 byte					Mask89:2;
			 byte					NMask:2;
			 byte					Reserved:3;
			 DifferentialInputMode	Lvds:1;
		 } bits;
	};

	union UnionConfig4
	{
		 byte bByte;
		 struct
		 {
			 byte Reserved:2;
			 byte Nench0:1;
			 byte Ch0Sel:1;
			 byte EnCh1:1;
			 byte Ch1Sel:1;
			 byte EnCh2:1;
			 byte Ch2Sel:1;
		 } bits;
	};

	struct IcMdConfig
	{
		UnionConfig0 reg0;
		UnionConfig1 reg1;
		UnionConfig2 reg2;
		UnionConfig3 reg3;
		UnionConfig4 reg4;
	};

	union UnionInstructionByte
	{
		 byte bByte;
		 struct
		 {
			 byte ABres0_ResetCounter0:1;
			 byte ABres1_ResetCounter1:1;
			 byte ABres2_ResetCounter2:1;
			 byte ZCen_EnableZeroCodification:1;
			 byte TP_LoadTP2withTP1valueAndTP1withABcntValue:1;
			 byte ACT0_SetActuatorPin0to:1;
			 byte ACT1_SetActuatorPin1to:1;
			 byte Reserved:1;
		 } bits;
	};

	union UnionStatus0x48
	{
		 byte bByte;
		 struct
		 {
			 byte TPVAL:1;
			 byte OVFREQ:1;
			 byte UPDBAL:1;
			 byte RVAL:1;
			 byte PDWN:1;
			 byte ZERO0:1;
			 byte OVF0:1;
			 byte ABBER0:1;
		 } bits;
	};

	union UnionStatus0x49
	{
		 byte bByte;
		 struct
		 {
			 byte TPS:1;
			 byte COMCOL:1;
			 byte EXTWARN:1;
			 byte EXTERR:1;
			 byte PDWN:1;
			 byte ZERO1:1;
			 byte OVF1:1;
			 byte ABBER1:1;
		 } bits;
	};

	union UnionStatus0x4A
	{
		 byte bByte;
		 struct
		 {
			 byte ENSSI:1;
			 byte COMCOL:1;
			 byte EXTWARN:1;
			 byte EXTERR:1;
			 byte PDWN:1;
			 byte ZERO2:1;
			 byte OVF2:1;
			 byte ABBER2:1;
		 } bits;
	};

	struct IcMdStatus
	{
		UnionStatus0x48 reg0;
		UnionStatus0x49 reg1;
		UnionStatus0x4A reg2;
	};

	union UnionConvertBytesToInt
	{
		 uint32_t values;
		 byte array[4];
	};

	union UnionCounterValues
	{
		byte array[7];
		struct
		{
			byte Reserved:6;
			byte NERR:1;
			byte NWARN:1;
			uint32_t Counter0:24;
		} Counter0_24Bit;
		struct
		{
			byte Reserved:6;
			byte NERR:1;
			byte NWARN:1;
			uint32_t Counter1:24;
			uint32_t Counter0:24;
		} Counter0_24Bit_Counter1_24Bit;
		struct
		{
			byte Reserved:6;
			byte NERR:1;
			byte NWARN:1;
			uint64_t Counter0:48;
		} Counter0_48Bit;
		struct
		{
			byte Reserved:6;
			byte NERR:1;
			byte NWARN:1;
			int16_t Counter0:16;
		} Counter0_16Bit;
		struct
		{
			byte Reserved:6;
			byte NERR:1;
			byte NWARN:1;
			int32_t Counter0:32;
		} Counter0_32Bit;
		struct
		{
			byte Reserved:6;
			byte NERR:1;
			byte NWARN:1;
			int16_t Counter1:16;
			int32_t Counter0:32;
		} Counter0_32Bit_Counter1_16Bit;
		struct
		{
			byte Reserved:6;
			byte NERR:1;
			byte NWARN:1;
			int16_t Counter0:16;
			int16_t Counter1:16;
		} Counter0_16Bit_Counter1_16Bit;
		struct
		{
			byte Reserved:6;
			byte NERR:1;
			byte NWARN:1;
			int16_t Counter2:16;
			int16_t Counter1:16;
			int16_t Counter0:16;
		} Counter0_16Bit_Counter1_16Bit_Counter2_16Bit;
	};

	DriverIcMd(int chipSelectPin);

	// Check Connection to ic-MD ic
	int CheckDevice();

	// Get Value of Ref Register, see Page 14 ( REF(23:0) Addr. 0x10..0x12)
	uint32_t GetRefRegister();

	// Get Value of UPD Register, see Page 15 ( UDP(23:0) Addr. 0x0A)
	uint32_t GetUdpRegister();

	// Get Counter Values, see Page 16, 23, 24
	UnionCounterValues GetCounter();

	// Send Instruction Byte, see Page 17
	void SendInstruction(UnionInstructionByte instruction);

	// Get Status Values, see Page 18-21
	IcMdStatus GetStatus();

	////////////////////////////////////////
	//	Counter Configuration
	////////////////////////////////////////

	// Get/Set Counter Configuration, see Page 12 (CNTCFG Addr. 0x00; bit (2:0))
	CounterMode GetCounterMode( void);
	void SetCounterMode( CounterMode counterMode);

	// Get/Set Index Signal Z, see Page 12 (CFGZ Addr. 0x01; bit (4:3))
	IndexSignalZMode GetIndexSignalZMode( void);
	void SetIndexSignalZMode( IndexSignalZMode counterMode);

	// Get/Set Index Signal Z Reset Counter, see Page 12 ( CFGZ Addr. 0x01; bit (4:3) & CBZ1 Addr. 0x01; bit (6))
	CleanedCounterWithZ GetIndexClearedCounter( char counter);
	void SetIndexClearedCounter( char counter, CleanedCounterWithZ CkearedByZ);

	// Get/Set Differential / TTL input mode, see Page 13 ( TTL Addr. 0x01; bit (7))
	InputMode GetInputMode();
	void SetInputMode( InputMode mode);

	// Get/Set Differential input mode RS-422 / LVDS (works only if InputMode is Differential), see Page 13 ( LVDS Addr. 0x03; bit (7))
	DifferentialInputMode GetDifferentialInputMode();
	void SetDifferentialInputMode( DifferentialInputMode mode);

	// Get/Set AB Counting direction, see Page 13 ( EXCH0-2 Addr. 0x01; bit (3-6))
	CountingDirection GetCountingDirection( char counter);
	void SetCountingDirection( char counter, CountingDirection direction);

	// Get/Set Index Inverted Mode, see Page 13 ( INV0-1 Addr. 0x00; bit (7:6))
	IndexInvertedMode GetIndexInvertedMode( char counter);
	void SetIndexInvertedMode( char counter, IndexInvertedMode mode);

	//###############################################################################
	//	Private
	//###############################################################################

  private:
	enum RegisterAddr
	{
		Config0							= 0x00,
		Config1							= 0x01,
		Config2							= 0x02,
		Config3							= 0x03,
		Config4							= 0x04,
		Config5							= 0x05,
		Config6							= 0x06,
		Config7							= 0x07,
		MD_AbSpichNerrNwarn				= 0x08,
		MD_UpdNaberrNuPdvall			= 0x0A,
		MD_Tp1NaberrNtpval				= 0x0D,
		MD_TP2NabberNtpval				= 0x0E,
		MD_Ref_Base	 					= 0x10,
		Spich_Base 						= 0x20,
		InstructionByte					= 0x30,
		BissProfileI_Base				= 0x42,
		Status0							= 0x48,
		Status1							= 0x49,
		Status2							= 0x4A,
		BissDeviceId_Base				= 0x78,
		BissDeviceRevision_Base			= 0x7A,
		BissDeviceManufacturerId_Base	= 0x7E,
	};

	int chipSelectPin;
	CounterMode counterMode;

	void WriteReadAdress( RegisterAddr address, byte readWriteOperation, byte *data, int valueLen);
};

#endif /* DRIVERICMD_H_ */
