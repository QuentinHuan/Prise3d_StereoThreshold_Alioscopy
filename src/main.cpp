//-----------------------------------------------------
/*

A minimal OpenGL Cmake project

    uses: SDL2, glad, OpenGL, [openVR]

    additional : lodepng and Mascreences lib in  include folder

*/
//-----------------------------------------------------
#include <glad/glad.h>
#include <SDL2/SDL.h>

#include "shader.h"
#include "mesh.h"

#include "texture.h"

#include "context.h"

#include <stdio.h>
#include <stdlib.h>

//--------------------------------------------------------
//scene
//--------------------------------------------------------
// geometry
float vertices_screen[] = {
    -1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
    1.0f, -1.0f, 0.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 0.0f, 1.0f, 0.0f,

    1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    -1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
    -1.0f, -1.0f, 0.0f, 0.0f, 1.0f};

float vertices[] = {
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
    1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f, 1.0f, 1.0f,

    1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f};

float vertices_tri[] = {
    -0.5f,
    -0.5f,
    0.0f,
    0.0f,
    0.0f,
    0.5f,
    -0.5f,
    0.0f,
    0.0f,
    0.0f,
    0.5f,
    0.5f,
    0.0f,
    0.0f,
    0.0f,
};

Mesh screen;
Shader alioscopy_Shader;
Shader fb_shader;

Mesh tri;
Shader triShader;

// framebuffer
unsigned int framebuffer[8];
unsigned int fb_texture[8];
Texture image_texture[8];
unsigned int rbo[8];

static void draw(SDL_Window *window)
{
}

static void sceneSetup()
{
  //(float *vertices, GLsizei count, Shader shader, unsigned int texture)
  screen.load(&vertices_screen[0], sizeof(vertices_screen) / sizeof(float), alioscopy_Shader, 0);
  tri.load(&vertices[0], sizeof(vertices) / sizeof(float), triShader, 0);
}

int main(int argc, char *argv[])
{
  if (!init())
  {
    printf("failed\n");
  }

  glViewport(0, 0, windowWidth, windowHeight);

  alioscopy_Shader.init("../shader/fb.vs", "../shader/glsl_mix_update.fs");
  fb_shader.init("../shader/fb.vs", "../shader/fb.fs");
  triShader.init("../shader/fb.vs", "../shader/fb.fs");


// setting up framebuffers
  for (int i = 0; i < 8; i++)
  {
    // framebuffer init
    glGenFramebuffers(1, &framebuffer[i]);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer[i]);

    // texture for framebuffer
    glGenTextures(1, &fb_texture[i]);
    glBindTexture(GL_TEXTURE_2D, fb_texture[i]);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // bind texture to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb_texture[i], 0);

    // render buffer object
    glGenRenderbuffers(1, &rbo[i]);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo[i]);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, windowWidth, windowHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo[i]);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete! " << i << "/8" << std::endl;
  }

// loading textures:
  for (int i = 0; i < 8; i++)
  {
    image_texture[i].load("../res/p3d_crown-0"+std::to_string(i+1)+"_00100.png");
  }


  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  //glBindFramebuffer(GL_FRAMEBUFFER, 0); // default framebuffer
  sceneSetup();
  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe
  while (1)
  {
    event();

    for (int i = 0; i < 8; i++)
    {
      glBindFramebuffer(GL_FRAMEBUFFER, framebuffer[i]);
      glEnable(GL_DEPTH_TEST);
      glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      //glActiveTexture(GL_TEXTURE0);
      tri.texture=image_texture[i].ID;
      //triShader.use();
      //float c = ((float)(1+i))/8.0f;
      //triShader.setVec4("color", glm::vec4( c, c, c, 1));
      
      tri.draw(triShader);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);


    screen.shader.use();
    
    for (unsigned int i = 0; i < 8; i++)
    { 
      glActiveTexture(GL_TEXTURE0+i);
      glBindTexture(GL_TEXTURE_2D, fb_texture[i]);
      screen.shader.setInt("srcTextures["+std::to_string(i)+"]",i);  
    }
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fb_texture[0]);

    //fb_shader.use();
    glBindVertexArray(screen.VAO);
    glDrawArrays(GL_TRIANGLES, 0, screen.vertexCount);


    glBindVertexArray(0);
    glUseProgram(0);

    SDL_GL_SwapWindow(window);
  }

  // framebuffer delete
  for (int i = 0; i < 8; i++)
  {
    glDeleteFramebuffers(1, &framebuffer[i]);
  }
  return 0;
}