#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cmath>
#include "BaseShape.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class Cylinder : public BaseShape
{
public:
    Cylinder(int slices = 16, float radius = 0.5f, float height = 1.0f)
        : m_Slices(slices), m_Radius(radius), m_Height(height),
        VAO(0), VBO(0), EBO(0)
    {
        // Default transform
        scale = glm::vec3(1.0f);
        position = glm::vec3(0.0f);
        rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
        rotationAngle = 0.0f;
    }

    virtual ~Cylinder()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    // Generate the vertex data & set up the GPU buffers
    virtual void init() override
    {
        generateCylinderData();

        // Create VAO
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        // Create VBO (positions)
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER,
            m_Vertices.size() * sizeof(float),
            m_Vertices.data(),
            GL_STATIC_DRAW);

        // Create EBO (indices)
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            m_Indices.size() * sizeof(unsigned int),
            m_Indices.data(),
            GL_STATIC_DRAW);

        // layout(location=0) => 3 floats for position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Unbind for safety
        glBindVertexArray(0);
    }

    // Render the cylinder
    virtual void draw(const glm::mat4& view,
        const glm::mat4& projection,
        unsigned int shaderID) override
    {
        glUseProgram(shaderID);

        // Build the model matrix from transforms
        glm::mat4 model(1.0f);
        model = glm::translate(model, position);
        if (rotationAngle != 0.0f)
            model = glm::rotate(model, glm::radians(rotationAngle), rotationAxis);
        model = glm::scale(model, scale);

        // Pass matrices to the shader
        GLint locModel = glGetUniformLocation(shaderID, "model");
        GLint locView = glGetUniformLocation(shaderID, "view");
        GLint locProj = glGetUniformLocation(shaderID, "projection");
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniformMatrix4fv(locView, 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(locProj, 1, GL_FALSE, &projection[0][0]);

        // Draw the cylinder
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_Indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

private:
    // User-defined parameters
    int   m_Slices;       // how many subdivisions around the circle
    float m_Radius;       // radius of the cylinder
    float m_Height;       // height of the cylinder

    unsigned int VAO, VBO, EBO;

    // For storing geometry
    std::vector<float>        m_Vertices;  // (x,y,z) for each vertex
    std::vector<unsigned int> m_Indices;

    // Build top disk, bottom disk, and side faces
    void generateCylinderData()
    {
        // We'll place the center of the cylinder at Y=0,
        // so the top circle is at y = +m_Height/2,
        // and the bottom circle is at y = -m_Height/2.
        float halfHeight = m_Height * 0.5f;

        // Generate top circle & bottom circle
        // We'll create (m_Slices+1) points for top (with center),
        // and (m_Slices+1) points for bottom (with center).
        // Then we'll create side faces by connecting top ring to bottom ring.

        // Indices where top vertices start in m_Vertices
        int topCenterIndex = 0; // after we push, we'll know the actual index
        int bottomCenterIndex = 0;

        // We'll push all the top vertices first, then all the bottom vertices
        // NOTE: We'll store in the order:
        //   - top center
        //   - top ring (m_Slices points)
        //   - bottom center
        //   - bottom ring (m_Slices points)

        // Push top center
        float topCenterY = +halfHeight;
        topCenterIndex = 0; // once we push, it's index 0
        m_Vertices.push_back(0.0f);      // x
        m_Vertices.push_back(topCenterY); // y
        m_Vertices.push_back(0.0f);      // z

        // Push top ring
        // We'll have m_Slices points around a circle of radius m_Radius
        for (int i = 0; i < m_Slices; i++)
        {
            float theta = 2.0f * static_cast<float>(M_PI) * (float)i / (float)m_Slices;
            float x = m_Radius * cosf(theta);
            float z = m_Radius * sinf(theta);

            m_Vertices.push_back(x);
            m_Vertices.push_back(topCenterY);
            m_Vertices.push_back(z);
        }

        // Push bottom center
        float bottomCenterY = -halfHeight;
        // This index is topCenterIndex + (m_Slices+1)
        bottomCenterIndex = 1 + (m_Slices - 1) + 1; // We'll calculate properly below
        // Actually, let's do it systematically:
        bottomCenterIndex = static_cast<int>(m_Vertices.size() / 3);
        m_Vertices.push_back(0.0f);
        m_Vertices.push_back(bottomCenterY);
        m_Vertices.push_back(0.0f);

        // Push bottom ring
        for (int i = 0; i < m_Slices; i++)
        {
            float theta = 2.0f * static_cast<float>(M_PI) * (float)i / (float)m_Slices;
            float x = m_Radius * cosf(theta);
            float z = m_Radius * sinf(theta);

            m_Vertices.push_back(x);
            m_Vertices.push_back(bottomCenterY);
            m_Vertices.push_back(z);
        }

        // Let's define some helper indices
        // top center is index 0
        // top ring starts at index 1 ... 1+(m_Slices-1)
        int topRingStart = topCenterIndex + 1; // = 1
        int bottomCenterIdx = bottomCenterIndex;  // (we computed above)
        int bottomRingStart = bottomCenterIdx + 1;
        // The ring has m_Slices points each.

        // Build indices for top disk (fan):
        // We'll create m_Slices triangles connecting the top center to each pair of adjacent ring vertices
        for (int i = 0; i < m_Slices; i++)
        {
            int current = topRingStart + i;
            int next = topRingStart + ((i + 1) % m_Slices); // wrap around

            m_Indices.push_back(topCenterIndex);
            m_Indices.push_back(current);
            m_Indices.push_back(next);
        }

        // Build indices for bottom disk (fan):
        // We'll do similarly with bottom center
        int bottomCenter = bottomCenterIdx;
        for (int i = 0; i < m_Slices; i++)
        {
            int current = bottomRingStart + i;
            int next = bottomRingStart + ((i + 1) % m_Slices);

            // Note we want the winding order so it faces downward
            // or consistent with the rest of your scene (maybe reversed).
            // We'll keep the same order for consistency. 
            m_Indices.push_back(bottomCenter);
            m_Indices.push_back(next);
            m_Indices.push_back(current);
        }

        // Build indices for side faces
        // The side is formed by connecting each top ring vertex to the corresponding bottom ring vertex
        // We'll do it in quads, each with 2 triangles
        for (int i = 0; i < m_Slices; i++)
        {
            int topCurrent = topRingStart + i;
            int topNext = topRingStart + ((i + 1) % m_Slices);
            int bottomCurrent = bottomRingStart + i;
            int bottomNext = bottomRingStart + ((i + 1) % m_Slices);

            // Triangle 1 of the quad
            m_Indices.push_back(topCurrent);
            m_Indices.push_back(bottomCurrent);
            m_Indices.push_back(topNext);

            // Triangle 2 of the quad
            m_Indices.push_back(topNext);
            m_Indices.push_back(bottomCurrent);
            m_Indices.push_back(bottomNext);
        }
    }
};
