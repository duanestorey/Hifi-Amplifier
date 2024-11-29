#include "cs8416.h"
#include "debug.h"

void 
CS8416::init() {
    // set higher update rate phase detector for sigma delta DAC
    mBus->writeRegisterByte( mAddress, CS8416::ADDR_CONTROL_0, 8 );

    // no auto clock switching, no muted zeros, active high interrupts, error switch to zeros,
    // master clock is 256*fs
    mBus->writeRegisterByte( mAddress, CS8416::ADDR_CONTROL_1, 4 );

    // DETCI?, de-emphasis off (turn it on after?), GPIO0 is sample rate
    mBus->writeRegisterByte( mAddress, CS8416::ADDR_CONTROL_2, 8 );

    // GPIO1 is non-audio detection, GPIO2 is GND
    mBus->writeRegisterByte( mAddress, CS8416::ADDR_CONTROL_3, ( 0b1001 << 4 ) | 0b0000 );

    // output as a I2S master, 24 bit audio, left justified data, 128*fs
    mBus->writeRegisterByte( mAddress, CS8416::ADDR_SERIAL_DATA_FORMAT, 0b11000000 );

    // Error mask - give us information about PLL locking, PCM validity, and confidence
    mBus->writeRegisterByte( mAddress, CS8416::ADDR_RECEIVER_ERROR_MASK, 0b00011100 );
}

void 
CS8416::setInput( uint8_t input ) {
    if ( input == CS8416::SPDIF_INPUT_1 || input == CS8416::SPDIF_INPUT_2 || input == CS8416::SPDIF_INPUT_3 ) {
        mInput = input;

        // update input via control register
        updateRunCommand();
    }
}

void 
CS8416::updateRunCommand() {
    if ( mRunning ) {
        // enable running, add input selection
        uint8_t control = 0b10000000 | ( mInput << 3 );
        mBus->writeRegisterByte( mAddress, CS8416::ADDR_CONTROL_4, control );
    } else {
        // disable running, add input selection
        uint8_t control = 0b00000000 | ( mInput << 3 );
        mBus->writeRegisterByte( mAddress, CS8416::ADDR_CONTROL_4, control );
    }
}

void
CS8416::run( bool enable ) {
    if ( enable == mRunning ) {
        // already running or not running
        return;
    }

    mRunning = enable;

    updateRunCommand();
}

uint8_t 
CS8416::getFormat() {
    uint8_t format = 0;
    mBus->readRegisterByte( mAddress, CS8416::ADDR_CONTROL_4, format );

    mCurrentFormat = format;

    return mCurrentFormat & 0b01111000;
}

bool 
CS8416::isHighFrequency() {
    getFormat();

    return ( mCurrentFormat & 1 );
}

bool 
CS8416::isSilence() {
    getFormat();

    return ( mCurrentFormat & 2 );
}

bool 
CS8416::hasLoopLock() {
    uint8_t status = 0;
    mBus->readRegisterByte( mAddress, CS8416::ADDR_RECEIVER_ERROR, status );

    // returns status of PLL locking
    return ( ( status & 16 ) == 0 );
}

bool 
CS8416::isValidPcm() {
    uint8_t status = 0;
    mBus->readRegisterByte( mAddress, CS8416::ADDR_RECEIVER_ERROR, status );

    // returns the PCM validity bit being zeor
    return ( ( status & 8 ) == 0 );
}

bool 
CS8416::isValidStream() {
    uint8_t status = 0;
    mBus->readRegisterByte( mAddress, CS8416::ADDR_RECEIVER_ERROR, status );

    // returns the confidence bit, basically PLL is locked and there is no bi-phase error
    return ( ( status & 4 ) == 0 );
}

