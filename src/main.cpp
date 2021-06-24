//-----------------------------------------------------
/*

A minimal OpenGL Cmake project

    uses: SDL2, glad, OpenGL, [openVR]

    additional : lodepng and Matrices lib in  include folder

*/
//-----------------------------------------------------
#include <glad/glad.h>
#include <SDL2/SDL.h>

#include "shader.h"
#include "mesh.h"

#include "context.h"

#include <stdio.h>
#include <stdlib.h>

//--------------------------------------------------------
//scene
//--------------------------------------------------------
//geometry
float vertices[] = {
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    0.0f, 0.5f, 0.0f};

Mesh tri;
Shader shader1;

static void draw(SDL_Window *window)
{
  tri.draw();
}

static void sceneSetup()
{
  tri.load(&vertices[0], sizeof(vertices) / sizeof(float), shader1);
}

int main(int argc, char *argv[])
{
  if (!init())
  {
    printf("failed\n");
  }

  glViewport(0, 0, windowWidth, windowHeight);

  shader1.init("../shader/vertex.vs", "../shader/fragment.fs");

  sceneSetup();

  while (1)
  {
    glClear(GL_COLOR_BUFFER_BIT);
    event();
    draw(window);
    SDL_GL_SwapWindow(window);
  }

  return 0;
}