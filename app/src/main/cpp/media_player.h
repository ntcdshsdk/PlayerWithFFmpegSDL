//
// Created by dongsheng cao on 2019/2/20.
//

#ifndef AND_MEDIA_PLAYER_H
#define AND_MEDIA_PLAYER_H

#include "player.h"
#include "includes.h"

#define EVENT_REFRESH (SDL_USEREVENT+1)
#define EVENT_QUIT (SDL_USEREVENT+9)
#define BLOCK_SIZE 4096000

class MediaPlayer {
private:
    int thread_exit = 0;

    int refresh_video_timer(void *udata);

public:
    int frame_rotate_90(AVFrame *src, AVFrame *dst);

    int play(JNIEnv *env,jstring mfp_, jobject surface);

    int prepare();

    int start(const char *mfp);

    int pause();

    int resume();

    int peek(long position);

    int stop();
};


#endif //AND_MEDIA_PALYER_H
