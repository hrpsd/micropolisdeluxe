#if defined(_WIN64)
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#if defined(__APPLE__)
#define SDL_INCLUDE_IMAGE <SDL2_image/SDL_image.h>
#define SDL_INCLUDE_TTF <SDL2_ttf/SDL_ttf.h>
#define SDL_INCLUDE_MIXER <SDL2_mixer/SDL_mixer.h>
#define SDL_INCLUDE_PIXELS <SDL2/SDL_pixels.h>
#elif defined(_WIN64)
#define SDL_INCLUDE_IMAGE <SDL_image.h>
#define SDL_INCLUDE_TTF <SDL_ttf.h>
#define SDL_INCLUDE_MIXER <SDL_mixer.h>
#define SDL_INCLUDE_PIXELS <SDL_pixels.h>
#else
#define SDL_INCLUDE_IMAGE <SDL2/SDL_image.h>
#define SDL_INCLUDE_TTF <SDL2/SDL_ttf.h>
#define SDL_INCLUDE_MIXER <SDL2/SDL_mixer.h>
#define SDL_INCLUDE_PIXELS <SDL2/SDL_pixels.h>
#endif
