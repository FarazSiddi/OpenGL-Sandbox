#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "BaseShape.h"

class Cube : public BaseShape
{
public:
    // The VAO and VBO to handle the GPU geometry
    unsigned int VAO, VBO;

    // Each face of the cube is 2 triangles (6 vertices total).
    // There are 6 faces, so 36 vertices. Each vertex is 3 floats (x, y, z).
    // This is a standard 1󪻑 cube centered at the origin.
    static constexpr float vertices[36 * 3] = {
        // Back
        -0.5f, -0.5f, -0.5f,   0.5f, -0.5f, -0.5f,   0.5f,  0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,  -0.5f,  0.5f, -0.5f,  -0.5f, -0.5f, -0.5f,

        // Front
        -0.5f, -0.5f,  0.5f,   0.5f, -0.5f,  0.5f,   0.5f,  0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,  -0.5f,  0.5f,  0.5f,  -0.5f, -0.5f,  0.5f,

        // Left
        -0.5f,  0.5f,  0.5f,  -0.5f,  0.5f, -0.5f,  -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,  -0.5f, -0.5f,  0.5f,  -0.5f,  0.5f,  0.5f,

        // Right
        0.5f,  0.5f,  0.5f,   0.5f,  0.5f, -0.5f,   0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,   0.5f, -0.5f,  0.5f,   0.5f,  0.5f,  0.5f,

        // Bottom
        -0.5f, -0.5f, -0.5f,   0.5f, -0.5f, -0.5f,   0.5f, -0.5f,  0.5f,
        0.5f, -0.5f,  0.5f,  -0.5f, -0.5f,  0.5f,  -0.5f, -0.5f, -0.5f,

        // Top
        -0.5f,  0.5f, -0.5f,   0.5f,  0.5f, -0.5f,   0.5f,  0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,  -0.5f,  0.5f,  0.5f,  -0.5f,  0.5f, -0.5f
    };

    Cube()
        : VAO(0), VBO(0)
    {
        // Optional: Set default transforms for the shape
        scale = glm::vec3(1.0f);
        position = glm::vec3(0.0f);
        rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
        rotationAngle = 0.0f;
    }

    virtual ~Cube()
    {
        // Cleanup GPU resources
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }

    // Called once after constructing, to set up buffers
    virtual void init() override
    {
        // Generate VAO/VBO
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        // Bind VAO
        glBindVertexArray(VAO);

        // Bind + Fill VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // Position attribute (layout = 0)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Unbind for safety
        glBindVertexArray(0);
    }

    // Called every frame to draw
    virtual void draw(const glm::mat4& view, const glm::mat4& projection, unsigned int shaderID) override
    {
        // Activate the shader
        glUseProgram(shaderID);

        // Create model matrix from our position/rotation/scale
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::rotate(model, glm::radians(rotationAngle), rotationAxis);
        model = glm::scale(model, scale);

        // Pass uniforms to the shader
        unsigned int locModel = glGetUniformLocation(shaderID, "model");
        unsigned int locView = glGetUniformLocation(shaderID, "view");
        unsigned int locProj = glGetUniformLocation(shaderID, "projection");

        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniformMatrix4fv(locView, 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(locProj, 1, GL_FALSE, &projection[0][0]);

        // Bind VAO and render
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }
};

