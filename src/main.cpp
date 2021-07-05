//-----------------------------------------------------
/*
A minimal OpenGL Cmake project

    uses: SDL2, glad, OpenGL
    additional : lodepng lib in  include folder
*/
//-----------------------------------------------------
#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "shader.h"
#include "mesh.h"
#include "texture.h"
#include "context.h"
#include "utility.h"
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

// display screen
Mesh screen;
Shader alioscopy_Shader;

// framebuffers
unsigned int fb[8]; // framebuffers
unsigned int T_fb[8]; // rendering textures
unsigned int rbo[8]; // render buffer objects

// plane to render ref texture:
//Mesh ref_plane;
Mesh noisy_plane;
Shader texture_plane_shader;
Texture T_ref[8]; // ref 8pov images
Texture T_noise[8]; // noisy 8pov images

// internal state
int sceneID=0; // 0 is tutorial
Timer experimentTimer;
Timer patchUpdateTimer;


// config variable
//std::string ImageDatabasePath="/mnt/sda2/image/8pov";
//std::string ImageDatabasePath="/home/huan/pbrtOut/8pov/png (copy)";

//std::string ImageDatabasePath="/home/stagiaire/Bureau/image/8pov";
std::string ImageDatabasePath="/home/stagiaire/Bureau/image/8pov/square";

std::string sceneList={"8pov_caustic-view0"};//"8pov_crown"
int oneSceneDuration=20; // in millisec
int patchUpdateFrequency=5; // in millisec
int refValue=100; // image ID
int noiseValue=1; // image ID
int patchPos=1; // patch position in image block ID ([1,16])

// change the noise texture displayed on noisePlane
static void loadNoise(int level)
{
  for (int i = 0; i < 8; i++)
  {
    T_noise[i].load(ImageDatabasePath+"/p3d_crown-0"+std::to_string(i+1)+"/p3d_crown-0"+std::to_string(i+1)+"_"+leadingZeros(level,5)+".png");
  }
}

static void parseConfigFile()
{
  std::ifstream input( "../config/config.ini" );
  for( std::string line; getline( input, line ); )
  {
    //std::cout<<"line: "<< line <<std::endl;
    if(line.find("#"))// filter out comments
    {
      // split on '='
      std::vector<std::string> split = std::vector<std::string>();
      str_split(line,split,'=');
      if(split.size()>1)
      {
      std::string param = split.at(0);
      std::string value = split.at(1);
      if(param == "ImagePath")
      {ImageDatabasePath=value;}
      if(param == "referenceNoiseValue")
      {refValue=stoi(value);}
      }
    }
  }
}

static void sceneSetup()
{
  srand(10);
  // parse config file
  parseConfigFile();
  // setting up the 8 POV framebuffers
  for (int i = 0; i < 8; i++)
  {
    // framebuffer init
    glGenFramebuffers(1, &fb[i]);
    glBindFramebuffer(GL_FRAMEBUFFER, fb[i]);
    // texture for framebuffer
    glGenTextures(1, &T_fb[i]);
    glBindTexture(GL_TEXTURE_2D, T_fb[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // bind texture to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, T_fb[i], 0);
    // render buffer object
    glGenRenderbuffers(1, &rbo[i]);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo[i]);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, windowWidth, windowHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo[i]);
    // check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete! " << i << "/8" << std::endl;
    
    // load mesh
    screen.load(&vertices_screen[0], sizeof(vertices_screen) / sizeof(float),std::string("../shader/fb.vs"), std::string("../shader/glsl_mix_update.fs"), 0);
    // fb screen right
    //ref_plane.load(&vertices[0], sizeof(vertices) / sizeof(float),std::string("../shader/fb.vs"), std::string("../shader/fb.fs"), 0);
    glm::mat4 T = glm::mat4(1.0);
    T=glm::translate(T,glm::vec3(0.5f,0,0));
    T=glm::scale(T,glm::vec3(0.5f,1,1));
    //ref_plane.setTransform(T);
    // fb screen left
    noisy_plane.load(&vertices[0], sizeof(vertices) / sizeof(float),std::string("../shader/fb.vs"), std::string("../shader/fb_noise.fs"), 0);
    glm::mat4 T2 = glm::mat4(1.0);
    T2=glm::translate(T2,glm::vec3(-0.0f,0,0));
    T2=glm::scale(T2,glm::vec3(0.5f,1,1));
    noisy_plane.setTransform(T2);
  }
  // image loading
  for (int i = 0; i < 8; i++)
  {
    // loading reference textures
    T_ref[i].load(ImageDatabasePath+"/p3d_crown-0"+std::to_string(i+1)+"/p3d_crown-0"+std::to_string(i+1)+"_"+leadingZeros(refValue,5)+".png");
  }
  loadNoise(1);

  noisy_plane.shader.setVec2("noisePos",idToVec2(11));

  // unbind framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

static void draw(SDL_Window *window)
{
  // render each framebuffer fb to its corresponding T_fb
  for (int i = 0; i < 8; i++)
    {
      // activate frame buffer [i]
      //--------------------
      glBindFramebuffer(GL_FRAMEBUFFER, fb[i]);
      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      glEnable(GL_DEPTH_TEST);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // draw ref_plane
      //--------------------
      //ref_plane.texture=T_ref[i].ID;
      //ref_plane.draw();

      // draw noisy_plane
      //--------------------
      // set texture and noise texture and draw
      noisy_plane.shader.use();
      //set reference texture
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, T_ref[i].ID);
      noisy_plane.shader.setInt("screenTexture",0);
      //set noise texture
      glActiveTexture(GL_TEXTURE0+1);
      glBindTexture(GL_TEXTURE_2D, T_noise[i].ID);
      noisy_plane.shader.setInt("screenNoiseTexture",1);  
      // draw
      glBindVertexArray(noisy_plane.VAO);
      glDrawArrays(GL_TRIANGLES, 0, noisy_plane.vertexCount);
      glBindVertexArray(0);
      glUseProgram(0);
    }

    // activate frame buffer 0: draw to screen
    //------------------------------------------
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // send fb_textures to alioscopy shader, then render to screen
    screen.shader.use();
    for (unsigned int i = 0; i < 8; i++)
    { 
      glActiveTexture(GL_TEXTURE0+i);
      glBindTexture(GL_TEXTURE_2D, T_fb[i]);
      screen.shader.setInt("srcTextures["+std::to_string(i)+"]",i);  
    }
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, T_fb[0]);
    glBindVertexArray(screen.VAO);
    glDrawArrays(GL_TRIANGLES, 0, screen.vertexCount);
    glBindVertexArray(0);
    glUseProgram(0);
}

int main(int argc, char *argv[])
{
  if (!init())
  {
    printf("failed\n");
  }

  glViewport(0, 0, windowWidth, windowHeight);

  sceneSetup();
  experimentTimer.reset();
  patchUpdateTimer.reset();
  while (1)
  {
    event();
    
    if (patchUpdateTimer.elapsed() > patchUpdateFrequency )
    {
      patchUpdateTimer.reset();
      noiseValue=(rand()%refValue)+1;
      loadNoise(noiseValue);
      patchPos=(rand()%16)+1;
      noisy_plane.shader.use();
      noisy_plane.shader.setVec2("noisePos",idToVec2(patchPos));
      
      std::cout << "MOVE in position" << patchPos << " NOISE VALUE = "<< noiseValue << std::endl;
    }

    if(bUserDetect)
    {
      std::cout<< float(mousePositionX)/float(windowWidth) <<" ; "<< float(mousePositionY)/float(windowHeight) <<std::endl;
    }
    draw(window);
    SDL_GL_SwapWindow(window);
  }

  // framebuffer delete
  for (int i = 0; i < 8; i++)
  {
    glDeleteFramebuffers(1, &fb[i]);
  }
  return 0;
}