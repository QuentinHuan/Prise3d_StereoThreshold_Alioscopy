#include "mesh.h"
#include "openvr.h"
//--------------------------------------------------------
/*
class for a static mesh that is displayed differently for left and right eye

desc:
spawn a mesh wherever you want in the scene and render it on screen
*/
//--------------------------------------------------------
class StereoMesh : Mesh
{
public:
    Shader shaderR;
    Shader shaderL;

    unsigned int textureL;
    unsigned int textureR;
    unsigned int noiseTextureL;
    unsigned int noiseTextureR;

    bool revertEye = 1;


    //--------------------------------------------------------
    //                          constructors
    //--------------------------------------------------------
    StereoMesh()
    {
    }
    StereoMesh(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) : Mesh(position, rotation, scale)
    {
    }

    StereoMesh(glm::vec3 position) : Mesh(position)
    {
    }
    //--------------------------------------------------------
    //              mesh loading from vertex array
    //--------------------------------------------------------
    void load(float *vertices, GLsizei count, Shader shaderL, Shader shaderR, unsigned int textureL, unsigned int textureR)
    {
        vertexCount = count;
        this->textureR = textureR;
        this->textureL = textureL;
        this->shaderR = shaderR;
        this->shaderL = shaderL;
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(float), vertices, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(0 * sizeof(float)));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glBindVertexArray(0);
        setupShader();
    }
    //--------------------------------------------------------
    //                    shader setup
    //--------------------------------------------------------
    void setupShader()
    {
        //geometry
        glm::mat4 T = glm::mat4(1.0f);
        T = glm::scale(T, scale);
        T = glm::translate(T, position);
        //yaw pitch roll
        T = glm::rotate(T, rotation.x, glm::vec3(0, 1, 0));
        T = glm::rotate(T, rotation.y, glm::vec3(1, 0, 0));
        T = glm::rotate(T, rotation.z, glm::vec3(0, 0, 1));

        shaderR.use();
        shaderR.setMat4("transform", T);
        //shaderR.setTexture("mytexture", 0, textureR);
        shaderL.use();
        shaderL.setMat4("transform", T);
        //shaderR.setTexture("mytexture", 1, textureL);
    }

    void setTransform(glm::mat4 T)
    {
        shaderR.use();
        shaderR.setMat4("transform", T);
        shaderL.use();
        shaderL.setMat4("transform", T);
    }

    //##################################
    // update noise block texture and position
    //##################################
    void updateNoiseBlock(unsigned int noiseTextureL, unsigned int noiseTextureR,glm::vec2 coord)
    {
        this->noiseTextureR = noiseTextureR;
        this->noiseTextureL = noiseTextureL;

        shaderR.use();
        shaderR.setVec2("noiseBlockPos",coord);

        shaderL.use();
        shaderL.setVec2("noiseBlockPos",coord);
    }
    //--------------------------------------------------------
    //                    draw fonction
    //--------------------------------------------------------
    //default : use this->shader
    void draw(vr::Hmd_Eye nEye)
    {
        if ((~revertEye)&(nEye == vr::Eye_Right))
        {
            shaderR.use();
            shaderR.setTexture("noisyTexture", 0, textureR);
            shaderR.setTexture("noisyTexture", 1, noiseTextureR);
            //glBindTexture(GL_TEXTURE_2D, textureR);
        }
        else
        {
            shaderL.use();
            //glBindTexture(GL_TEXTURE_2D, textureL);
            shaderL.setTexture("noisyTexture", 0, textureL);
            shaderL.setTexture("noisyTexture", 1, noiseTextureL);
        }

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
        glBindVertexArray(0);
        glUseProgram(0);
    }
};