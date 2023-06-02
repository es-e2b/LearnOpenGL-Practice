#version 330 core
out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord; // 추가

uniform Material material;
uniform Light light;
uniform vec3 eyePos;
uniform sampler2D texture1; // 추가: 텍스처 유니폼 선언

void main()
{
    // 텍스처 샘플링
    vec3 texColor = texture(texture1, TexCoord).rgb;

    // diffuse term
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse * texColor);

    // specular term
    vec3 View = normalize(eyePos - FragPos);
    vec3 refl = 2.0 * norm * dot(norm, lightDir) - lightDir;
    float spec = pow(max(dot(refl, View), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);

    // ambient term
    vec3 ambient = light.ambient * material.ambient * texColor;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
