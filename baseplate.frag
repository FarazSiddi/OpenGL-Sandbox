#version 330 core
in vec3 FragPos;
out vec4 FragColor;

uniform vec3 baseplateColor;

void main()
{
    // Render a solid color
    FragColor = vec4(baseplateColor, 1.0);
}
