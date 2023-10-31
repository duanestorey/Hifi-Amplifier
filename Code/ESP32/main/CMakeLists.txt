# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES )
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCS "main.cpp" "amplifier.cpp" "wifi.cpp" "timer.cpp" "mutex.cpp" "queue.cpp" "bluetooth.cpp" "i2c-bus.cpp" "lcd.cpp" "dac-pcm1681.cpp" "channelsel-ax2358.cpp" "tmp100.cpp" "encoder.cpp" "dolby-sta310.cpp" "button.cpp" )
set(COMPONENT_ADD_INCLUDEDIRS "")

register_component()


