#pragma once
#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <string>
#include <glm/glm.hpp>

class TextRenderer {
public:
    TextRenderer();
    ~TextRenderer();
    void init(const char* vertexShaderPath, const char* fragmentShaderPath);
    void render(const std::string& text, float x, float y, glm::vec3 color, glm::mat4 projection);
private:
    unsigned int VAO, VBO;
    unsigned int shaderProgram;
};



#endif