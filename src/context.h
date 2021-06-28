#include <glad/glad.h>
#include <SDL2/SDL.h>

//--------------------------------------------------------
//SDL window
//--------------------------------------------------------
//window dimensions
int windowWidth = 1600;
int windowHeight = 900;

int windowPosx = 0;
int windowPosy = 0;

//window pointer
SDL_Window *window;

static bool init()
{
  //SDL init
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
  {
    printf("%s - SDL could not initialize! SDL Error: %s\n", __FUNCTION__, SDL_GetError());
    return false;
  }
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  SDL_GL_SetSwapInterval(1);

  //SDL window creation
  Uint32 windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

  window = SDL_CreateWindow("P3d_StereoThresholds_Alioscopy", windowPosx, windowPosy, windowWidth, windowHeight, windowFlags);
  if (window == NULL)
  {
    printf("%s - Window could not be created! SDL Error: %s\n", __FUNCTION__, SDL_GetError());
    return false;
  }

  //GL context creation
  SDL_GLContext windowGLContext = SDL_GL_CreateContext(window);
  if (windowGLContext == NULL)
  {
    printf("%s - OpenGL context could not be created! SDL Error: %s\n", __FUNCTION__, SDL_GetError());
    return false;
  }
  //glad load
  gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);

  return true;
}

static void quit()
{
  SDL_Quit();
  exit(0);
}

static void handle_key_down(SDL_Keysym *keysym)
{
  switch (keysym->sym)
  {
  case SDLK_ESCAPE:
    quit();
    break;

  default:
    break;
  }
}

static void event()
{
  SDL_Event event;

  while (SDL_PollEvent(&event))
  {

    switch (event.type)
    {
    case SDL_KEYDOWN:
      handle_key_down(&event.key.keysym);
      break;
    case SDL_QUIT:
      quit();
      break;
    }
  }
}

