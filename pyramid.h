#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "BaseShape.h"

class Pyramid : public BaseShape
{
public:
    // GPU buffers
    unsigned int VAO = 0;
    unsigned int VBO = 0;

    // A simple square-based pyramid, centered at the origin on the XZ-plane,
    // base Y=0, apex at Y=1. We'll define it as 18 vertices (4 triangular sides + 2 triangles for the base).
    // Each face is 3 floats (x,y,z) * 3 vertices = 9 floats, times 6 faces = 54 floats total.
    static constexpr float vertices[54] = {
        // Side face 1 (front) 
        0.0f, 1.0f, 0.0f,    -0.5f, 0.0f,  0.5f,    0.5f, 0.0f,  0.5f,

        // Side face 2 (right) 
        0.0f, 1.0f, 0.0f,     0.5f, 0.0f,  0.5f,    0.5f, 0.0f, -0.5f,

        // Side face 3 (back) 
        0.0f, 1.0f, 0.0f,     0.5f, 0.0f, -0.5f,   -0.5f, 0.0f, -0.5f,

        // Side face 4 (left) 
        0.0f, 1.0f, 0.0f,    -0.5f, 0.0f, -0.5f,   -0.5f, 0.0f,  0.5f,

        // Base triangle 1 
        -0.5f, 0.0f,  0.5f,   0.5f, 0.0f,  0.5f,    0.5f, 0.0f, -0.5f,

        // Base triangle 2 
         0.5f, 0.0f, -0.5f,  -0.5f, 0.0f, -0.5f,   -0.5f, 0.0f,  0.5f
    };

    Pyramid()
    {
        // Default transform
        scale = glm::vec3(1.0f);
        position = glm::vec3(0.0f);
        rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
        rotationAngle = 0.0f;
    }

    virtual ~Pyramid()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }

    // Sets up the VAO/VBO with the above vertex data
    virtual void init() override
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // layout(location = 0) => position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
    }

    // Draws the pyramid
    virtual void draw(const glm::mat4& view,
        const glm::mat4& projection,
        unsigned int shaderID) override
    {
        // Use the shader
        glUseProgram(shaderID);

        // Build the model matrix from position/rotation/scale
        glm::mat4 model(1.0f);
        model = glm::translate(model, position);
        if (rotationAngle != 0.0f)
            model = glm::rotate(model, glm::radians(rotationAngle), rotationAxis);
        model = glm::scale(model, scale);

        // Pass uniforms (assuming your shader has "model", "view", "projection")
        GLint locModel = glGetUniformLocation(shaderID, "model");
        GLint locView = glGetUniformLocation(shaderID, "view");
        GLint locProj = glGetUniformLocation(shaderID, "projection");

        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniformMatrix4fv(locView, 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(locProj, 1, GL_FALSE, &projection[0][0]);

        // Bind and draw the pyramid
        glBindVertexArray(VAO);

        // 18 vertices total
        glDrawArrays(GL_TRIANGLES, 0, 18);

        glBindVertexArray(0);
    }
};
#pragma once
