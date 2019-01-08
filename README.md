# Side Scroller Engine

This is a side scroller engine that can be used to build games like Super Mario Bros.
## Feature Overview

- 2D rendering with static background
- Text file as map detail allowing easy editing of objects
- AABB based collision detection
- Multi platform (Windows, MacOS and GNU Linux)

## Technology Stack

- SDL2 for rendering and platform Abstraction
- SDL mixer for sound
- SDL ttf for font rendering
- Cmake as build system

Regular updates are shared as videos. Check out at:
[![Side scroller Version 0.1f](http://img.youtube.com/vi/Q2I64npC_pQ/0.jpg)](https://www.youtube.com/watch?v=Q2I64npC_pQ)

## Building

After installing the dependencies

```bash
$ mkdir build
$ cd build
$ cmake ../
```

## Running

### In Application:
- `wasd` or arrow keys for walking.
- `space` for jumping.
