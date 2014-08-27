#include <SDL.h>
#include <SDL_image.h>

#include "BirdMain.h"

int main(int argc, char *argv[])
{
  // initialize SDL
  if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0)
    {
      fprintf(stderr, "SDL_Init() failed: %s\n", SDL_GetError());
      return 255;
    }
  atexit(SDL_Quit);

  // initialize SDL_mixer
  if (IMG_Init(IMG_INIT_PNG) < 0)
    {
      fprintf(stderr, "SDL_Init() failed: %s\n", SDL_GetError());
      return 255;
    }

  atexit(IMG_Quit);


  return 0;
}
