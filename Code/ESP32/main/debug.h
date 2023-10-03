#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "esp_log.h"
#include <sstream>

#define AMP_DEBUG_I(x) ESP_LOGI( __PRETTY_FUNCTION__, x )
#define AMP_DEBUG_SI(x) { std::stringstream s; s << x; ESP_LOGI( __PRETTY_FUNCTION__, "%s", s.str().c_str() ); }
#define AMP_DEBUG_W(x) ESP_LOGW( __PRETTY_FUNCTION__, x )
#define AMP_DEBUG_SW(x) { std::stringstream s; s << x; ESP_LOGW( __PRETTY_FUNCTION__, "%s", s.str().c_str() ); }
#define AMP_DEBUG_E(x) ESP_LOGE( __PRETTY_FUNCTION__, x )

#endif 