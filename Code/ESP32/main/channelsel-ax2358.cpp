#include "channelsel-ax2358.h"
#include "debug.h"

#define         AX2358_CMD_STEREO_1         0b11001011        
#define         AX2358_CMD_STEREO_2         0b11001010       
#define         AX2358_CMD_STEREO_3         0b11001001       
#define         AX2358_CMD_STEREO_4         0b11001000        
#define         AX2358_CMD_6CH              0b11001111       

#define         AX2358_VOL_ALL_1            0b11100000
#define         AX2358_VOL_ALL_10           0b11010000
#define         AX2358_VOL_CH1_1            0b10010000
#define         AX2358_VOL_CH1_10           0b10000000
#define         AX2358_VOL_CH2_1            0b01010000
#define         AX2358_VOL_CH2_10           0b01000000
#define         AX2358_VOL_CH3_1            0b00010000
#define         AX2358_VOL_CH3_10           0b00000000
#define         AX2358_VOL_CH4_1            0b00110000
#define         AX2358_VOL_CH4_10           0b00100000
#define         AX2358_VOL_CH5_1            0b01110000
#define         AX2358_VOL_CH5_10           0b01100000
#define         AX2358_VOL_CH6_1            0b10110000
#define         AX2358_VOL_CH6_10           0b10100000
#define         AX2358_VOL_CHAN_CLEAR       0b11000100


#define         AX2358_MUTE_FL              0b11110001
#define         AX2358_MUTE_FL_OFF          0b11110000
#define         AX2358_MUTE_FR              0b11110011
#define         AX2358_MUTE_FR_OFF          0b11110010
#define         AX2358_MUTE_ALL             0xff
#define         AX2358_MUTE_NONE            0xfe

#define         AX2358_SURR_ENH_ON          0b11000000
#define         AX2358_SURR_ENH_OFF         0b11000001

#define         AX2358_MIX_6DB_ON           0b11000010
#define         AX2358_MIX_6DB_OFF          0b11000011

void 
ChannelSel_AX2358::writeChipValue( uint8_t value ) {
    uint8_t data[2];
    data[0] = value;
    data[1] = value;

    mI2C->writeBytes( mAddress, &value, 1 );

    AMP_DEBUG_SI( "Writing two bytes data to CHANNELSEL " << (int)value );
}

void 
ChannelSel_AX2358::init() {
    // Need to clear registers
    writeChipValue( AX2358_VOL_CHAN_CLEAR ); 
    
    // Let's mute the output
  //  mute( true );

    // Set the volume levels to 20 to start
    setVolume( mVolume );

    // Enable surround enhancement
    writeChipValue( AX2358_SURR_ENH_ON ); 

    // Enable 6db reduction for combined channels
    writeChipValue( AX2358_MIX_6DB_ON );
}

void 
ChannelSel_AX2358::setInput( uint8_t input ) {
    switch( input ) {
        case ChannelSel::INPUT_STEREO_1:
            writeChipValue( AX2358_CMD_STEREO_1 );
            break;
        case ChannelSel::INPUT_STEREO_2:
            writeChipValue( AX2358_CMD_STEREO_2 );
            break;
        case ChannelSel::INPUT_STEREO_3:
            writeChipValue( AX2358_CMD_STEREO_3 );
            break;
        case ChannelSel::INPUT_STEREO_4:
            writeChipValue( AX2358_CMD_STEREO_4 );
            break;
        case ChannelSel::INPUT_6CH:
            writeChipValue( AX2358_CMD_6CH );
            break;
        default:
            break;
    }
    
}

void 
ChannelSel_AX2358::mute( bool mute ) {
    if ( mute ) {
        writeChipValue( AX2358_MUTE_ALL ); 
    } else {
        writeChipValue( AX2358_MUTE_NONE ); 
       // writeChipValue( AX2358_MUTE_FL_OFF );
     //   writeChipValue( AX2358_MUTE_FR_OFF );
    }
}

bool
ChannelSel_AX2358::setVolume( uint8_t volume ) {
    setChannelVolume( ChannelSel::CHANNEL_ALL, volume );
   // setChannelVolume( ChannelSel::CHANNEL_FRONT_LEFT, volume );
   // setChannelVolume( ChannelSel::CHANNEL_FRONT_RIGHT, volume );

    return true;
}

bool 
ChannelSel_AX2358::setChannelVolume( uint8_t channel, uint8_t volume ) {
    if ( volume > 79 ) {
        volume = 79;
    }

    uint8_t negVol = 79-volume; 
    uint8_t onesDigit = negVol % 10;
    uint8_t tensDigit = negVol / 10;

    uint8_t data[2] = {0};

    switch( channel ) {
        case ChannelSel::CHANNEL_FRONT_LEFT:
            data[ 0 ] = AX2358_VOL_CH1_10 | tensDigit;
            data[ 1 ] = AX2358_VOL_CH1_1 | onesDigit;
            return mI2C->writeBytes( mAddress, data, 2 );
            break;
        case ChannelSel::CHANNEL_FRONT_RIGHT:
            data[ 0 ] = AX2358_VOL_CH2_10 | tensDigit;
            data[ 1 ] = AX2358_VOL_CH2_1 | onesDigit;
            return mI2C->writeBytes( mAddress, data, 2 );
            break;
        case ChannelSel::CHANNEL_REAR_LEFT:
            data[ 0 ] = AX2358_VOL_CH3_10 | tensDigit;
            data[ 1 ] = AX2358_VOL_CH3_1 | onesDigit;
            return mI2C->writeBytes( mAddress, data, 2 );
            break;
        case ChannelSel::CHANNEL_REAR_RIGHT:
            data[ 0 ] = AX2358_VOL_CH4_10 | tensDigit;
            data[ 1 ] = AX2358_VOL_CH4_1 | onesDigit;
            return mI2C->writeBytes( mAddress, data, 2 );
            break;
        case ChannelSel::CHANNEL_CENTER:
            data[ 0 ] = AX2358_VOL_CH5_10 | tensDigit;
            data[ 1 ] = AX2358_VOL_CH5_1 | onesDigit;
            return mI2C->writeBytes( mAddress, data, 2 );
            break;
        case ChannelSel::CHANNEL_SUBWOOFER:
            data[ 0 ] = AX2358_VOL_CH6_10 | tensDigit;
            data[ 1 ] = AX2358_VOL_CH6_1 | onesDigit;
            return mI2C->writeBytes( mAddress, data, 2 );
            break;
        case ChannelSel::CHANNEL_ALL:
            data[ 0 ] = AX2358_VOL_ALL_10 | tensDigit;
            data[ 1 ] = AX2358_VOL_ALL_1 | onesDigit;
            return mI2C->writeBytes( mAddress, data, 2 );
        default:
            return false;
            break;
    }   
}