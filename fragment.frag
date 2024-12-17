#version 330 core
out vec4 FragColor;
out vec4 baseplateColor;

in vec2 TexCoord;

// texture sampler
uniform sampler2D texture1;

uniform vec3 baseplateColor1;

void main()
{
    FragColor = texture(texture1, TexCoord);
    baseplateColor = vec4(baseplateColor1, 1.0); // Use the baseplate color
}