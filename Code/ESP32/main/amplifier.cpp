#include "amplifier.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_system.h"
#include "esp_log.h"
#include "driver/ledc.h"

#include "pins.h"
#include "esp_log.h"
#include "debug.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_netif_sntp.h"
#include "esp_sntp.h"

#include "esp_bt.h"
#include "sdkconfig.h"
#include "dac-pcm1681.h"
#include "channelsel-ax2358.h"

Amplifier::Amplifier() : mWifiEnabled( false ), mWifiConnectionAttempts( 0 ), mUpdatingFromNTP( false ), mPoweredOn( true ), mDisplayQueue( 3 ), mTimerID( 0 ), mReconnectTimerID( 0 ), mLCD( 0 ),
    mDAC( 0 ), mChannelSel( 0 ), mDolbyDecoder( 0 ), mMicroprocessorTemp( 0 ), mVolumeEncoder( 15, 13, true ), mInputEncoder( 4, 16, false ), mAudioTimerID( 0 ), mPendingVolumeChange( false ), mPendingVolume( 0 ) {
}

void 
Amplifier::taskDelayInMs( uint32_t ms ) {
    vTaskDelay( ms / portTICK_PERIOD_MS );
}

AmplifierState 
Amplifier::getCurrentState() {
    ScopedLock lock( mStateMutex );
    AmplifierState stateCopy = mState;
    return stateCopy;
}

void 
Amplifier::updateInput( uint8_t input, bool doActualUpdate ) {
    {
        ScopedLock lock( mStateMutex );
        mState.mInput = input;
    }

    mChannelSel->setInput( input );

    if ( doActualUpdate ) {
        asyncUpdateDisplay();
    }
}

void 
Amplifier::updateConnectedStatus( bool connected, bool doActualUpdate ) {
    {
        ScopedLock lock( mStateMutex );
        mState.mConnected = connected;
    }

    if ( doActualUpdate ) {
        asyncUpdateDisplay();
    }   

    if ( connected ) {
        gpio_set_level( PIN_LED_CONNECTED, 1 );
    } else {
        gpio_set_level( PIN_LED_CONNECTED, 0 );
    }

}

void 
Amplifier::init() {
    nvs_flash_init(); 

    configurePins();

    gpio_set_level( PIN_DECODER_RESET, 0 );

    taskDelayInMs( 100 );

    gpio_set_level( PIN_DECODER_RESET, 1 );

    taskDelayInMs( 250 );

    mTimerID = mTimer.setTimer( 15000, mAmplifierQueue, true );

    mLCD = new LCD( 0x27, &mI2C );
    mMicroprocessorTemp = new TMP100( 0x48, &mI2C );
    mChannelSel = new ChannelSel_AX2358( 0x4a, &mI2C );
    mDAC = new DAC_PCM1681( 0x4c, &mI2C ); 
    mDolbyDecoder = new Dolby_STA310( 0x60, &mI2C );


  //  taskDelayInMs( 100 );

    // 0x5c is Dolby or 60?

    // DAC should be 0x4c or 0x4d

    mI2C.scanBus();

    
   
    mAudioTimerID = mTimer.setTimer( 200, mAudioQueue, true );
}

void 
Amplifier::asyncUpdateDisplay() {
    mDisplayQueue.add( Message::MSG_DISPLAY_SHOULD_UPDATE );
}

void 
Amplifier::handleDisplayThread() {
    AMP_DEBUG_I( "Starting Display Thread" );
    Message msg;

    // Startup
    mLCD->begin();

    // Let the audio task know the display is up and ready, should it care
    mAmplifierQueue.add( Message::MSG_DISPLAY_DONE_INIT );

    while( true ) {
        while ( mDisplayQueue.waitForMessage( msg, 10 ) ) {
            AMP_DEBUG_I( "Processing Display Queue Message" );

            switch( msg.mMessageType ) {
                case Message::MSG_DISPLAY_SHOULD_UPDATE:
                    updateDisplay();
                    break;
                default:
                    break;
            }
        } 
    } 
}

void
Amplifier::updateDisplay() {
    AmplifierState state = getCurrentState();
    char s[32];
    char d[14];

    sprintf( d, "Vol %ddB", ( state.mCurrentVolume - 79 ) );

    if ( state.mConnected ) {
        sprintf( s, "%-12s%8s", d, "Wifi" );
    } else {
        sprintf( s, "%-20s", d );
    }
    mLCD->writeLine( 0, std::string( s ) );

    switch( state.mState ) {
        case AmplifierState::STATE_INIT:
            sprintf( s, "%-12s", "Starting");
            break;
        case AmplifierState::STATE_PLAYING:
            sprintf( s, "%-12s%7.1fC", "Playing", mMicroprocessorTemp->readTemperature() );
            break;
        case AmplifierState::STATE_MUTED:
            sprintf( s, "%-12s", "Muted" );
            break;
        case AmplifierState::STATE_SLEEPING:
            sprintf( s, "%-12s", "Sleeping" );
            break;
        case AmplifierState::STATE_UPDATING:
            sprintf( s, "%-12s", "Updating" );
            break;
        case AmplifierState::STATE_ERROR_MINOR:
            sprintf( s, "%-12s", "Minor Error" );
            break;
        case AmplifierState::STATE_ERROR_MAJOR:
            sprintf( s, "%-12s", "Major Error" );
            break;
    }
    mLCD->writeLine( 1, s );

    char rate[12] = {0};
    if ( state.mAudioType == AmplifierState::AUDIO_ANALOG ) {
        strcpy( rate, "" );
    } else {
        sprintf( rate, "%lukHz", state.mSamplingRate / 1000 );
    }

    switch( state.mAudioType ) {
        case AmplifierState::AUDIO_ANALOG:
            sprintf( s, "%-10s%10s", "Analog", rate );
            break;
        case AmplifierState::AUDIO_DOLBY:
            sprintf( s, "%-10s%10s", "AC3", rate );
            break;
        case AmplifierState::AUDIO_DTS:
            sprintf( s, "%-10s%10s", "DTS", rate );
            break;
        case AmplifierState::AUDIO_PCM:
            sprintf( s, "%-10s%10s", "PCM", rate );
            break;
    }
    mLCD->writeLine( 2, std::string( s ) );

    char input[24];
    char audioType[10];
    switch( state.mSpeakerConfig ) {
        case AmplifierState::AUDIO_2_CH:
            strcpy( audioType, "Stereo" );
            break;
        case AmplifierState::AUDIO_2_DOT_1:
            strcpy( audioType, "2.1" );
            break;
        case AmplifierState::AUDIO_5_DOT_1:
            strcpy( audioType, "5.1" );
            break;
    }

    switch( state.mInput ) {
        case AmplifierState::INPUT_STEREO_1:
            sprintf( input, "%-12s%8s", "TV", audioType );
            break;
        case AmplifierState::INPUT_STEREO_2:
            sprintf( input, "%-12s%8s", "Streamer", audioType );
            break;
        case AmplifierState::INPUT_STEREO_3:
            sprintf( input, "%-12s%8s", "Game", audioType );
            break;
        case AmplifierState::INPUT_STEREO_4:
            sprintf( input, "%-12s%8s", "Vinyl", audioType );
            break;
        case AmplifierState::INPUT_6CH:
           // mLCD->writeLine( 3, "SPDIF" );
            sprintf( input, "%-12s%8s", "SPDIF", audioType );
            break;
    }

    mLCD->writeLine( 3, input );
}

void 
Amplifier::changeAmplifierState( uint8_t newState ) {
    {
        ScopedLock lock( mStateMutex );
        mState.mState = newState;
    }

    asyncUpdateDisplay();
}

void 
Amplifier::handleTimerThread() {
    AMP_DEBUG_SI( "Starting Timer Thread on Core " << xPortGetCoreID() );
    while( true ) {
        mTimer.processTick();
        taskDelayInMs( 20 );
    } 
}

void 
Amplifier::handleAmplifierThread() {
    AMP_DEBUG_SI( "Starting Amplifier Thread on Core " << xPortGetCoreID() );

    Message msg;
    while( true ) {
        while ( mAmplifierQueue.waitForMessage( msg, 10 ) ) {
            AMP_DEBUG_I( "Processing Amplifier Queue Message" );

            switch( msg.mMessageType ) {
                case Message::MSG_DISPLAY_SHOULD_UPDATE:
                    // need to update the display
                    break;
                case Message::MSG_DISPLAY_DONE_INIT:
                    // We are done display startup, let's start wifi now
                    mRadioQueue.add( Message::MSG_WIFI_INIT );
                    break;
                case Message::MSG_TIMER:
                    if ( msg.mParam == mTimerID ) {
                        AMP_DEBUG_SI( "In Periodic Timer Event, Temp is " << mMicroprocessorTemp->readTemperature() );
                    }
                    break;
                case Message::MSG_DECODER_IRQ:
                    handleDecoderIRQ();
                    break;
                case Message::MSG_BUTTON_POWER_PRESS:
                    handlePowerButtonPress();
                    break;
                case Message::MSG_VOLUME_POWER_PRESS:
                    handleVolumeButtonPress( msg.mParam );
                    break;
                case Message::MSG_INPUT_POWER_PRESS:
                    handleInputButtonPress();
                    break;        
                case Message::MSG_VOLUME_UP:
                    {
                        ScopedLock lock( mStateMutex );

                        if ( mState.mCurrentVolume < 79 ) {
                            mState.mCurrentVolume++;

                            asyncUpdateDisplay();

                            mAudioQueue.add( Message::MSG_VOLUME_SET, mState.mCurrentVolume );
                        }
                    }
                    break;
                case Message::MSG_VOLUME_DOWN:
                    {
                        ScopedLock lock( mStateMutex );

                        if ( mState.mCurrentVolume > 0 ) {
                            mState.mCurrentVolume--;

                            asyncUpdateDisplay();

                            mAudioQueue.add( Message::MSG_VOLUME_SET, mState.mCurrentVolume );
                        }
                    }  
                    break;
                case Message::MSG_INPUT_UP:
                    {
                        ScopedLock lock( mStateMutex );

                        switch( mState.mInput ) {
                            case AmplifierState::INPUT_STEREO_1:
                                mState.mInput = AmplifierState::INPUT_STEREO_2;
                                mState.mAudioType = AmplifierState::AUDIO_ANALOG;
                                break;
                            case AmplifierState::INPUT_STEREO_2:
                                mState.mInput = AmplifierState::INPUT_STEREO_3;
                                mState.mAudioType = AmplifierState::AUDIO_ANALOG;
                                break;
                            case AmplifierState::INPUT_STEREO_3:
                                mState.mInput = AmplifierState::INPUT_STEREO_4;
                                mState.mAudioType = AmplifierState::AUDIO_ANALOG;
                                break;
                            case AmplifierState::INPUT_STEREO_4:
                                mState.mInput = AmplifierState::INPUT_6CH;
                                mState.mAudioType = AmplifierState::AUDIO_DOLBY;
                                break;
                            case AmplifierState::INPUT_6CH:
                                mState.mInput = AmplifierState::INPUT_STEREO_1;
                                mState.mAudioType = AmplifierState::AUDIO_ANALOG;
                                break;
                            default:
                                break;
                        }

                        asyncUpdateDisplay();

                        mAudioQueue.add( Message::MSG_INPUT_SET, mState.mInput );
                    }
                    break;
                case Message::MSG_INPUT_DOWN:
                    {
                        ScopedLock lock( mStateMutex );
                        
                        switch( mState.mInput ) {
                            case AmplifierState::INPUT_STEREO_1:
                                mState.mInput = AmplifierState::INPUT_6CH;
                                mState.mAudioType = AmplifierState::AUDIO_DOLBY;
                                break;
                            case AmplifierState::INPUT_STEREO_2:
                                mState.mInput = AmplifierState::INPUT_STEREO_1;
                                mState.mAudioType = AmplifierState::AUDIO_ANALOG;
                                break;
                            case AmplifierState::INPUT_STEREO_3:
                                mState.mInput = AmplifierState::INPUT_STEREO_2;
                                mState.mAudioType = AmplifierState::AUDIO_ANALOG;
                                break;
                            case AmplifierState::INPUT_STEREO_4:
                                mState.mInput = AmplifierState::INPUT_STEREO_3;
                                mState.mAudioType = AmplifierState::AUDIO_ANALOG;
                                break;
                            case AmplifierState::INPUT_6CH:
                                mState.mInput = AmplifierState::INPUT_STEREO_4;
                                mState.mAudioType = AmplifierState::AUDIO_ANALOG;
                                break;
                            default:
                                break;
                        }

                        asyncUpdateDisplay();

                        mAudioQueue.add( Message::MSG_INPUT_SET, mState.mInput );
                    }       
                    break;
                case Message::MSG_INPUT_BUTTON_PRESS:
                    AMP_DEBUG_I( "Input Button Pressed" );
                    break;               
                default:
                    break;
            }
        } 
    } 
}   

void 
Amplifier::handleDecoderIRQ() {
    AMP_DEBUG_I( "Decoder Interrupt" );  
    mDolbyDecoder->handleInterrupt();
}

void
Amplifier::handlePowerButtonPress() {
    AMP_DEBUG_I( "Power button pressed" );
}

void 
Amplifier::handleInputButtonPress() {
    AMP_DEBUG_I( "Input button pressed" );
}

void 
Amplifier::handleVolumeButtonPress(  uint8_t param ) {
  //  AMP_DEBUG_I( "Volume button pressed" );

    AMP_DEBUG_SI( "Volume encoder is " << gpio_get_level( GPIO_NUM_13 ) << " " << gpio_get_level( GPIO_NUM_15 ) << " " << mVolumeEncoder.level() )
}

void 
Amplifier::handleAudioThread() {
    AMP_DEBUG_I( "Starting Audio Thread" );

    AMP_DEBUG_I( "Decoder set into active mode" );   

     vTaskDelay( 1000 / portTICK_PERIOD_MS );    

    gpio_set_level( PIN_RELAY, 1 );

    vTaskDelay( 200 / portTICK_PERIOD_MS );

    mDolbyDecoder->init();
    mDolbyDecoder->mute( false );
    mDolbyDecoder->run();
    mDolbyDecoder->play( true );

    // Channel Selector is now muted - need to umuted when audio is ready to go
    mChannelSel->init();
    mChannelSel->mute( true );

    AMP_DEBUG_I( "Starting DAC initialization" );
    mDAC->init();

    mDAC->enable( true );

    mDAC->setFormat( DAC::FORMAT_SONY );
    // Right justified 24 bit
    //mDAC->setFormat( 4 );
    //mDAC->setFormat( 4 );

    // Should eventually be able to set this higher as hopefully channel selector will handle volume
    mDAC->setVolume( 100 );

    // Initial volume, 0-79
    AmplifierState state = getCurrentState();

    AMP_DEBUG_I( "Setting involumeput" );
    mChannelSel->setVolume( state.mCurrentVolume );

    AMP_DEBUG_I( "Setting input" );
    mChannelSel->setInput( state.mInput );

    asyncUpdateDisplay();
    
    mChannelSel->mute( false );

    AMP_DEBUG_I( "Channel selector un-muted" );

    gpio_set_level( PIN_LED_ACTIVE, 1 );
   
    // Activate power
    vTaskDelay( 100 / portTICK_PERIOD_MS );
    
    // Set to playing status
    changeAmplifierState( AmplifierState::STATE_PLAYING );

    Message msg;
    while( true ) {
        while ( mAudioQueue.waitForMessage( msg, 10 ) ) {
            switch( msg.mMessageType ) {
                case Message::MSG_VOLUME_SET:
                    AMP_DEBUG_SI( "Setting pending audio volume to " << msg.mParam );
                    mPendingVolumeChange = true;
                    mPendingVolume = msg.mParam;
                    
                    break;
                case Message::MSG_INPUT_SET:
                    AMP_DEBUG_SI( "Setting audio input to " << msg.mParam );
                    mChannelSel->setInput( msg.mParam );

                    break;
                case Message::MSG_TIMER: 
                    if ( mPendingVolumeChange ) {
                        AMP_DEBUG_SI( "Actually setting pending audio volume to " << mPendingVolume );
                        bool result = mChannelSel->setVolume( mPendingVolume );
                        if ( result ) {
                            // if it failed, we will try again shortly
                            mPendingVolumeChange = false;
                        }
                    }
                    
                    break;
                default:
                    break;
            }
        } 

     //   mDolbyDecoder->checkForInterrupt();
    } 
}

static void wifi_event_handler( void *event_handler_arg, esp_event_base_t event_base, int32_t event_id,void *event_data ){
    (( Amplifier *)event_handler_arg)->_handleWifiCallback( event_id );
}

void
Amplifier::_handleWifiCallback( int32_t event_id ) {
    mRadioQueue.add( Message::MSG_WIFI_UPDATE, event_id );
}

void
Amplifier::handleWifiCallback( int32_t event_id ) {
    AMP_DEBUG_I( "Processing Wifi Callback" ); 
    switch( event_id ) {
        case WIFI_EVENT_STA_START:
            AMP_DEBUG_I( "Wifi Starting Up" );
            break;
        case WIFI_EVENT_STA_CONNECTED:
            AMP_DEBUG_I( "Wifi Connection Established" );

            updateConnectedStatus( true );
            break;
        case WIFI_EVENT_STA_DISCONNECTED:
            AMP_DEBUG_I( "Wifi Disconnected" );
            if ( mWifiEnabled ) {
                AMP_DEBUG_I( "Setting Reconnection Timer" );
                mReconnectTimerID = mTimer.setTimer( 10000, mRadioQueue, false );
            }
            updateConnectedStatus( false );
            break;
        case IP_EVENT_STA_GOT_IP:
            AMP_DEBUG_I( "Wifi Received IP Address" );
            updateTimeFromNTP();
            break;
    }
}

void NTP_Callback(struct timeval *tv) {
    AMP_DEBUG_I( "Received NTP Callback" );  
}

 void 
 Amplifier::updateTimeFromNTP() {
    AMP_DEBUG_I( "Attemping To Update Time From NTP Server" );
    esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG( "pool.ntp.org" );

    esp_netif_sntp_init( &config );
    sntp_set_time_sync_notification_cb( &NTP_Callback );

    setenv("TZ", "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00", 1);
    tzset();

    mUpdatingFromNTP = true;
    esp_netif_sntp_start();
 }

void 
Amplifier::setupWifi() {
    esp_netif_init();
    esp_event_loop_create_default(); 
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_initiation);
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, (void*)this );
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, (void*)this );//creating event handler register for ip event
    wifi_config_t wifi_config;
    memset( &wifi_config, 0, sizeof( wifi_config ) );

    strcpy( (char*)wifi_config.sta.ssid, (char*)"The Grey Havens" );
    strcpy( (char*)wifi_config.sta.password, (char*)"brazil1234!" );

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config( WIFI_IF_STA, &wifi_config );

    mWifiEnabled = true;

    esp_wifi_start();

   // Bluetooth::instance().init();

    // Bluetooth
    /*
    uint32_t ret;
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init( &bt_cfg );
    ret = esp_bt_controller_enable( ESP_BT_MODE_BLE );
    ret = esp_bluedroid_init();
    ret = esp_bluedroid_enable();
    ret = esp_ble_gatts_register_callback(gatts_callback);
    ret = esp_ble_gap_register_callback(gap_callback);
    ret = esp_ble_gatts_app_register( 0x01 );
    esp_err_t local_mtu_ret = esp_ble_gatt_set_local_mtu(512);
    */
}

void 
Amplifier::attemptWifiConnect() {
    AMP_DEBUG_I( "Attempting to connect Wifi" );

    mWifiConnectionAttempts = 0;
    esp_wifi_connect();
}

void
Amplifier::handleRadioThread() {
    AMP_DEBUG_SI( "Starting Radio Thread on Core " << xPortGetCoreID() );

    setupWifi();
    attemptWifiConnect();

    Message msg;
    while( true ) {
        if ( mUpdatingFromNTP ) {
            if ( SNTP_SYNC_STATUS_COMPLETED == sntp_get_sync_status() ) {
                mUpdatingFromNTP = false;

                time_t now;
                struct tm timeinfo;
                char strftime_buf[64];

                time(&now);
                localtime_r(&now, &timeinfo);
                strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
                
                AMP_DEBUG_SI("The current date/time is: " << strftime_buf  );  

                esp_netif_sntp_deinit();
            }
        }

        while ( mRadioQueue.waitForMessage( msg, 10 ) ) {
            AMP_DEBUG_I( "Processing Radio Queue Message" );
            switch( msg.mMessageType ) {
                case Message::MSG_WIFI_INIT:
                   
                    break;
                case Message::MSG_WIFI_UPDATE:
                    handleWifiCallback( msg.mParam );
                    break;
                case Message::MSG_TIMER:
                    if ( msg.mParam == mReconnectTimerID ) {
                        AMP_DEBUG_I( "In Reconnection Timer Event" );
                        attemptWifiConnect();
                        mReconnectTimerID = 0;
                    } else if ( msg.mParam == mTimerID ) {
                        AMP_DEBUG_I( "In Periodic Timer Event" );
                    } else {
                        AMP_DEBUG_SI( "Unknown timer event " << msg.mParam );
                    }
                   
                    break;
                default:
                    break;
            }
        } 
    } 
}

void 
Decoder_ISR( void *arg ) {
    (( Amplifier * )arg)->_handleDecoderISR();
}

void 
Power_Button_ISR( void *arg ) {
    (( Amplifier * )arg)->_handlePowerButtonISR();
}

void 
Volume_Button_ISR( void *arg ) {
    (( Amplifier * )arg)->_handleVolumeButtonISR();
}

void 
Input_Button_ISR( void *arg ) {
    (( Amplifier * )arg)->_handleInputButtonISR();
}

void
Input_Button_Encoder_ISR( void *arg ) {
    (( Amplifier * )arg)->_handleInputButtonEncoderISR();
}

void 
Amplifier::_handleDecoderISR() {
    mAmplifierQueue.addFromISR( Message::MSG_DECODER_IRQ );
}

void 
Amplifier::_handlePowerButtonISR() {
    mAmplifierQueue.addFromISR( Message::MSG_BUTTON_POWER_PRESS );
}

void 
Amplifier::_handleVolumeButtonISR() {
    ENCODER_DIR direction = mVolumeEncoder.process();
    switch( direction ) {
        case ENCODER_FORWARD:
            mAmplifierQueue.addFromISR( Message::MSG_VOLUME_UP );
            break;
        case ENCODER_BACKWARD:
            mAmplifierQueue.addFromISR( Message::MSG_VOLUME_DOWN );
            break;
        default:
            break;
    }
}

void 
Amplifier::_handleInputButtonEncoderISR() {
    ENCODER_DIR direction = mInputEncoder.process();
    switch( direction ) {
        case ENCODER_FORWARD:
            mAmplifierQueue.addFromISR( Message::MSG_INPUT_UP );
            break;
        case ENCODER_BACKWARD:
            mAmplifierQueue.addFromISR( Message::MSG_INPUT_DOWN );
            break;
        default:
            break;
    }
}

void 
Amplifier::_handleInputButtonISR() {

    mAmplifierQueue.addFromISR( Message::MSG_INPUT_BUTTON_PRESS );
   //  mAmplifierQueue.addFromISR( Message::MSG_INPUT_POWER_PRESS );


}

void 
Amplifier::setupPWM() {
    ledc_timer_config_t backlight_config = {};
    backlight_config.speed_mode         = LEDC_HIGH_SPEED_MODE;
    backlight_config.duty_resolution    = LEDC_TIMER_10_BIT;
    backlight_config.timer_num          = LEDC_TIMER_0;
    backlight_config.freq_hz            = 200;
    backlight_config.clk_cfg            = LEDC_USE_REF_TICK;

    ledc_channel_config_t ledc_channel = {};
    ledc_channel.gpio_num   = 2;
    ledc_channel.speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_channel.channel    = LEDC_CHANNEL_0;
    ledc_channel.intr_type  = LEDC_INTR_DISABLE;
    ledc_channel.timer_sel  = LEDC_TIMER_0;
    ledc_channel.duty       = 0;
    ledc_channel.hpoint     = 0;
    ledc_channel.flags.output_invert = 1;

    ledc_timer_config( &backlight_config );
    ledc_channel_config( &ledc_channel );
    
    ledc_set_duty( LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 500 );
	ledc_update_duty( LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0 );

    ledc_channel.channel = LEDC_CHANNEL_1;
    ledc_channel.gpio_num   = 23;
    ledc_channel_config( &ledc_channel );

    ledc_set_duty( LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, 500 );
	ledc_update_duty( LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1 );
}

void
Amplifier::configurePins() {
    gpio_install_isr_service( 0 );

    // Configure LEDs
    configureOnePin( PIN_LED_ACTIVE, GPIO_INTR_DISABLE, GPIO_MODE_OUTPUT, GPIO_PULLDOWN_ENABLE, GPIO_PULLUP_DISABLE );
    gpio_set_level( PIN_LED_ACTIVE, 0 );
    
    // The standby power LED indicator
    configureOnePin( PIN_LED_FRONT_STANDBY, GPIO_INTR_DISABLE, GPIO_MODE_OUTPUT, GPIO_PULLDOWN_ENABLE, GPIO_PULLUP_DISABLE );
    gpio_set_level( PIN_LED_FRONT_STANDBY, 1 );

    configureOnePin( PIN_LED_FRONT_POWER, GPIO_INTR_DISABLE, GPIO_MODE_OUTPUT, GPIO_PULLDOWN_ENABLE, GPIO_PULLUP_DISABLE );

    configureOnePin( PIN_LED_CONNECTED, GPIO_INTR_DISABLE, GPIO_MODE_OUTPUT, GPIO_PULLDOWN_ENABLE, GPIO_PULLUP_DISABLE );
    gpio_set_level( PIN_LED_CONNECTED, 0 );

    // Configure Output Triggers
    configureOnePin( PIN_RELAY, GPIO_INTR_DISABLE, GPIO_MODE_OUTPUT, GPIO_PULLDOWN_ENABLE, GPIO_PULLUP_DISABLE );
    gpio_set_level( PIN_RELAY, 0 );

    // Configure Buttons
    configureOnePin( PIN_BUTTON_VOLUME, GPIO_INTR_POSEDGE, GPIO_MODE_INPUT, GPIO_PULLDOWN_ENABLE, GPIO_PULLUP_DISABLE );
    gpio_isr_handler_add( PIN_BUTTON_VOLUME, Volume_Button_ISR, this );

    configureOnePin( GPIO_NUM_15, GPIO_INTR_ANYEDGE, GPIO_MODE_INPUT, GPIO_PULLDOWN_DISABLE, GPIO_PULLUP_ENABLE );
    gpio_isr_handler_add( GPIO_NUM_15, Volume_Button_ISR, this );

    configureOnePin( GPIO_NUM_13, GPIO_INTR_ANYEDGE, GPIO_MODE_INPUT, GPIO_PULLDOWN_DISABLE, GPIO_PULLUP_ENABLE );
    gpio_isr_handler_add( GPIO_NUM_13, Volume_Button_ISR, this );

    configureOnePin( PIN_BUTTON_INPUT, GPIO_INTR_NEGEDGE, GPIO_MODE_INPUT, GPIO_PULLDOWN_DISABLE, GPIO_PULLUP_ENABLE );
    gpio_isr_handler_add( PIN_BUTTON_INPUT, Input_Button_ISR, this );

    configureOnePin( GPIO_NUM_4, GPIO_INTR_ANYEDGE, GPIO_MODE_INPUT, GPIO_PULLDOWN_DISABLE, GPIO_PULLUP_ENABLE );
    gpio_isr_handler_add( GPIO_NUM_4, Input_Button_Encoder_ISR, this );

    configureOnePin( GPIO_NUM_16, GPIO_INTR_ANYEDGE, GPIO_MODE_INPUT, GPIO_PULLDOWN_DISABLE, GPIO_PULLUP_ENABLE );
    gpio_isr_handler_add( GPIO_NUM_16, Input_Button_Encoder_ISR, this );

    configureOnePin( PIN_BUTTON_POWER, GPIO_INTR_POSEDGE, GPIO_MODE_INPUT, GPIO_PULLDOWN_ENABLE, GPIO_PULLUP_DISABLE );
    gpio_isr_handler_add( PIN_BUTTON_POWER, Power_Button_ISR, this );

    // Configure Others
    configureOnePin( PIN_BACKLIGHT, GPIO_INTR_DISABLE, GPIO_MODE_OUTPUT, GPIO_PULLDOWN_ENABLE, GPIO_PULLUP_DISABLE );
    gpio_set_level( PIN_BACKLIGHT, 1 );

    // Decoder reset
    configureOnePin( PIN_DECODER_RESET, GPIO_INTR_DISABLE, GPIO_MODE_OUTPUT, GPIO_PULLDOWN_ENABLE, GPIO_PULLUP_DISABLE );
    gpio_set_level( PIN_DECODER_RESET, 0 );

    // Setting up decoder
    configureOnePin( PIN_DECODER_IRQ, GPIO_INTR_DISABLE, GPIO_MODE_INPUT, GPIO_PULLDOWN_DISABLE, GPIO_PULLUP_ENABLE );
    gpio_isr_handler_add( PIN_DECODER_IRQ, Decoder_ISR, this );
    gpio_set_intr_type( PIN_DECODER_IRQ, GPIO_INTR_NEGEDGE );
}

void 
Amplifier::configureOnePin( PIN pin, gpio_int_type_t interrupts, gpio_mode_t mode, gpio_pulldown_t pulldown, gpio_pullup_t pullup ) {
    uint_fast32_t mask = ( ((uint_fast32_t)1) << ( uint_fast32_t ) pin );
    gpio_config_t io_conf;

	io_conf.intr_type = interrupts;
	io_conf.mode = mode;
	io_conf.pin_bit_mask = mask;
	io_conf.pull_down_en = pulldown;
	io_conf.pull_up_en = pullup;

	gpio_config( &io_conf );   
}
