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

  return 0; // TODO
}
