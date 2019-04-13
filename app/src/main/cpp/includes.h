//
// Created by dongsheng cao on 2019/1/12.
//

#ifndef AND_INCLUDES_H
#define AND_INCLUDES_H

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavformat/avio.h"
#include "libavfilter/avfilter.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
#include "libavutil/opt.h"
#include "libavutil/mathematics.h"
#include "libavutil/imgutils.h"
#include "libavutil/timestamp.h"
#include "libavutil/time.h"
#include "libavutil/avutil.h"
#include "libavutil/avstring.h"
#include "sdl2/SDL.h"
#include "x264/x264.h"
#include "fdk-aac/aacenc_lib.h"
#include "fdk-aac/aacdecoder_lib.h"
#include "logger.h"
};


#endif //AND_M_INCLUDES_H
