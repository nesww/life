#include "../include/life.h"
#include <stdint.h>
#include <stdio.h>

static uint8_t err = 0x0;

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
        break;
      }
        //
      case SDL_MOUSEBUTTONDOWN: {
      }

      case SDL_QUIT: {
        close_flag |= 0x1;
        break;
      }
        //
      }
    }

    if (start_life_flag) {

      int row = 0;
      int pixel_scale = 5;
      for (int i = 0; i < WORLD_W * WORLD_H; ++i) {

        // TODO: add logic rules for life/death of cells
        // TODO: check array logic for borders (avoid reading out to check
        // neighbors) lv->cells[i + WORLD_W + 1] =

        SDL_Rect r = {(i % WORLD_W) * pixel_scale, row * pixel_scale,
                      pixel_scale, pixel_scale};
        uint32_t color = 0x000000;
        if (lv->cells[i]) {
          color = 0xffffff;
        }
        SDL_FillRect(sdl->surface, &r, color);
        if (i % WORLD_W == 0)
          ++row;
      }
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
  // TODO: free cells & cache IF lv loaded flags is up (avoid double free)
  free(lv);
  fprintf(stdout, "[loop] Closing...\n");
  return err;
}
