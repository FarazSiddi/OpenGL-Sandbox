#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

class BaseShape
{
public:
    // Each shape will store its own transform
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 rotationAxis;
    float rotationAngle;

    BaseShape()
        : position(0.0f), scale(1.0f), rotationAxis(0.0f, 1.0f, 0.0f), rotationAngle(0.0f)
    {
    }

    virtual ~BaseShape() {}

    // Called once: sets up VAO/VBO, etc.
    virtual void init() = 0;

    // Called each frame to render
    virtual void draw(const glm::mat4& view, const glm::mat4& projection, unsigned int shaderID) = 0;
};
