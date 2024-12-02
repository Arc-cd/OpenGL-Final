#version 430 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform int flag;

void main()
{             
    // retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;
    FragPos = normalize(FragPos) * 0.5 + 0.5;
    Normal = normalize(Normal) * 0.5 + 0.5;

    if (flag == 1) {
        FragColor = vec4(FragPos, 1.0);
    } else if (flag == 2) {
        FragColor = vec4(Normal, 1.0);
    } else if (flag == 3) {
        FragColor = vec4(Diffuse, 1.0);
    }
}