#include <SDL.h>

#include "Video.h"

SDL_Window        *gpWindow           = NULL;
SDL_Renderer      *gpRenderer         = NULL;

bool VideoInit()
{
  gpWindow = SDL_CreateWindow("Flappy Bird",
			      SDL_WINDOWPOS_UNDEFINED,
			      SDL_WINDOWPOS_UNDEFINED,
			      SCREEN_WIDTH,
			      SCREEN_HEIGHT,
			      SDL_WINDOW_SHOWN);

  if (gpWindow == NULL)
    {
      return false;
    }

  gpRenderer = SDL_CreateRenderer(gpWindow, -1, SDL_RENDERER_ACCELERATED);

  if (gpRenderer == NULL)
    {
      SDL_DestroyWindow(gpWindow);
      return false;
    }

  #if defined (__IPHONEOS__)
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
  SDL_GL_SetAttribute(SDL_GL_RETAINED_BACKING, 1);
  #endif

  return true;
}

void VideoDestroy()
{
  if (gpRenderer != NULL)
    {
      SDL_DestroyRenderer(gpRenderer);
      gpRenderer = NULL;
    }

  if (gpWindow != NULL)
    {
      SDL_DestroyWindow(gpWindow);
      gpWindow = NULL;
    }
}
