#include "Sprite.h"

CSprite::CSprite(SDL_Renderer *pRenderer, const char *szImageFileName, const char *szTxtFileName)
{
  Load(pRenderer, szImageFileName, szTxtFileName);
}

CSprite::~CSprite()
{
  if (m_pTexture != NULL)
    {
      SDL_DestroyTexture(m_pTexture);
    }
}

/**
 * This hash function has been taken from an Article in Dr Dobbs Journal.
 * This is normally a collision-free function, distributing keys evenly.
 * Collision can be avoided by comparing the key itself in last resort.
 */
inline unsigned int CalcTag(const char *sz)
{
  unsigned int hash = 0;

  while (*sz)
    {
      hash += (unsigned short)*sz;
      hash += (hash << 10);
      hash ^= (hash >> 6);

      sz++;
    }

  hash += (hash << 3);
  hash ^= (hash >> 11);
  hash += (hash << 15);

  return hash;
}

void CSprite::Draw(SDL_Renderer *pRenderer, const char *szTag, int x, int y)
{
  unsigned int uiTag = CalcTag(szTag);

  std::map<unsigned int, SpritePart_t>::iterator it = m_mapSpriteParts.find(uiTag);

  if (it != m_mapSpriteParts.end())
    {
      SDL_Rect srcrect, dstrect;

      srcrect.x = it->second.X;
      srcrect.y = it->second.Y;
      srcrect.w = it->second.usWidth;
      srcrect.h = it->second.usHeight;

      dstrect.x = x;
      dstrect.y = y;
      dstrect.w = it->second.usWidth;
      dstrect.h = it->second.usHeight;

      SDL_RenderCopy(pRenderer, m_pTexture, &srcrect, &dstrect);
    }
}

void CSprite::DrawEx(SDL_Renderer *pRenderer, const char *szTag, int x, int y, double angle, SDL_RendererFlip flip)
{
  unsigned int uiTag = CalcTag(szTag);

  std::map<unsigned int, SpritePart_t>::iterator it = m_mapSpriteParts.find(uiTag);

  if (it != m_mapSpriteParts.end())
    {
      SDL_Rect srcrect, dstrect;

      srcrect.x = it->second.X;
      srcrect.y = it->second.Y;
      srcrect.w = it->second.usWidth;
      srcrect.h = it->second.usHeight;

      dstrect.x = x;
      dstrect.y = y;
      dstrect.w = it->second.usWidth;
      dstrect.h = it->second.usHeight;

      SDL_RenderCopyEx(pRenderer, m_pTexture, &srcrect, &dstrect, angle, NULL, flip);
    }
}

void CSprite::Load(SDL_Renderer *pRenderer, const char *szImageFileName, const char *szTxtFileName)
{
  SDL_Surface *pSurface = SDL_LoadBMP(szImageFileName);

  if (pSurface == NULL)
    {
      fprintf(stderr, "CSprite::Load(): IMG_Load failed: %s\n", SDL_GetError());
      return;
    }

  m_iTextureWidth = pSurface->w;
  m_iTextureHeight = pSurface->h;

  m_pTexture = SDL_CreateTextureFromSurface(pRenderer, pSurface);
  SDL_FreeSurface(pSurface);

  if (m_pTexture == NULL)
    {
      fprintf(stderr, "CSprite::Load(): SDL_CreateTextureFromSurface failed: %s\n", SDL_GetError());
      return;
    }

  // Load txt file
  if (!LoadTxt(szTxtFileName))
    {
      SDL_DestroyTexture(m_pTexture);
      m_pTexture = NULL;

      fprintf(stderr, "CSprite::Load(): LoadTxte failed\n");
      return;
    }
}

bool CSprite::LoadTxt(const char *szTxtFileName)
{
  FILE *fp = fopen(szTxtFileName, "r");

  if (fp == NULL)
    {
      return false;
    }

  while (!feof(fp))
    {
      char name[256];
      int w, h;
      float x1, y1, x2, y2;

      if (fscanf(fp, "%s %d %d %f %f %f %f", name, &w, &h, &x1, &y1, &x2, &y2) != 7)
	{
	  continue;
	}

      SpritePart_t spritePart;

      spritePart.usWidth = w;
      spritePart.usHeight = h;
      spritePart.X = (unsigned short)(m_iTextureWidth * x1);
      spritePart.Y = (unsigned short)(m_iTextureHeight * y1);

      unsigned int uiTag = CalcTag(name);

      if (m_mapSpriteParts.find(uiTag) == m_mapSpriteParts.end())
	{
	  m_mapSpriteParts[uiTag] = spritePart;
	}
      else
	{
	  fprintf(stderr, "CSprite::LoadTxt(): WARNING, duplicate tag: %s %u\n", name, uiTag);
	}
    }

  fclose(fp);
  return true;
}
