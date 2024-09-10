#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;  // Add this to get the fragment position

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
    // Define a threshold for blending textures based on the y-coordinate
    float boundary = -2.1; // Adjust this boundary based on your object's scale and shape

    // If the y-coordinate is less than the boundary, use texture1 (bottom part), else use texture2 (top part)
    if (FragPos.y < boundary)
    {
        FragColor = texture(texture1, TexCoord);
    }
    else
    {
        FragColor = texture(texture2, TexCoord);
    }
}