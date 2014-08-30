#ifndef _SPRITE_H_
#define _SPRITE_H_

#include <SDL.h>
#include <SDL_image.h>

#include <map>

typedef struct tagSpritePart
{
  unsigned short usWidth;
  unsigned short usHeight;
  unsigned short X, Y;
} SpritePart_t;

class CSprite
{
 public:
  CSprite(SDL_Renderer *pRenderer, const char *szImageFileName, const char *szTxtFileName);
  virtual ~CSprite();

  void                  Draw(SDL_Renderer *pRenderer, const char *szTag, int x, int y);
  void                  SetAlpha(unsigned char alpha) { SDL_SetTextureAlphaMod(m_pTexture, alpha); }
  void                  SetColorMod(unsigned char r, unsigned char g, unsigned char b) { SDL_SetTextureColorMod(m_pTexture, r, g, b); }

 private:
  void                  Load(SDL_Renderer *pRenderer, const char *szImageFileName, const char *szTxtFileName);
  bool                  LoadTxt(const char *szTxtFileName);

  SDL_Texture          *m_pTexture;
  int                   m_iTextureWidth;
  int                   m_iTextureHeight;
  std::map<unsigned int, SpritePart_t>    m_mapSpriteParts;
};

#endif /* _SPRITE_H_ */








