//-----------------------------------------------------
/*
  Quentin Huan, 06/2021

  Prise3D stereo thresholds experiment : 
  works on alioscopy autostereoscopic screen

    uses: SDL2, glad, OpenGL
    additional : lodepng lib in  include folder
    glm math lib
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

float vertices_noisy_plane[] = {
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
    1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f};

// main buffer plane
Mesh screen;
Shader alioscopy_Shader;

// instructions
Mesh instruction_plane;

// framebuffers
unsigned int fb[8]; // framebuffers
unsigned int T_fb[8]; // rendering textures
unsigned int rbo[8]; // render buffer objects

// 8 pov buffer plane 
Mesh noisy_plane;
Texture T_ref[8]; // ref 8pov images
Texture T_noise[8]; // noisy 8pov images
Texture T_instruction;

// internal state
Timer experimentTimer;
Timer tutorialTimer;
Timer patchUpdateTimer;
Timer aquisitionTimer;
std::vector<std::string> sceneList; // scenelist loaded from config/scenes.ini
std::vector<int> stimulusSet; // stimulus for each block, computed by script/ComputeNewStimulusSet.py when the scene is loaded
int sceneID=-1; // 0 is tutorial
float aquisitionTime=0.01f; // save data to log every x seconds
int patchPos=0; // patch position in image block ID ([1,16]) ; -1 is outside of the plane
int noiseSPP=1; // image ID
bool bInExperiment = false;
// parameters ==> change them in config/config.ini
std::string ImageDatabasePath;
int oneSceneDuration=15; // in sec
int patchUpdateFrequency=5; // in sec
int refSPP=1; // image ID

int patchPosList[] = {13,2,3,16,5,9,14,8,6,12,11,10,1,7,15,4};
int patchLoop=0;
//--------------------
// Texture management
//--------------------
// change the noise texture displayed on noisePlane
static void loadNoise(int level)
{
  level = std::min(level,refSPP);
  std::string sceneName = sceneList.at(sceneID);
  for (int i = 0; i < 8; i++)
  {
    std::string folderName = sceneName+std::string("-")+leadingZeros(i+1,2);
    T_noise[i].load(ImageDatabasePath+std::string("/")+folderName+std::string("/")+folderName+std::string("_")+leadingZeros(level,5)+std::string(".png"));
  }
}

// change the reference texture
static void loadRef()
{
  std::string sceneName = sceneList.at(sceneID);
  for (int i = 0; i < 8; i++)
  {
    std::string folderName = sceneName+std::string("-")+leadingZeros(i+1,2);;
    T_ref[i].load(ImageDatabasePath+"/"+folderName+"/"+folderName+"_"+leadingZeros(refSPP,5)+".png");
  }
}

//-----------------
// file management
//-----------------
// parse config/config.ini and set parameters accordingly
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
      {refSPP=stoi(value);}
      if(param=="oneSceneDuration")
      {oneSceneDuration=stoi(value);}
      if(param=="patchUpdateFrequency")
      {patchUpdateFrequency=stoi(value);}
      }
    }
  }
}

// parse config/scenes.ini and put the result in sceneList vector
static bool parseSceneFile()
{
  std::ifstream input( "../config/scenes.ini" );
  for( std::string line; getline( input, line ); )
  {
    if(line.find("#"))// filter out comments
    {
      line.pop_back();
      sceneList.push_back(line);
    }
  }
  if (sceneList.size() >= 1)
  {return true;}
  else
  {return false;}
}

// backup old log/p3d.log file and create a new one
void setupLogFile()
{
  // open previous file, read first line (date) and rename it "date.log"
  std::ifstream input( "../logs/p3d.log" );
  std::string savedDate; getline( input, savedDate );
  std::string path = "../logs/"+savedDate+".log";
  if(rename("../logs/p3d.log", &path[0]) != 0)
  {
    std::cout << "!!! error while saving previous .log file" << std::endl;
  }
  // create new p3d.log file
  std::stringstream date;
  time_t now = time(0);
  char* dt = ctime(&now); date << dt;
  std::ofstream outfile ("../logs/p3d.log");
  outfile << date.str() << std::endl;
  outfile.close();
  return;
}

//-----------------
// change scene
//-----------------
// change the scene:
// save results, select next scene and compute the new stimulus set
static int changeScene()
{
  //saveExperiment()
  // change scene ID and load corresponding images
  sceneID = (sceneID + 1);

  if(sceneID >= sceneList.size())
  {std::cout << "changeScene() return -1" << std::endl;
    return -1;}

  loadRef();
  loadNoise(1);
  std::cout << "Change scene:" << sceneList.at(sceneID)<<std::endl;

  stimulusSet = next_stimulus_MLE(sceneList.at(sceneID));
  std::cout<<"next stimulus set:"<<std::endl;
  for (int i = 0; i < stimulusSet.size(); i++)
  {
    std::cout<<stimulusSet.at(i)<<",";
  }
  std::cout<<std::endl;
  return 1;
}

//-----------------
// setup scene
//-----------------
static void sceneSetup()
{
  // parse config files
  parseConfigFile();
  if(!parseSceneFile())
  {std::cout<<"!!! failed to load scene list"<<std::endl;}

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
    
    // load meshes
    screen.load(&vertices_screen[0], sizeof(vertices_screen) / sizeof(float),std::string("../shader/fb.vs"), std::string("../shader/glsl_mix_update.fs"), 0);
    
    instruction_plane.load(&vertices_noisy_plane[0], sizeof(vertices_noisy_plane) / sizeof(float),std::string("../shader/fb.vs"), std::string("../shader/fb.fs"), 0);
    glm::mat4 T1 = glm::mat4(1.0);
    T1=glm::translate(T1,glm::vec3(-0.0f,0,0));
<<<<<<< HEAD
    T1=glm::scale(T1,glm::vec3(0.5f,0.5,1));
    instruction_plane.setTransform(T1);

    T_instruction.generateText("oui bonjour");


=======
    T1=glm::scale(T1,glm::vec3(1,1,1));
    instruction_plane.setTransform(T1);

>>>>>>> 4b6926db33bc69abfac11475807d7650fd765932
    noisy_plane.load(&vertices_noisy_plane[0], sizeof(vertices_noisy_plane) / sizeof(float),std::string("../shader/fb.vs"), std::string("../shader/fb_noise.fs"), 0);
    glm::mat4 T2 = glm::mat4(1.0);
    T2=glm::translate(T2,glm::vec3(-0.0f,0,0));
    T2=glm::scale(T2,glm::vec3(0.5f,1,1));
    noisy_plane.setTransform(T2);
  }
  // initialise scene
  changeScene();
  // initial noise patch position: 0 is invisible (outside of rendering plane)
  noisy_plane.shader.setVec2("noisePos",idToVec2(0));
  // unbind framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//-----------------
// draw everything
//-----------------
static void draw(SDL_Window *window)
{
  // render each framebuffer fb to its corresponding T_fb
  //------------------------------------------------------
  for (int i = 0; i < 8; i++)
    {
      // activate frame buffer [i]
      //--------------------
      glBindFramebuffer(GL_FRAMEBUFFER, fb[i]);
      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      glEnable(GL_DEPTH_TEST);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // draw 8 pov plane
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

      // mouse cursor rendering
      //------------------------
      // set mousePos and bUserDetect uniforms
      noisy_plane.shader.setInt("bUserDetect",bUserDetect);  
      noisy_plane.shader.setVec2("mousePos",mousePosition);  

      // while in tutorial pulsation mouse cursor when hovering noise patch
      if(sceneList.at(sceneID) == "p3d_tutorial")
      {
        glm::vec2 MouseToPatch_distanceVector = (mousePosition-idToVec2(patchPos));
        float distance = std::max(std::abs(MouseToPatch_distanceVector.x),std::abs(MouseToPatch_distanceVector.y));
        noisy_plane.shader.use();
        if(distance < 0.125f && bUserDetect && mousePosition.x >=0 && mousePosition.x <=1 && mousePosition.y >=0 && mousePosition.y <=1)
        {
          noisy_plane.shader.setFloat("variableMouseRadius",glm::pow(glm::sin(experimentTimer.elapsed()*5),2));
        }
        else
        {
          noisy_plane.shader.setFloat("variableMouseRadius",0.0f);
        }
      }

      if(bInExperiment) // Instructions hidden, draw 8pov images
      {
        // draw noisy_plane
        glBindVertexArray(noisy_plane.VAO);
        glDrawArrays(GL_TRIANGLES, 0, noisy_plane.vertexCount);
        glBindVertexArray(0);
        glUseProgram(0);
      }
      else // show instructions
      {
        // draw instruction plane

        //set instruction_plane texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, T_instruction.ID);
        instruction_plane.shader.use();
        instruction_plane.shader.setInt("screenTexture",0);  

        // draw instruction_plane
        glBindVertexArray(instruction_plane.VAO);
        glDrawArrays(GL_TRIANGLES, 0, instruction_plane.vertexCount);
        glBindVertexArray(0);
        glUseProgram(0);
      }
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
    // draw rendering plane
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, T_fb[0]);
    glBindVertexArray(screen.VAO);
    glDrawArrays(GL_TRIANGLES, 0, screen.vertexCount);
    glBindVertexArray(0);
    glUseProgram(0);
}

//-----------------
//      MAIN
//-----------------
int main(int argc, char *argv[])
{
  // see context.h
  if (!init()){printf("failed\n");}
  glViewport(0, 0, windowWidth, windowHeight);
  srand(10); // random seed
  setupLogFile();
  sceneSetup();
  //timers initialisation
  experimentTimer.reset();
  patchUpdateTimer.reset();
  aquisitionTimer.reset();
  tutorialTimer.reset();

  int run = 0;
  // when experiment is done, loop to main menu
  while(1)
  {
    // instruction menu
    std::string exitMsg="appuyez sur [Esc] pour sauvegarder les resultats";
    std::string launchMsg="appuyez sur [espace] pour lancer";
    std::string addMsg;
    if(run>=1) addMsg = exitMsg;
    else addMsg = launchMsg;
    T_instruction.generateText("Les instructions ici\n et la \n et la"+std::string(30,'\n')+addMsg,windowWidth/2);

    while (1)
    {
      event();
      draw(window);
      if(bSpaceKeyDown && run==0)
      {
        break;
      }
      SDL_GL_SwapWindow(window);
    }

    std::cout << "start experiment" << std::endl;
    bInExperiment=true;
    run++;
    // main experiment loop
    while (1)
    {
      event();

      //-------------------
      //  timers callback
      //-------------------
      // patch update callback
      if (patchUpdateTimer.elapsed() > patchUpdateFrequency )
      {
        patchUpdateTimer.reset();

        patchPos=patchPosList[(patchLoop%16)];
        patchLoop=patchLoop+1; // next patch position

        if(sceneList.at(sceneID) != "p3d_tutorial")// use python script to choose new noise levels
        {
          noiseSPP=stimulusSet.at(patchPos-1);// select corresponding noise value
          //noiseSPP=5; // for test
        }
        else// set noise to 0 spp for tutorial
        {noiseSPP=1;}

        loadNoise(noiseSPP);
        noisy_plane.shader.use();
        noisy_plane.shader.setVec2("noisePos",idToVec2(patchPos));
        std::cout << "MOVE in position [" << patchPos << "] ; NOISE VALUE = "<< noiseSPP << std::endl;
      }
      // change scene callback
      if(experimentTimer.elapsed() > oneSceneDuration)
      {
        experimentTimer.reset();
        patchUpdateTimer.reset();
        patchPos = -1; // hide block
        noisy_plane.shader.use();
        noisy_plane.shader.setVec2("noisePos",idToVec2(patchPos));
        std::cout << "MOVE in position [" << patchPos << "] ; NOISE VALUE = "<< noiseSPP << std::endl;

        // quit when scenes are exhausted
        if(changeScene() == -1)
        {
          break;
        }
      }
      // aquisition callback
      if(aquisitionTimer.elapsed() > aquisitionTime && patchPos > 0)
        {log(1,1,sceneList.at(sceneID),experimentTimer.elapsed(),mousePosition,idToVec2(patchPos),noiseSPP,(int)bUserDetect);}
      
      draw(window);
      SDL_GL_SwapWindow(window);
    }
    std::cout << "end of experiement" << std::endl;
    sceneID=0;
    bInExperiment=false;
  }
  // delete framebuffers 
  for (int i = 0; i < 8; i++)
  {
    glDeleteFramebuffers(1, &fb[i]);
  }
  return 0;
}