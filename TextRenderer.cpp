#include "TextRenderer.hpp"
#include <GL/glew.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include "stb_easy_font.h"
#include <glm/gtc/type_ptr.hpp>
#include <vector>

static unsigned int compileShader(unsigned int type, const char* path) {
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();
    const char* src = source.c_str();

    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info[512];
        glGetShaderInfoLog(shader, 512, NULL, info);
        std::cerr << "Shader compile error: " << info << std::endl;
    }
    return shader;
}

TextRenderer::TextRenderer() : VAO(0), VBO(0), shaderProgram(0) {}

TextRenderer::~TextRenderer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
}

void TextRenderer::init(const char* vertexShaderPath, const char* fragmentShaderPath) {
    shaderProgram = glCreateProgram();
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShaderPath);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShaderPath);
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);
    glLinkProgram(shaderProgram);
    glDeleteShader(vs);
    glDeleteShader(fs);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
}

void TextRenderer::render(const std::string& text, float x, float y, glm::vec3 color, glm::mat4 projection) {
    char buffer[999];
    int quads = stb_easy_font_print(x, y, (char*)text.c_str(), NULL, buffer, sizeof(buffer));

    std::vector<float> vertices;

    for (int i = 0; i < quads; ++i) {
        float* quad = ((float*)buffer) + i * 4 * 2;

        // 4 tačke kvadrata
        float x0 = quad[0], y0 = quad[1];
        float x1 = quad[2], y1 = quad[3];
        float x2 = quad[4], y2 = quad[5];
        float x3 = quad[6], y3 = quad[7];

        // trougao 1: (0,1,2)
        vertices.insert(vertices.end(), { x0, y0, color.r, color.g, color.b });
        vertices.insert(vertices.end(), { x1, y1, color.r, color.g, color.b });
        vertices.insert(vertices.end(), { x2, y2, color.r, color.g, color.b });

        // trougao 2: (2,3,0)
        vertices.insert(vertices.end(), { x2, y2, color.r, color.g, color.b });
        vertices.insert(vertices.end(), { x3, y3, color.r, color.g, color.b });
        vertices.insert(vertices.end(), { x0, y0, color.r, color.g, color.b });
    }

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

    glUseProgram(shaderProgram);
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));

    std::cout << "Crtam " << (vertices.size() / 5) << " verteksa." << std::endl;

    glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 5); // svaka tačka je 5 float-a

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}
