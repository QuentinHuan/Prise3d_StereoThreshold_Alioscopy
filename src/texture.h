#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include "lodepng.h"

class Texture
{
public:
    std::string path;
    GLuint ID = 0;

    Texture()
    {
    }

    void load(std::string path)
    {

        //std::cout << "loading texture  " << path << std::endl;
        glGenTextures(1, &ID);
        glBindTexture(GL_TEXTURE_2D, ID);

        // load and generate the texture
        unsigned width, height, nrChannels;
        std::vector<unsigned char> data;

        unsigned nError = lodepng::decode(data, width, height, path);

        if (nError != 0)
        {
            std::cout << "Failed to load texture" << std::endl;
        }
        else
        {
            //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);

            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

            GLfloat fLargest;
            glGetFloatv(GL_TEXTURE_MAX_ANISOTROPY, &fLargest);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, fLargest);

            glBindTexture(GL_TEXTURE_2D, 0);
            std::cout << "loading texture  " << path << " DONE" << std::endl;
        }
    }

    //load texture from previously stored bitmap data
    void loadFrom(std::vector<unsigned char> data, unsigned width, unsigned height)
    {
        //std::cout << "loading texture  " << path << std::endl;
        glGenTextures(1, &ID);
        glBindTexture(GL_TEXTURE_2D, ID);

        if (data.size() == 0)
        {
            std::cout << "invalid data : size = 0" << std::endl;
        }
        else
        {
            //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

            //texture upload
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);

            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

            GLfloat fLargest;
            glGetFloatv(GL_TEXTURE_MAX_ANISOTROPY, &fLargest);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, fLargest);

            glBindTexture(GL_TEXTURE_2D, 0);
            //std::cout << "loading texture  " << path << " DONE" << std::endl;
        }
    }

    void loadFromSDL_Surface(SDL_Surface *Surface)
    {
        glGenTextures(1, &ID);
        glBindTexture(GL_TEXTURE_2D, ID);



        /* int w = pow(2, ceil(log(Surface->w) / log(2))); // Round up to the nearest power of two
        int h = pow(2, ceil(log(Surface->h) / log(2))); // Round up to the nearest power of two */

        int w = 1920;
        int h = 1080;
        SDL_Surface *newSurface =
            SDL_CreateRGBSurface(0, w, h, 24, 0x000000ff, 0x00ff0000, 0x0000ff00, 0xff000000);
        SDL_BlitSurface(Surface, 0, newSurface, 0); // Blit onto a purely RGB Surface

        int Mode = GL_RGB;
        if (newSurface->format->BytesPerPixel == 4)
        {
            printf("alpha\n");
            Mode = GL_RGBA;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, Mode, newSurface->w, newSurface->h, 0, Mode, GL_UNSIGNED_BYTE, newSurface->pixels);

        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);
    }
};