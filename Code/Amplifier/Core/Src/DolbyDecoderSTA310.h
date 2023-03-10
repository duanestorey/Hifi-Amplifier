/*
 * DolbyDecoderSTA310.h
 *
 *  Created on: Jan 11, 2023
 *      Author: duane
 */

#ifndef SRC_DOLBYDECODERSTA310_H_
#define SRC_DOLBYDECODERSTA310_H_

#include "DolbyDecoder.h"
#include "I2CDevice.h"
#include "DecoderEvents.h"

class DolbyDecoder_STA310 : public DolbyDecoder {
protected:
	I2C_Device *mDevice;
	bool mInitialized;
	bool mMuted;
	bool mRunning;
	bool mPlaying;

	DecoderEvents *mEventHandler;

	uint8_t mIdent;
	uint8_t mSoftwareVersion;
public:
public:
	enum {
		SYN = 1,
		HDR = 2,
		ERR = 4,
		SFR = 8,
		DEM = 16,
		BOF = 32,
		PTR = 64,
		ANC = 128
	} INT1S;

	enum {
		PCM = 1,
		FBF = 2,
		FBE = 4,
		FIO = 8,
		RST = 16,
		LCK = 32,
		USD = 64,
		TBD = 128
	} INT2S;

	enum {
		IDENT = 0x01,
		SOFTVER = 0x71,
		FREQ = 0x05,
		INT1 = 0x07,
		INT2 = 0x08,
		ERROR = 0x0f,
		SOFT_RESET = 0x10,
		INT1_RES = 0x09,
		INT2_RES = 0x0a,
		SIN_SETUP = 0x0c,
		CAN_SETUP = 0x0d,
		PLL_CTRL = 0x12,
		PLAY = 0x13,
		MUTE = 0x14,
		STREAM_SEL = 0x4c,
		DECODE_SEL = 0x4d,
		PCM_DIV = 0x54,
		PCM_CONF = 0x55,
		SOFT_MUTE = 0x73,
		BREAKPOINT = 0x2b,
		HDR_ERROR = 0x42,
		CLOCK_CMD = 0x3a,
		HEAD_4 = 0x42,
		HEAD_3 = 0x43,
		PACKET_LOCK = 0x4f,
		ID_EN = 0x50,
		ID = 0x51,
		ID_EXT = 0x52,
		SYNC_LOCK = 0x53,
		OCFG = 0x66,
		SPDIF_CONF = 0x60,
		AC3_DECODE_LFE = 0x68,
		AC3_COMP_MOD = 0x69,
		AC3_HDR = 0x6a,
		AC3_LDR = 0x6b,
		AC3_RPC = 0x6c,
		AC3_KARAOKE = 0x6d,
		AC3_DUALMODE = 0x6e,
		AC3_DOWNMIX = 0x6f,
		RUN = 0x72,
		DOLBY_STATUS_1 = 0x76,
		AC3_STATUS_1 = 0x77,
		ENABLE_PLL = 0xb5,
		AUTODETECT_ENA = 0xe0,
		AUTODETECT_SENS = 0xe1
	} REGISTERS;

	DolbyDecoder_STA310( I2C_Device *device  );
	virtual ~DolbyDecoder_STA310();

	virtual void setEventHandler( DecoderEvents *handler ) { mEventHandler = handler; }

	virtual void initialize();
	virtual void mute( bool enable = true );
	virtual void run();
	virtual void play( bool enable = true );

	virtual bool isInitialized() { return mInitialized; }
	virtual void checkForInterrupt();
	virtual void checkFormat();

	virtual void reset();

	virtual void setInformation( const std::string info ) { if ( mEventHandler ) mEventHandler->onInformation( info ); }
private:
	void softReset();
	void enableAudioPLL();
	void configureAudioPLL();
	void configureInterrupts( bool enableHDR = false );
	void configureSync();
	void configureSPDIF();
	void configurePCMOUT();
	void configureAC3();
	void configureDecoder();
};

#endif /* SRC_DOLBYDECODERSTA310_H_ */
