#include <glad/glad.h>
#include <SDL2/SDL.h>
#include "shader.h"

class Mesh
{
public:
    unsigned int ID;
    Shader shader;
    unsigned int VBO;
    unsigned int VAO;

    Mesh()
    {
    }

    void load(float *vertices, GLsizei count, Shader shader)
    {
        this->shader = shader;
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, count * sizeof(float), vertices, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);
    }

    void draw()
    {
        shader.use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);
    }
};