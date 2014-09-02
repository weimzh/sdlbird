#include <SDL.h>
#include <SDL_image.h>

#include "BirdGame.h"
#include "Sprite.h"
#include "Video.h"

#include <algorithm>
#include <math.h>
#include <stdlib.h>
#include <time.h>

static CSprite *gpSprite = NULL;

typedef enum tagGameState
  {
    GAMESTATE_INITIAL = 0,
    GAMESTATE_GAMESTART,
    GAMESTATE_GAME,
    GAMESTATE_GAMEOVER,
  } GameState;

static GameState g_GameState = GAMESTATE_INITIAL;
static bool g_bMouseDown = false;
static bool g_bNight = false;
static int g_iBirdPic = 0;
static int g_iMouseX = 0;
static int g_iMouseY = 0;
static int g_iScore = 0;
static float g_flBirdVelocity = 0;
static float g_flBirdHeight = 0;
static float g_flBirdAngle = 0;
static int g_iPipePosX[3] = { 0, 0, 0 };
static int g_iPipePosY[3] = { 0, 0, 0 };

#define GRAVITY      0.00015f
#define WINGPOWER    0.1f
#define ROTATION     0.07f
#define PIPEDISTANCE 180
#define PIPEWIDTH    50
#define BIRDWIDTH    30
#define BIRDMARGIN   10

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
	  g_iMouseX = evt.button.x;
	  g_iMouseY = evt.button.y;
	  break;

	case SDL_MOUSEBUTTONUP:
	  g_bMouseDown = false;
	  break;

	case SDL_FINGERDOWN:
	  g_bMouseDown = true;
	  g_iMouseX = (int)(evt.tfinger.x * SCREEN_WIDTH);
	  g_iMouseY = (int)(evt.tfinger.y * SCREEN_HEIGHT);
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
}

static void DrawLand(bool bStatic)
{
  static unsigned int time = 0;
  if (!bStatic)
    {
      time++;
    }

  gpSprite->Draw(gpRenderer, "land", -(int)((time / 20) % SCREEN_WIDTH), SCREEN_HEIGHT - 110);
  gpSprite->Draw(gpRenderer, "land", 287 - ((time / 20) % SCREEN_WIDTH), SCREEN_HEIGHT - 110);
}

static void DrawScore(int score)
{
  int iScoreLen = 0;
  int iBeginX = SCREEN_WIDTH / 2;
  int iReverseScore = 0;

  do
    {
      if (score % 10 == 1)
	{
	  iBeginX -= 16 / 2 + 1;
	}
      else
	{
	  iBeginX -= 24 / 2 + 1;
	}

      iReverseScore *= 10;
      iReverseScore += score % 10;

      score /= 10;
      iScoreLen++;
    } while (score > 0);

  do
    {
      char buf[256];
      sprintf(buf, "font_%.3d", 48 + (iReverseScore % 10));

      gpSprite->Draw(gpRenderer, buf, iBeginX, 60);
      if (iReverseScore % 10 == 1)
	{
	  iBeginX += 16 + 2;
	}
      else
	{
	  iBeginX += 24 + 2;
	}
      
      iReverseScore /= 10;
      iScoreLen--;
    } while (iScoreLen > 0);
}

static void GameThink_Initial()
{
  static unsigned int fading_start_time = 0;
  static GameState enNextGameState;

  if (fading_start_time > 0)
    {
      unsigned int elapsed = SDL_GetTicks() - fading_start_time;

      if (elapsed > 500)
	{
	  g_GameState = enNextGameState;
	  gpSprite->SetColorMod(255, 255, 255);
	  fading_start_time = 0;
	  g_bNight = ((rand() % 2) == 1);
	  g_iBirdPic = rand() % 3;
	  for (int i = 0; i < 3; i++)
	    {
	      g_iPipePosX[i] = SCREEN_WIDTH + 200 + i * PIPEDISTANCE;
	      g_iPipePosY[i] = rand() % 200;
	    }
	  return;
	}

      elapsed *= 255;
      elapsed /= 500;

      elapsed = 255 - elapsed;

      gpSprite->SetColorMod(elapsed, elapsed, elapsed);
    }

  DrawBackground();
  DrawLand(false);

  gpSprite->Draw(gpRenderer, "title", 55, 110);

  char buf[256];
  sprintf(buf, "bird0_%d", (SDL_GetTicks() / 200) % 3);
  gpSprite->Draw(gpRenderer, buf, 118, 180 + (int)(cos(SDL_GetTicks() / 2 * 3.14 / 180) * 5));

  gpSprite->Draw(gpRenderer, "button_rate", 105, 275);
  gpSprite->Draw(gpRenderer, "button_play", 25, 340);
  gpSprite->Draw(gpRenderer, "button_score", 145, 340);

  gpSprite->Draw(gpRenderer, "brand_copyright", 80, 450);

  if (g_bMouseDown)
    {
      if (g_iMouseX > 25 && g_iMouseY > 340 && g_iMouseX < 25 + 100 && g_iMouseY < 340 + 55)
	{
	  // user clicked "play" button
	  fading_start_time = SDL_GetTicks();
	  enNextGameState = GAMESTATE_GAMESTART;
	}
      else if (g_iMouseX > 145 && g_iMouseY > 340 && g_iMouseX < 145 + 100 && g_iMouseY < 340 + 55)
	{
	  // user clicked "score" button
	  // TODO
	}
    }
}

static void BirdFly()
{
  g_flBirdVelocity = WINGPOWER;
  g_flBirdAngle = -45;
  //PlaySound();
}

static void GameThink_GameStart()
{
  static unsigned int fading_start_time = 0;

  if (fading_start_time == 0)
    {
      fading_start_time = SDL_GetTicks();
    }

  unsigned int elapsed = SDL_GetTicks() - fading_start_time;

  if (elapsed < 500)
    {
      elapsed *= 255;
      elapsed /= 500;

      gpSprite->SetColorMod(elapsed, elapsed, elapsed);
    }
  else
    {
      gpSprite->SetColorMod(255, 255, 255);
    }

  DrawBackground();
  DrawLand(false);

  char buf[256];
  sprintf(buf, "bird%d_%d", g_iBirdPic, (SDL_GetTicks() / 200) % 3);
  g_flBirdHeight = 230 + (float)(cos(SDL_GetTicks() / 2 * 3.14 / 180) * 5);
  gpSprite->Draw(gpRenderer, buf, 60, (int)g_flBirdHeight);

  // draw score
  DrawScore(0);

  // draw "get ready" notice
  gpSprite->Draw(gpRenderer, "text_ready", 50, 130);

  // draw hint picture
  gpSprite->Draw(gpRenderer, "tutorial", 90, 220);

  if (g_bMouseDown)
    {
      g_GameState = GAMESTATE_GAME;
      gpSprite->SetColorMod(255, 255, 255);
      fading_start_time = 0;
      g_iScore = 0;
      BirdFly();
    }
}

static void GameThink_Game()
{
  static bool bPrevMouseDown = false;
  bool bGameOver = false;
	
  static bool bPrevInRange = false;

  static unsigned int time = 0;
  int i;

  g_flBirdHeight -= g_flBirdVelocity;
  g_flBirdVelocity -= GRAVITY;

  g_flBirdAngle += ROTATION;
  if (g_flBirdAngle > 85)
    {
      g_flBirdAngle = 85;
    }

  if (g_flBirdHeight < -50)
    {
      g_flBirdHeight = -50;
    }
  else if (g_flBirdHeight > SCREEN_HEIGHT - 150)
    {
      // bird has hit the ground
      g_flBirdHeight = SCREEN_HEIGHT - 150;
      bGameOver = true;
    }

  DrawBackground();

  // move pipes
  time++;
  if (time % 20 == 0)
    {
      for (i = 0; i < 3; i++)
	{
	  g_iPipePosX[i]--;
	}

      if (g_iPipePosX[0] < -PIPEWIDTH)
	{
	  g_iPipePosX[0] = g_iPipePosX[1];
	  g_iPipePosX[1] = g_iPipePosX[2];
	  g_iPipePosX[2] = g_iPipePosX[1] + PIPEDISTANCE;

	  g_iPipePosY[0] = g_iPipePosY[1];
	  g_iPipePosY[1] = g_iPipePosY[2];
	  g_iPipePosY[2] = rand() % 200;
	}
    }

  // draw pipes
  for (i = 0; i < 3; i++)
    {
      gpSprite->Draw(gpRenderer, "pipe_down", g_iPipePosX[i], -320 + 60 + g_iPipePosY[i]);
      gpSprite->Draw(gpRenderer, "pipe_up", g_iPipePosX[i], SCREEN_HEIGHT - 110 - 250 + g_iPipePosY[i]);
    }

  DrawScore(g_iScore);
  DrawLand(false);

  // draw bird
  char buf[256];
  sprintf(buf, "bird%d_%d", g_iBirdPic, (SDL_GetTicks() / 200) % 3);
  gpSprite->DrawEx(gpRenderer, buf, 60, (int)g_flBirdHeight, g_flBirdAngle, SDL_FLIP_NONE);

  // check if bird is in the range of a pipe
  if (g_iPipePosX[0] < 60 + BIRDWIDTH && g_iPipePosX[0] + PIPEWIDTH > 60 + BIRDMARGIN)
    {
      if (!bPrevInRange)
	{
	  g_iScore++;
	  //PlaySound(SCORE);
	}

      bPrevInRange = true;

      // check if the bird hits the pipe
      if (g_flBirdHeight + BIRDMARGIN < 60 + g_iPipePosY[0] ||
	  g_flBirdHeight + BIRDMARGIN + BIRDWIDTH > SCREEN_HEIGHT - 110 - 250 + g_iPipePosY[0])
	{
	  bGameOver = true;
	}
    }
  else
    {
      bPrevInRange = false;
    }

  if (bGameOver)
    {
      bPrevMouseDown = false;
      bPrevInRange = false;
      g_GameState = GAMESTATE_GAMEOVER;
      return;
    }

  if (g_bMouseDown && !bPrevMouseDown)
    {
      BirdFly();
    }

  bPrevMouseDown = g_bMouseDown;
}

static void GameThink_GameOver()
{
}

int GameMain()
{
  srand((unsigned int)time(NULL));

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

	case GAMESTATE_GAMESTART:
	  GameThink_GameStart();
	  break;

	case GAMESTATE_GAME:
	  GameThink_Game();
	  break;

	case GAMESTATE_GAMEOVER:
	  GameThink_GameOver();
	  break;

	default:
	  fprintf(stderr, "invalid game state: %d\n", (int)g_GameState);
	  exit(255);
	}

      SDL_RenderPresent(gpRenderer);
    }

  return 255; // shouldn't really reach here
}
