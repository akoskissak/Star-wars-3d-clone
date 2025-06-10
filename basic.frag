#version 330 core
out vec4 FragColor;
uniform vec3 objectColor;

void main() {
    if (gl_FrontFacing)
        FragColor = vec4(objectColor, 1.0);
    else
        FragColor = vec4(0.8, 0.2, 1.0, 1.0);
}

