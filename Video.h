#ifndef _VIDEO_H_
#define _VIDEO_H_

extern SDL_Window        *gpWindow;
extern SDL_Renderer      *gpRenderer;

bool VideoInit();
void VideoDestroy();

#endif /* _VIDEO_H_ */
