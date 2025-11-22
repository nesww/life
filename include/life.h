#include <SDL2/SDL.h>
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#define SDL_INIT_SUCCESS 0x0
#define SDL_BAD_INIT 0x1
#define SDL_WIN_INIT_FAIL 0x2
#define SDL_SURFACE_INIT_FAIL 0x3
#define SDL_RENDERER_INIT_FAIL 0x4

#define DEFAULT_WORLD_W WIN_WIDTH / 5
#define DEFAULT_WORLD_H WIN_HEIGHT / 5
#define DEFAULT_GAME_SPEED 20 // gen per second

struct vec2 {
  int x;
  int y;
};

struct SDL_vars {
  SDL_Window *win;
  SDL_Surface *surface;
  SDL_Renderer *renderer;
};

struct life_vars {
  uint8_t *cells;
  uint8_t *cells_cache;
};
