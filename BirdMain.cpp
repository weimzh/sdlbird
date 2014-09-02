#include <SDL.h>

#include "BirdMain.h"
#include "Video.h"

int main(int argc, char *argv[])
{
  // initialize SDL
  if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0)
    {
      fprintf(stderr, "SDL_Init() failed: %s\n", SDL_GetError());
      return 255;
    }
  atexit(SDL_Quit);

  // initialize video
  if (!VideoInit())
    {
      fprintf(stderr, "VideoInit() failed: %s\n", SDL_GetError());
      return 255;
    }

  atexit(VideoDestroy);

  // initialize audio
  if (!InitSound())
    {
      fprintf(stderr, "InitSound() failed: %s\n", SDL_GetError());
      return 255;
    }

  atexit(DestroySound);

  return GameMain();
}
