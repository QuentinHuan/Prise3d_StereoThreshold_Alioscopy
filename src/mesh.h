#include <glad/glad.h>
#include <SDL2/SDL.h>
#include "shader.h"

//--------------------------------------------------------
/*
class for a basic static mesh

desc:
spawn a mesh wherever you want in the scene and render it on screen
*/
//--------------------------------------------------------
class Mesh
{
public:
    unsigned int ID;
    Shader shader;
    unsigned int VBO;
    unsigned int VAO;
    unsigned int texture;
    GLsizei vertexCount;

    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    //--------------------------------------------------------
    //                          constructors
    //--------------------------------------------------------
    Mesh()
    {
        position = glm::vec3(0.0f, 0.0f, 0.0f);
        rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        scale = glm::vec3(1.0f, 1.0f, 1.0f);
        vertexCount = 0;
    }
    Mesh(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) : position(position), rotation(rotation), scale(scale)
    {
        vertexCount = 0;
    }

    Mesh(glm::vec3 position) : position(position)
    {
        rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        scale = glm::vec3(1.0f, 1.0f, 1.0f);
        vertexCount = 0;
    }
    //--------------------------------------------------------
    //              mesh loading from vertex array
    //--------------------------------------------------------
    void load(float *vertices, GLsizei count, Shader shader, unsigned int texture)
    {
        vertexCount = count;
        this->texture = texture;
        this->shader = shader;
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
        shader.use();
        //texture
        glBindTexture(GL_TEXTURE_2D, texture);
        //shader.setInt("screenTexture", texture);
        //geometry
        glm::mat4 T = glm::mat4(1.0f);
        T = glm::scale(T, scale);
        T = glm::translate(T, position);
        //yaw pitch roll
        T = glm::rotate(T, rotation.x, glm::vec3(0, 1, 0));
        T = glm::rotate(T, rotation.y, glm::vec3(1, 0, 0));
        T = glm::rotate(T, rotation.z, glm::vec3(0, 0, 1));

        shader.setMat4("transform", T);
    }

    void setTransform(glm::mat4 T)
    {
        shader.use();
        shader.setMat4("transform", T);
    }

    //--------------------------------------------------------
    //                    draw fonction
    //--------------------------------------------------------
    //request specific shader
    void draw(Shader shader)
    {
        shader.use();

        glBindVertexArray(VAO);
        glBindTexture(GL_TEXTURE_2D, texture);
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
        glBindVertexArray(0);
        glUseProgram(0);
    }
    //default : use this->shader
    void draw()
    {
        shader.use();
        
        glBindVertexArray(VAO);
        glBindTexture(GL_TEXTURE_2D, texture);
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
        glBindVertexArray(0);
        glUseProgram(0);
    }
};