//
// Created by dongsheng cao on 2019/2/20.
//

#include "media_player.h"
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <android/surface_texture.h>
#include <android/surface_texture_jni.h>
#include <zconf.h>
#include <dlfcn.h>


MediaPlayer *mediaPlayer;

int MediaPlayer::play(JNIEnv *env, jstring mfp_, jobject surface) {

    return 0;
}


int MediaPlayer::prepare() {
    return 0;
}

int MediaPlayer::start(const char *mfp) {
    int win_width, win_height;
    int video_width, video_height;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    SDL_Rect rect;
    SDL_Event event;
    SDL_Thread *thread;
    Uint8 video_buf[BLOCK_SIZE];

    size_t read_buff_len = 0;
    Uint8 *video_pos = NULL;
    Uint8 *video_end = NULL;
    unsigned int blank_space_len = 0;
    unsigned int remain_len = 0;

    unsigned int yuv_frame_len = video_width * video_height * 12 / 8;

    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow("AndPlayer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              win_width, win_height,
                              SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!window) {
        LOGE("SDL Create window failed!");
        return -1;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        LOGE("SDL Create renderer failed!");
        return -1;
    }

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING,
                                video_width, video_height);

//    SDL_Surface *surface = SDL_LoadBMP("");
    SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);

    FILE *infile = fopen(mfp, "r");
    read_buff_len = fread(video_buf, 1, BLOCK_SIZE, infile);
    if (read_buff_len <= 0) {
        return -1;
    }

    video_pos = video_buf;
    video_end = video_buf + read_buff_len;
    blank_space_len = BLOCK_SIZE - read_buff_len;
//    thread = SDL_CreateThread(refresh_video_timer, NULL, NULL);

    do {
        SDL_WaitEvent(&event);
        if (event.type == EVENT_REFRESH) {
            if ((video_pos + yuv_frame_len) > video_end) {
                remain_len = video_end - video_pos;
                if (remain_len && !blank_space_len) {
                    //copy data to header of buffer
                    memcpy(video_buf, video_pos, remain_len);
                    blank_space_len = BLOCK_SIZE - remain_len;
                    video_pos = video_buf;
                    video_end = video_buf + remain_len;
                }

                if (video_end == (video_buf + BLOCK_SIZE)) {
                    video_pos = video_buf;
                    video_end = video_buf;
                    blank_space_len = BLOCK_SIZE;
                }

                if ((read_buff_len = fread(video_end, 1, blank_space_len, infile)) <= 0) {
                    fprintf(stderr, "eof, exit thread!");
                    thread_exit = 1;
                    continue;// to wait event for exiting
                }

                //reset video_end
                video_end += read_buff_len;
                blank_space_len -= read_buff_len;
            }
            SDL_UpdateTexture(texture, NULL, 0, 0);

            rect.x = 0;
            rect.y = 0;
            rect.w = win_width;
            rect.h = win_height;

            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, &rect);
            SDL_RenderPresent(renderer);
        } else if (event.type == SDL_WINDOWEVENT) {
            SDL_GetWindowSize(window, &win_width, &win_height);
        } else if (event.type == SDL_QUIT) {
            thread_exit = 1;
        } else if (event.type == EVENT_QUIT) {
            break;
        }
    } while (true);

    if (infile) fclose(infile);

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

int MediaPlayer::pause() {
    SDL_SetMainReady();
    return 0;
}

int MediaPlayer::resume() {
    return 0;
}

int MediaPlayer::peek(long position) {
    return 0;
}

int MediaPlayer::stop() {
    return 0;
}

int MediaPlayer::refresh_video_timer(void *udata) {

    thread_exit = 0;

    while (!thread_exit) {
        SDL_Event event;
        event.type = EVENT_REFRESH;
        SDL_PushEvent(&event);
        SDL_Delay(40);
    }

    thread_exit = 0;

    //push quit event
    SDL_Event event;
    event.type = EVENT_QUIT;
    SDL_PushEvent(&event);

    return 0;
}

int MediaPlayer::frame_rotate_90(AVFrame *src, AVFrame *dst) {
    int n = 0;
    int hw = src->width >> 1;
    int hh = src->height >> 1;
    int size = src->width * src->height;
    int hsize = size >> 2;

    int pos = 0;
    //copy y
    for (int j = 0; j < src->width; j++) {
        pos = size;
        for (int i = src->height - 1; i >= 0; i--) {
            pos -= src->width;
            dst->data[0][n++] = src->data[0][pos + j];
        }
    }
    //copy uv
    n = 0;
    for (int j = 0; j < hw; j++) {
        pos = hsize;
        for (int i = hh - 1; i >= 0; i--) {
            pos -= hw;
            dst->data[1][n] = src->data[1][pos + j];
            dst->data[2][n] = src->data[2][pos + j];
            n++;
        }
    }

    dst->linesize[0] = src->height;
    dst->linesize[1] = src->height >> 1;
    dst->linesize[2] = src->height >> 1;
    dst->height = src->width;
    dst->width = src->height;

    return 0;
}


extern "C"
JNIEXPORT jint JNICALL
Java_com_osshare_media_play_AndPlayer_aNativePlay(JNIEnv *env, jobject instance, jstring mfp_, jobject surface) {
    const char *mfp = env->GetStringUTFChars(mfp_, 0);

    AVFormatContext *mfmt_ctx;
    int ret = avformat_open_input(&mfmt_ctx, mfp, NULL, NULL);
    if (ret < 0) {
        LOGE("打开失败");
        return -1;
    }
    //4.获取视频信息（将视频信息封装到上下文中）
    if (avformat_find_stream_info(mfmt_ctx, NULL) < 0) {
        LOGE("获取信息失败");
        return 0;
    }

    AVCodecContext *pCodecCtx = avcodec_alloc_context3(NULL);
    //5.用来记住视频流的索引
    int video_stream_index = -1;
    for (int index = 0; index < mfmt_ctx->nb_streams; index++) {
        LOGE("循环  %d", index);
        if (mfmt_ctx->streams[index]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = index;
            avcodec_parameters_to_context(pCodecCtx, mfmt_ctx->streams[index]->codecpar);
        }
    }

    //获取解码器（加密视频就是在此处无法获取）
    AVCodec *pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    LOGE("获取视频编码 %p", pCodec);

    //6.打开解码器。 （ffempg版本升级名字叫做avcodec_open2）
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        LOGE("解码失败");
        return 0;
    }
    //----------------------解码前准备--------------------------------------
    //准备开始解码时需要一个AVPacket存储数据（通过av_malloc分配内存）
    AVPacket *packet = (AVPacket *) av_malloc(sizeof(AVPacket));
    av_init_packet(packet);//初始化结构体

    //解封装需要AVFrame
    AVFrame *frame = av_frame_alloc();
    //声明一个rgb_Frame的缓冲区
    AVFrame *rgb_Frame = av_frame_alloc();
    //rgb_Frame  的缓冲区 初始化
    av_image_alloc(rgb_Frame->data, rgb_Frame->linesize, pCodecCtx->width, pCodecCtx->height,
                   AV_PIX_FMT_RGBA, 1);
    LOGE("宽 %d  高 %d", pCodecCtx->width, pCodecCtx->height);


    //格式转码需要的转换上下文（根据封装格式的宽高和编码格式，以及需要得到的格式的宽高）
    //pCodecCtx->pix_fmt 封装格式文件的上下文
    //AV_PIX_FMT_RGBA ： 目标格式 需要跟SurfaceView设定的格式相同
    //SWS_BICUBIC ：清晰度稍微低一点的算法（转换算法，前面的算法清晰度高效率低，下面的算法清晰度低效率高）
    //NULL,NULL,NULL ： 过滤器等
    SwsContext *swsContext = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
                                            pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_RGBA,
                                            SWS_BICUBIC, NULL, NULL, NULL);
    int frameCount = 0;

    //获取nativeWindow对象,准备进行绘制
    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);
    ANativeWindow_Buffer outBuffer;//申明一块缓冲区 用于绘制

    //------------------------一桢一帧开始解码--------------------
    int length = 0;
    int got_frame;
    while (av_read_frame(mfmt_ctx, packet) >= 0) {//开始读每一帧的数据
        if (packet->stream_index == video_stream_index) {//如果这是一个视频流

            AVStream *stream = mfmt_ctx->streams[video_stream_index];
            AVDictionaryEntry *m = NULL;
            while ((m = av_dict_get(stream->metadata, "", m, AV_DICT_IGNORE_SUFFIX)) != NULL) {
                LOGI("22222222====Key:%s ===value:%s\n", m->key, m->value);
            }


            //7.解封装（将packet解压给frame，即：拿到了视频数据frame）
            length = avcodec_decode_video2(pCodecCtx, frame, &got_frame, packet);//解封装函数
            LOGE(" 获得长度   %d 解码%d  ", length, frameCount++);
            if (got_frame > 0) {

                //8.准备绘制
                //配置绘制信息 宽高 格式(这个绘制的宽高直接决定了视频在屏幕上显示的情况，这样会平铺整个屏幕，可以根据特定的屏幕分辨率和视频宽高进行匹配)
                ANativeWindow_setBuffersGeometry(nativeWindow, pCodecCtx->width, pCodecCtx->height,
                                                 WINDOW_FORMAT_RGBA_8888);
                ANativeWindow_lock(nativeWindow, &outBuffer, NULL);//锁定画布(outBuffer中将会得到数据)
                //9.转码（转码上下文，原数据，一行数据，开始位置，yuv的缓冲数组，yuv一行的数据）
                sws_scale(swsContext, (const uint8_t *const *) frame->data, frame->linesize, 0,
                          frame->height, rgb_Frame->data,
                          rgb_Frame->linesize
                );
                //10.绘制
                uint8_t *dst = (uint8_t *) outBuffer.bits; //实际的位数
                int destStride = outBuffer.stride * 4; //拿到一行有多少个字节 RGBA
                uint8_t *src = rgb_Frame->data[0];//像素数据的首地址
                int srcStride = rgb_Frame->linesize[0]; //实际内存一行的数量
                for (int i = 0; i < pCodecCtx->height; ++i) {
                    //将rgb_Frame缓冲区里面的数据一行一行copy到window的缓冲区里面
                    //copy到window缓冲区的时候进行一些偏移设置可以将视频播放居中
                    memcpy(dst + i * destStride, src + i * srcStride, srcStride);
                }

                ANativeWindow_unlockAndPost(nativeWindow);//解锁画布
                usleep(1000 * 16);//可以根据帧率休眠16ms

            }
        }
        av_packet_unref(packet);
    }
    ANativeWindow_release(nativeWindow);//释放window
    av_frame_free(&frame);
    av_frame_free(&rgb_Frame);
    avcodec_close(pCodecCtx);
    avformat_free_context(mfmt_ctx);

    env->ReleaseStringUTFChars(mfp_, mfp);

    return 0;
}


extern "C"
JNIEXPORT jint JNICALL
Java_org_libsdl_app_SDLActivity_sdlPlayLocal(JNIEnv *env, jclass type, jstring mfp_) {
    const char *mfp = env->GetStringUTFChars(mfp_, NULL);

    AVFormatContext *mfmt_ctx = NULL;
    if (avformat_open_input(&mfmt_ctx, mfp, NULL, NULL) != 0) {
        LOGE("avformat_open_input failed \n");
    }
    //获取视频信息（将视频信息封装到上下文中）
    if (avformat_find_stream_info(mfmt_ctx, NULL) < 0) {
        LOGE("avformat_find_stream_info failed \n");
    }

    AVCodecContext *codec_ctx = avcodec_alloc_context3(NULL);
    //用来记住视频流的索引
    int video_stream_index = -1;
    for (int index = 0; index < mfmt_ctx->nb_streams; index++) {
        if (mfmt_ctx->streams[index]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = index;
            avcodec_parameters_to_context(codec_ctx, mfmt_ctx->streams[index]->codecpar);
        }
    }
    if (video_stream_index < 0) {
        LOGE("find video stream failed");
    }
    //获取解码器（加密视频就是在此处无法获取）
    AVCodec *codec = avcodec_find_decoder(codec_ctx->codec_id);
    if (!codec) {
        LOGE("avcodec_find_decoder failed");
    }
    if (avcodec_open2(codec_ctx, codec, NULL) < 0) {
        LOGE("avcodec_open2 failed");
    }

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        LOGE("SDL_Init failed - %s\n", SDL_GetError());
    }

    int win_width = codec_ctx->width * 1.5, win_height = codec_ctx->height * 1.5;
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Texture *texture = NULL;
    window = SDL_CreateWindow("AndPlayer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              win_width, win_height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!window) {
        LOGE("SDL_CreateWindow failed");
    }
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        LOGE("SDL_CreateRenderer failed");
    }
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING,
                                win_width, win_height);
    if (!texture) {
        LOGE("SDL_CreateTexture failed");
    }

    SwsContext *swsContext = sws_getContext(codec_ctx->width, codec_ctx->height, codec_ctx->pix_fmt,
                                            win_width, win_height, AV_PIX_FMT_YUV420P,
                                            SWS_BILINEAR, NULL, NULL, NULL);

    AVFrame *yuvFrame = av_frame_alloc();
    av_image_alloc(yuvFrame->data, yuvFrame->linesize, win_width, win_height, AV_PIX_FMT_YUV420P, 1);

    int ret = -1;
    SDL_Rect rect;
    AVPacket packet;
    AVFrame *originFrame = av_frame_alloc();

    while (av_read_frame(mfmt_ctx, &packet) >= 0) {
        if (packet.stream_index == video_stream_index) {
            if ((ret = avcodec_send_packet(codec_ctx, &packet)) == 0) {
                while (ret == 0) {
                    ret = avcodec_receive_frame(codec_ctx, originFrame);
                    if (ret == 0) {
                        sws_scale(swsContext, originFrame->data, originFrame->linesize,
                                  0, codec_ctx->height, yuvFrame->data, yuvFrame->linesize);
                        SDL_UpdateYUVTexture(texture, NULL,
                                             yuvFrame->data[0], yuvFrame->linesize[0],
                                             yuvFrame->data[1], yuvFrame->linesize[1],
                                             yuvFrame->data[2], yuvFrame->linesize[2]);
                        rect.x = 0;
                        rect.y = 0;
                        rect.w = win_width;
                        rect.h = win_height;
                        SDL_RenderClear(renderer);
                        SDL_RenderCopy(renderer, texture, NULL, &rect);
                        SDL_RenderPresent(renderer);
                    } else if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN) ||
                               ret == AVERROR(EINVAL)) {
                        LOGE("avcodec_receive_frame failed %s \n", av_err2str(ret));
                        break;
                    }
                }
            } else if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN) || ret == AVERROR(EINVAL) ||
                       ret == AVERROR(ENOMEM)) {
                LOGE("avcodec_send_packet failed %s \n", av_err2str(ret));
                break;
            }
        }
        av_packet_unref(&packet);
    }

    if (mfmt_ctx) {
        avformat_close_input(&mfmt_ctx);
        avformat_free_context(mfmt_ctx);
    }
    avcodec_close(codec_ctx);
    av_frame_free(&originFrame);
    av_frame_free(&yuvFrame);

    if (texture) {
        SDL_DestroyTexture(texture);
    }
    if (renderer) {
        SDL_DestroyRenderer(renderer);
    }
    if (window) {
        SDL_DestroyWindow(window);
    }

    SDL_Quit();
    env->ReleaseStringUTFChars(mfp_, mfp);

    return 0;
}