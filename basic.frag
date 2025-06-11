#version 330 core
out vec4 FragColor;
uniform vec3 objectColor;

in vec2 TexCoord;

uniform sampler2D texture1;
uniform bool useTexture;

void main() {
    vec4 color = vec4(objectColor, 1.0);
    if (useTexture)
        color = texture(texture1, TexCoord);

    if (gl_FrontFacing)
        FragColor = color;
    else
        FragColor = vec4(0.8, 0.2, 1.0, 1.0);
}
