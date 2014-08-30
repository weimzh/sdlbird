#ifndef _VIDEO_H_
#define _VIDEO_H_

extern SDL_Window        *gpWindow;
extern SDL_Renderer      *gpRenderer;

bool VideoInit();
void VideoDestroy();

#define SCREEN_WIDTH 287
#define SCREEN_HEIGHT 511

#endif /* _VIDEO_H_ */
