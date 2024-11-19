#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    // TexCoords = aPos;
    TexCoords = vec3(aPos.x, 1.0 - aPos.y, aPos.z); // Flip the Y coordinate
    gl_Position = projection * view * vec4(aPos, 1.0);
}  