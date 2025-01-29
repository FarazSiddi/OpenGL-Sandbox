#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cmath>      // for sin, cos, M_PI
#include "BaseShape.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class Sphere : public BaseShape
{
public:
    Sphere(int slices = 16, int stacks = 16)
        : VAO(0), VBO(0), EBO(0),
        m_Slices(slices),
        m_Stacks(stacks)
    {
        // Default transform
        scale = glm::vec3(1.0f);
        position = glm::vec3(0.0f);
        rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
        rotationAngle = 0.0f;
    }

    virtual ~Sphere()
    {
        // Cleanup GPU resources
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    virtual void init() override
    {
        // 1. Generate vertices & indices
        generateSphereData();

        // 2. Create VAO
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        // 3. Create VBO (store vertex positions)
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER,
            sizeof(float) * m_Vertices.size(),
            m_Vertices.data(),
            GL_STATIC_DRAW);

        // 4. Create EBO (store indices)
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            sizeof(unsigned int) * m_Indices.size(),
            m_Indices.data(),
            GL_STATIC_DRAW);

        // 5. Set up vertex attributes
        // layout (location = 0): 3 floats for position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // 6. Unbind VAO (optional safety)
        glBindVertexArray(0);
    }

    // Called each frame to draw the sphere
    virtual void draw(const glm::mat4& view,
        const glm::mat4& projection,
        unsigned int shaderID) override
    {
        glUseProgram(shaderID);

        // Build the model matrix from the shape's transforms
        glm::mat4 model(1.0f);
        model = glm::translate(model, position);
        if (rotationAngle != 0.0f)
            model = glm::rotate(model, glm::radians(rotationAngle), rotationAxis);
        model = glm::scale(model, scale);

        // Pass uniforms to the shader
        GLint locModel = glGetUniformLocation(shaderID, "model");
        GLint locView = glGetUniformLocation(shaderID, "view");
        GLint locProj = glGetUniformLocation(shaderID, "projection");
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniformMatrix4fv(locView, 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(locProj, 1, GL_FALSE, &projection[0][0]);

        // Bind VAO and draw
        glBindVertexArray(VAO);

        // Use glDrawElements because we have an EBO
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_Indices.size()), GL_UNSIGNED_INT, 0);

        // Unbind VAO (optional)
        glBindVertexArray(0);
    }

private:
    unsigned int VAO, VBO, EBO;
    int m_Slices;
    int m_Stacks;

    // Vertex buffer and index buffer
    // Each vertex is just (x,y,z) for now.
    std::vector<float>        m_Vertices;
    std::vector<unsigned int> m_Indices;

    // Generate a UV-sphere around the origin with a radius of 0.5f
    void generateSphereData()
    {
        float radius = 0.5f;

        // Create vertices
        for (int stack = 0; stack <= m_Stacks; ++stack)
        {
            // phi: angle from top (0) to bottom (PI)
            float phi = static_cast<float>(M_PI) * (float)stack / (float)m_Stacks;

            for (int slice = 0; slice <= m_Slices; ++slice)
            {
                // theta: angle around the equator (0 to 2PI)
                float theta = 2.0f * static_cast<float>(M_PI) * (float)slice / (float)m_Slices;

                // spherical -> Cartesian
                float x = radius * sinf(phi) * cosf(theta);
                float y = radius * cosf(phi);
                float z = radius * sinf(phi) * sinf(theta);

                // push back positions (x,y,z)
                m_Vertices.push_back(x);
                m_Vertices.push_back(y);
                m_Vertices.push_back(z);

                // If you need normals, you'd push them here as well
                // If you need texture coordinates, you'd also push them, etc.
            }
        }

        // Create indices
        // Each "stack" connects to the next stack with quads (2 triangles)
        for (int stack = 0; stack < m_Stacks; ++stack)
        {
            for (int slice = 0; slice < m_Slices; ++slice)
            {
                int first = (stack * (m_Slices + 1)) + slice;
                int second = ((stack + 1) * (m_Slices + 1)) + slice;

                // two triangles per quad
                m_Indices.push_back(first);
                m_Indices.push_back(second);
                m_Indices.push_back(first + 1);

                m_Indices.push_back(second);
                m_Indices.push_back(second + 1);
                m_Indices.push_back(first + 1);
            }
        }
    }
};
