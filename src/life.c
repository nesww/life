#include "../include/life.h"
#include <stdint.h>
#include <stdio.h>

#define PIXEL_SCALE 5

static uint8_t err = 0x0;
static uint8_t lv_loaded_flag = 0x0;
static long long gen = 0;

struct SDL_vars *init_sdl() {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    err |= SDL_BAD_INIT;
    return NULL;
  }

  SDL_Window *w = SDL_CreateWindow("Life", 100, 100, WIN_WIDTH, WIN_HEIGHT,
                                   SDL_WINDOW_SHOWN);
  if (!w) {
    err |= SDL_WIN_INIT_FAIL;
    return NULL;
  }

  SDL_Surface *s = SDL_GetWindowSurface(w);
  if (!s) {
    err |= SDL_SURFACE_INIT_FAIL;
    return NULL;
  }

  SDL_Renderer *r = SDL_GetRenderer(w);
  if (!r) {
    err |= SDL_RENDERER_INIT_FAIL;
    return NULL;
  }

  struct SDL_vars *v = malloc(sizeof(struct SDL_vars));
  v->win = w;
  v->surface = s;
  v->renderer = r;
  return v;
}

static int WORLD_W = DEFAULT_WORLD_W;
static int WORLD_H = DEFAULT_WORLD_H;
static int GENPERSEC = DEFAULT_GAME_SPEED;

void handle_args(int argc, char **argv) {
  switch (argc) {
    //
  case 2: {
    //
    WORLD_W = atoi(argv[1]);
  } break;
  case 3: {
    WORLD_W = atoi(argv[1]);
    WORLD_H = atoi(argv[2]);
    //
  } break;
  default: {
    WORLD_W = atoi(argv[1]);
    WORLD_H = atoi(argv[2]);
    GENPERSEC = atoi(argv[3]);
  } break;
  }
}

void generate_map(struct life_vars *lv) {
  for (int i = 0; i < WORLD_W * WORLD_H; ++i) {
    lv->cells[i] = rand() < rand() ? 0x1 : 0x0;
    lv->cells_cache[i] = lv->cells[i];
  }
  fprintf(stdout, "[load] Map generated\n");
}

int main(int argc, char **argv) {

  if (argc < 2) {
    fprintf(stdout, "[conf] No args given, default values will be used:\n");
  } else {
    handle_args(argc, argv);
    fprintf(stdout, "[conf] Configuration (some values may be defaulted if not "
                    "provided):\n");
  }
  fprintf(stdout, "[conf] \t * World dimensions: %dx%d\n", WORLD_W, WORLD_H);
  fprintf(stdout, "[conf] \t * Game speed: %d generations per second\n",
          GENPERSEC);

  struct SDL_vars *sdl = init_sdl();
  if (!sdl) {
    goto exec_err;
  }
  fprintf(stdout, "[load] Initialized SDL2, window will open\n");

  fprintf(stdout, "[load] Generating map...\n");

  struct life_vars *lv = malloc(sizeof(struct life_vars));
  lv->cells = malloc(sizeof(uint8_t) * WORLD_W * WORLD_H);
  lv->cells_cache = malloc(sizeof(uint8_t) * WORLD_W * WORLD_H);

  generate_map(lv);
  lv_loaded_flag = 0x1;

  // SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 255, 90, 120));
  fprintf(stdout, "[runn] Ready, press q to exit\n");
  fprintf(stdout, "[runn] Press SPACE to start/pause the execution\n");

  uint8_t close_flag = 0x0;
  uint8_t start_life_flag = 0x1;
  SDL_Event event;

  while (!close_flag) {

    while (SDL_PollEvent(&event)) {
      //
      switch (event.type) {
        //
      case SDL_KEYDOWN: {
        if (event.key.keysym.sym == SDLK_q) {
          close_flag |= 0x1;
          break;
        }
        if (event.key.keysym.sym == SDLK_SPACE) {
          start_life_flag ^= 0x1;
          fprintf(stdout, "[loop] Life game will %s\n",
                  start_life_flag ? "resume/start" : "stop");
        }
        if (event.key.keysym.sym == SDLK_g) {
          fprintf(stdout, "[info] Generation: %lld\n", gen);
        }
        break;
      }
        //
      case SDL_MOUSEBUTTONDOWN: {
        fprintf(stdout, "[evnt] mouse_click: x: %d y:%d, cell(%d,%d)\n",
                event.button.x, event.button.y, event.button.x / PIXEL_SCALE,
                event.button.y / PIXEL_SCALE);
        lv->cells[event.button.x + event.button.y * WORLD_W] ^= 0x1;
        lv->cells_cache[event.button.x + event.button.y * WORLD_W] ^= 0x1;
      } break;

      case SDL_QUIT: {
        close_flag |= 0x1;
        break;
      }
        //
      }
    }

    if (start_life_flag) {

      int row = 0;
      for (int i = 0; i < WORLD_W * WORLD_H; ++i) {

        // first row ignored for cells update
        if (i > WORLD_W && i < WORLD_H * (WORLD_W - 1)) {
          // if current cell not a border one
          if (((row * WORLD_W) + i) % WORLD_W >= 1 ||
              ((row * WORLD_W) + i) % WORLD_W <= WORLD_W) {

            uint8_t nw = lv->cells_cache[i - WORLD_W - 1];
            uint8_t n = lv->cells_cache[i - WORLD_W];
            uint8_t ne = lv->cells_cache[i - WORLD_W + 1];
            uint8_t e = lv->cells_cache[i + 1];
            uint8_t se = lv->cells_cache[i + WORLD_W + 1];
            uint8_t s = lv->cells_cache[i + WORLD_W];
            uint8_t sw = lv->cells_cache[i + WORLD_W - 1];
            uint8_t w = lv->cells_cache[i - 1];

            int neighbors = nw + n + ne + e + se + s + sw + w;
            if (lv->cells_cache[i]) {
              lv->cells[i] = neighbors == 3 || neighbors == 2 ? 0x1 : 0x0;
            } else {
              lv->cells[i] = neighbors == 3 ? 0x1 : 0x0;
            }
          }
        }

        SDL_Rect r = {(i % WORLD_W) * PIXEL_SCALE, row * PIXEL_SCALE,
                      PIXEL_SCALE, PIXEL_SCALE};
        uint32_t color = 0xffffff;
        if (lv->cells[i]) {
          color = 0x000000;
        }
        SDL_FillRect(sdl->surface, &r, color);
        if (i % WORLD_W == 0)
          ++row;
      }
      memcpy(lv->cells_cache, lv->cells, WORLD_W * WORLD_H);
      ++gen;
      SDL_UpdateWindowSurface(sdl->win);
      SDL_Delay(1000 / GENPERSEC);
    } else {
      SDL_Delay(50);
    }
  }

exec_err:
  if (err > SDL_INIT_SUCCESS)
    fprintf(stderr, "Error: something went wrong during execution: %s\n",
            SDL_GetError());
  goto clean;

clean:
  if (close_flag || err) {
    SDL_DestroyWindow(sdl->win);
    SDL_Quit();
  }

  free(sdl);

  if (lv_loaded_flag) {
    free(lv->cells);
    free(lv->cells_cache);
    free(lv);
  }

  fprintf(stdout, "[loop] Closing...\n");
  return err;
}
