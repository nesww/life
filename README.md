# Life

Still in progress and non-functional.
This small project is an attempt to emulate the [Conway's Game of Life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life) in C, using [SDL2](https://wiki.libsdl.org/SDL2/FrontPage) for visualization.

## Compile

See the provided [Makefile](./Makefile).
You only need to have SDL2 installed.
Compile with:

```bash
user@os:~/ mkdir build
user@os:~/ make
```

Binary will be in build folder.

## Usage

By default, it will start with a map of 160x120 cells (using 800x600 resolution, see [life.h](./include/life.h)) and a number of generations per second of 20.
You can change the map parameters with args given to the binary:

```bash
user@os:~/ ./build/life <map_width> <map_height> <gen_per_sec>
```

The order matters, but if one the parameters is missing, the missing ones will be defaulted.

**Since it's not complete, some weird behavior must be expected.**
