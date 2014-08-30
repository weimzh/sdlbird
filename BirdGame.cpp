#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "BirdGame.h"
#include "Sprite.h"
#include "Video.h"

#include <algorithm>
#include <math.h>

static CSprite *gpSprite = NULL;

typedef enum tagGameState
  {
    GAMESTATE_INITIAL = 0,
    GAMESTATE_GAME,
    GAMESTATE_SCORE,
  } GameState;

static GameState g_GameState = GAMESTATE_INITIAL;
static bool g_bMouseDown = false;
static bool g_bNight = false;

static void UpdateEvents()
{
  SDL_Event evt;

  while (SDL_PollEvent(&evt))
    {
      switch (evt.type)
	{
	case SDL_QUIT:
	  exit(0);
	  break;

	case SDL_MOUSEBUTTONDOWN:
	  g_bMouseDown = true;
	  break;

	case SDL_MOUSEBUTTONUP:
	  g_bMouseDown = false;
	  break;

	case SDL_FINGERDOWN:
	  g_bMouseDown = true;
	  break;

	case SDL_FINGERUP:
	  g_bMouseDown = false;
	  break;
	}
    }
}

static void ShowTitle()
{
  SDL_Surface *pSurfaceTitle = IMG_Load("res/splash.png");
  if (pSurfaceTitle == NULL)
    {
      fprintf(stderr, "cannot load res/splash.png\n");
      return;
    }

  SDL_Texture *pTextureTitle = SDL_CreateTextureFromSurface(gpRenderer, pSurfaceTitle);
  SDL_FreeSurface(pSurfaceTitle);

  if (pTextureTitle == NULL)
    {
      fprintf(stderr, "cannot create texture from splash.png\n");
      return;
    }

  unsigned int uiStartTime = SDL_GetTicks();

  while (SDL_GetTicks() - uiStartTime < 3000)
    {
      SDL_RenderCopy(gpRenderer, pTextureTitle, NULL, NULL);
      SDL_RenderPresent(gpRenderer);

      UpdateEvents();
      SDL_Delay(100);
    }

  SDL_DestroyTexture(pTextureTitle);
}

static void DrawBackground()
{
  gpSprite->Draw(gpRenderer, g_bNight ? "bg_night" : "bg_day", 0, 0);

  unsigned int time = SDL_GetTicks() / 10;
  gpSprite->Draw(gpRenderer, "land", -(time % 287), 401);
  gpSprite->Draw(gpRenderer, "land", 287 - (time % 287), 401);
}

static void GameThink_Initial()
{
  static unsigned int fading_start_time = 0;

  if (fading_start_time > 0)
    {
      unsigned int elapsed = SDL_GetTicks() - fading_start_time;

      if (elapsed > 500)
	{
	  g_GameState = GAMESTATE_GAME;
	  gpSprite->SetColorMod(255, 255, 255);
	  fading_start_time = 0;
	  return;
	}

      elapsed *= 255;
      elapsed /= 500;

      elapsed = 255 - elapsed;

      gpSprite->SetColorMod(elapsed, elapsed, elapsed);
    }

  DrawBackground();

  gpSprite->Draw(gpRenderer, "title", 55, 110);

  char buf[256];
  sprintf(buf, "bird0_%d", (SDL_GetTicks() / 200) % 3);
  gpSprite->Draw(gpRenderer, buf, 118, 180 + cos(SDL_GetTicks() / 2 * 3.14 / 180) * 5);

  gpSprite->Draw(gpRenderer, "button_rate", 105, 275);
  gpSprite->Draw(gpRenderer, "button_play", 25, 340);
  gpSprite->Draw(gpRenderer, "button_score", 145, 340);

  gpSprite->Draw(gpRenderer, "brand_copyright", 80, 450);

  if (g_bMouseDown)
    {
      fading_start_time = SDL_GetTicks();
    }
}

static void GameThink_Game()
{
}

static void GameThink_Score()
{
}

int GameMain()
{
  gpSprite = new CSprite(gpRenderer, "res/atlas.png", "res/atlas.txt");

  atexit([](void) { delete gpSprite; });

  ShowTitle();

  g_GameState = GAMESTATE_INITIAL;

  unsigned int uiCurrentTime = SDL_GetTicks();

  while (1)
    {
      // 10fps should be enough for a crappy game like this
      SDL_Delay(std::max<int>((int)((1000 / 10) - (SDL_GetTicks() - uiCurrentTime)), 0));
      UpdateEvents();

      switch (g_GameState)
	{
	case GAMESTATE_INITIAL:
	  GameThink_Initial();
	  break;

	case GAMESTATE_GAME:
	  GameThink_Game();
	  break;

	case GAMESTATE_SCORE:
	  GameThink_Score();
	  break;

	default:
	  fprintf(stderr, "invalid game state: %d\n", (int)g_GameState);
	  exit(255);
	}

      SDL_RenderPresent(gpRenderer);
    }

  return 255; // shouldn't really reach here
}
