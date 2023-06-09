#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord; // 추가

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = normalize(transpose(inverse(mat3(model))) * aNormal);
    TexCoord = aTexCoord; // 추가
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
