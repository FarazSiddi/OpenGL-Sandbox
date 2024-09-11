#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;  // Add this to get the fragment position

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;
uniform sampler2D texture4;
uniform sampler2D texture5;

void main()
{
    // Define a threshold for blending textures based on the y-coordinate
    float grassBoundary = -2.1; // Adjust this boundary based on your object's scale and shape
    float treeBoundary = -1.999; // Adjust this boundary based on your object's scale and shape
    float leafBoundary = -1.0; // Adjust this boundary based on your object's scale and shape
    float snowBoundary = 3.0; // Adjust this boundary based on your object's scale and shape

    // If the y-coordinate is less than the boundary, use texture1 (bottom part), else use texture2 (top part)
    if (FragPos.y < grassBoundary)
    {
        FragColor = texture(texture1, TexCoord);
    }
    else if (FragPos.y < treeBoundary)
    {
		FragColor = texture(texture2, TexCoord);
	}
    else if (FragPos.y < leafBoundary)
    {
		FragColor = texture(texture3, TexCoord);
	}
    else if (FragPos.y < snowBoundary)
    {
        FragColor = texture(texture4, TexCoord);
    } 
    else
	{
		FragColor = texture(texture5, TexCoord);
	}
}