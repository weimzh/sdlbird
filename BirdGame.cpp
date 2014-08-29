#include <SDL.h>
#include <SDL_image.h>

#include "BirdGame.h"
#include "Sprite.h"
#include "Video.h"

static CSprite *gpSprite = NULL;

int GameMain()
{
  gpSprite = new CSprite(gpRenderer, "res/atlas.png", "res/atlas.txt");

  atexit([](void) { delete gpSprite; });
  int d = 0;
  while (1)
    {
      gpSprite->Draw(gpRenderer, "bg_day", -(d%287), 0);
      gpSprite->Draw(gpRenderer, "bg_day", 287-(d%287), 0);
      char sss[256];
      sprintf(sss, "bird1_%d", d%3);
      gpSprite->Draw(gpRenderer, sss, 12, 22);
      SDL_RenderPresent(gpRenderer);
      SDL_Delay(100);
      d++;
    }

  return 0; // TODO
}
