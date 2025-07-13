
#type vertex
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;
out vec2 TexCoord;
void main()
{
    gl_Position = vec4(aPos, 0.0, 1.0);
    TexCoord = aTexCoord;
}

#type fragment
#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
uniform sampler2D screenTexture;
uniform vec2 resolution;
uniform float radius;

void main()
{
    vec2 tex_offset = 1.0 / resolution; 
    vec3 result = texture(screenTexture, TexCoord).rgb * 0.227027;
    result += texture(screenTexture, TexCoord + vec2(tex_offset.x, 0.0) * radius).rgb * 0.1945946;
    result += texture(screenTexture, TexCoord - vec2(tex_offset.x, 0.0) * radius).rgb * 0.1945946;
    result += texture(screenTexture, TexCoord + vec2(0.0, tex_offset.y) * radius).rgb * 0.1945946;
    result += texture(screenTexture, TexCoord - vec2(0.0, tex_offset.y) * radius).rgb * 0.1945946;
    FragColor = vec4(result, 1.0);
}