//
// Created by dongsheng cao on 2019/2/17.
//

#ifndef AND_LOGGER_H
#define AND_LOGGER_H

#ifdef ANDROID

#include <android/log.h>

#define DEBUG    1
#define LOG_TAG    "AndMedia"
#define LOGI(FORMAT, ...) if(DEBUG){__android_log_print(ANDROID_LOG_INFO, LOG_TAG, FORMAT, ##__VA_ARGS__);}
#define LOGD(FORMAT, ...) if(DEBUG){__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, FORMAT, ##__VA_ARGS__);}
#define LOGE(FORMAT, ...) if(DEBUG){__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, FORMAT, ##__VA_ARGS__);}
#else
#define LOG(FORMAT, ...)  if(DEBUG){printf(LOG_TAG FORMAT "\n", ##__VA_ARGS__);}
#endif

#endif //AND_LOGGER_H
