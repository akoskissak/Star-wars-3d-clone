#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <sstream>

#include <GL/glew.h> 
#include <GLFW/glfw3.h>

//GLM biblioteke
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

unsigned int compileShader(GLenum type, const char* source);
unsigned int createShader(const char* vsSource, const char* fsSource);

glm::vec3 shipPosition = glm::vec3(0.0f, 0.0f, 10.0f);
float shipRotationY = 0.0f; // u radijanima
float shipSpeed = 0.05f;
float shipTurnSpeed = glm::radians(2.0f);

const int NUM_STARS = 200;
std::vector<glm::vec3> stars;
GLuint starVAO = 0, starVBO = 0;

GLuint loadTexture(const char* path) {
    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = nrChannels == 4 ? GL_RGBA : GL_RGB;
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else {
        std::cout << "Failed to load texture: " << path << std::endl;
    }
    stbi_image_free(data);
    return textureID;
}

int main(void)
{
    if (!glfwInit())
    {
        std::cout << "GLFW Biblioteka se nije ucitala! :(\n";
        return 1;
    }


    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window;
    unsigned int wWidth = 1000;
    unsigned int wHeight = 800;
    const char wTitle[] = "Star wars brod";
    window = glfwCreateWindow(wWidth, wHeight, wTitle, NULL, NULL);

    if (window == NULL)
    {
        std::cout << "Prozor nije napravljen! :(\n";
        glfwTerminate();
        return 2;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // za 60FPS
    glViewport(0, 0, wWidth, wHeight);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW nije mogao da se ucita! :'(\n";
        return 3;
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ PROMJENLJIVE I BAFERI +++++++++++++++++++++++++++++++++++++++++++++++++
    unsigned int unifiedShader = createShader("basic.vert", "basic.frag");

   

    float cubeVertices[] = {
        // prednja
        -0.5f,-0.5f, 0.5f,  0.5f,-0.5f, 0.5f,  0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f,-0.5f, 0.5f,
        // zadnja
        -0.5f,-0.5f,-0.5f, -0.5f, 0.5f,-0.5f,  0.5f, 0.5f,-0.5f,
        0.5f, 0.5f,-0.5f,  0.5f,-0.5f,-0.5f, -0.5f,-0.5f,-0.5f,
        // leva
        -0.5f, 0.5f, 0.5f, -0.5f, 0.5f,-0.5f, -0.5f,-0.5f,-0.5f,
        -0.5f,-0.5f,-0.5f, -0.5f,-0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
        // desna
        0.5f, 0.5f, 0.5f,  0.5f,-0.5f, 0.5f,  0.5f,-0.5f,-0.5f,
        0.5f,-0.5f,-0.5f,  0.5f, 0.5f,-0.5f,  0.5f, 0.5f, 0.5f,
        // donja
        -0.5f,-0.5f,-0.5f,  0.5f,-0.5f,-0.5f,  0.5f,-0.5f, 0.5f,
        0.5f,-0.5f, 0.5f, -0.5f,-0.5f, 0.5f, -0.5f,-0.5f,-0.5f,
        // gornja
        -0.5f, 0.5f,-0.5f, -0.5f, 0.5f, 0.5f,  0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,  0.5f, 0.5f,-0.5f, -0.5f, 0.5f,-0.5f
    };

    float pyramidVertices[] = {
        // baza (kvadrat)
        -0.5f, 0.5f, -0.5f,
         0.5f, 0.5f, -0.5f,
         0.5f, 0.5f,  0.5,
         0.5f, 0.5f,  0.5f,
        -0.5f, 0.5f,  0.5f,
        -0.5f, 0.5f, -0.5f,

        // strane (4 trougla)
        -0.5f, 0.5f, -0.5f,  0.5f, 0.5f, -0.5f,  0.0f, 1.3f, 0.0f,
         0.5f, 0.5f, -0.5f,  0.5f, 0.5f,  0.5f,  0.0f, 1.3f, 0.0f,
         0.5f, 0.5f,  0.5f, -0.5f, 0.5f,  0.5f,  0.0f, 1.3f, 0.0f,
        -0.5f, 0.5f,  0.5f, -0.5f, 0.5f, -0.5f,  0.0f, 1.3f, 0.0f
    };

    float quadVertices[] = {
        // pozicija       // tex koordinate
        // gornji levi trougao
        0.0f, 1.0f, 0.0f,   0.0f, 1.0f, // top-left
        1.0f, 0.0f, 0.0f,   1.0f, 0.0f, // bottom-right
        0.0f, 0.0f, 0.0f,   0.0f, 0.0f, // bottom-left

        // donji desni trougao
        0.0f, 1.0f, 0.0f,   0.0f, 1.0f, // top-left
        1.0f, 1.0f, 0.0f,   1.0f, 1.0f, // top-right
        1.0f, 0.0f, 0.0f,   1.0f, 0.0f  // bottom-right
    };


    GLuint cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    GLuint pyramidVAO, pyramidVBO;
    glGenVertexArrays(1, &pyramidVAO);
    glGenBuffers(1, &pyramidVBO);
    glBindVertexArray(pyramidVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pyramidVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    GLuint quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);

    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    // Pozicija
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Tekstura koordinate
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    // Generisanje zvezda
    for (int i = 0; i < NUM_STARS; ++i) {
        float x = (rand() % 200 - 100) * 0.5f;
        float y = (rand() % 100 - 50) * 0.5f + 10.0f;
        float z = (rand() % 200 - 100) * 0.5f;
        stars.emplace_back(x, y, z);
    }

    glGenVertexArrays(1, &starVAO);
    glGenBuffers(1, &starVBO);
    glBindVertexArray(starVAO);
    glBindBuffer(GL_ARRAY_BUFFER, starVBO);
    glBufferData(GL_ARRAY_BUFFER, stars.size() * sizeof(glm::vec3), &stars[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++            UNIFORME            +++++++++++++++++++++++++++++++++++++++++++++++++

    glm::mat4 model = glm::mat4(1.0f); //Matrica transformacija - mat4(1.0f) generise jedinicnu matricu

    glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)wWidth / (float)wHeight, 0.1f, 100.0f); //Matrica perspektivne projekcije (FOV, Aspect Ratio, prednja ravan, zadnja ravan)

    glm::mat4 projectionO = glm::ortho(0.0f, (float)wWidth, (float)wHeight, 0.0f, -1.0f, 1.0f); //Matrica ortogonalne projekcije (Lijeva, desna, donja, gornja, prednja i zadnja ravan)


    GLuint texture = loadTexture("ime.png");
    // Postavljanje šejdera
    glUseProgram(unifiedShader);
    GLuint modelLoc = glGetUniformLocation(unifiedShader, "model");
    GLuint viewLoc = glGetUniformLocation(unifiedShader, "view");
    GLuint projLoc = glGetUniformLocation(unifiedShader, "projection");
    GLuint colorLoc = glGetUniformLocation(unifiedShader, "objectColor");
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ RENDER LOOP - PETLJA ZA CRTANJE +++++++++++++++++++++++++++++++++++++++++++++++++

    bool depthEnabled = true;
    bool cullEnabled = true;

    glEnable(GL_CULL_FACE);         // Omogući odstranjivanje lica
    glCullFace(GL_BACK);            // Uklanjaj zadnja lica
    glFrontFace(GL_CCW);            // Prednja lica su ona čiji su verteksi CCW

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    while (!glfwWindowShouldClose(window))
    {
        // Kamera
        glm::vec3 cameraOffset = glm::vec3(-4.0f * sin(shipRotationY), 2.5f, 4.0f * cos(shipRotationY));
        glm::vec3 cameraPos = shipPosition + cameraOffset;
        glm::vec3 cameraTarget = shipPosition;
        glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, glm::vec3(0, 1, 0));

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);

        // Aktivacija i deaktivacija testova
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) depthEnabled = true;
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) depthEnabled = false;
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) cullEnabled = true;
        if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) cullEnabled = false;

        if (depthEnabled) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
        if (cullEnabled)  glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);

        // Kontrole kretanja broda
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            shipPosition.x += sin(shipRotationY) * shipSpeed;
            shipPosition.z -= cos(shipRotationY) * shipSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            shipPosition.x -= sin(shipRotationY) * shipSpeed;
            shipPosition.z += cos(shipRotationY) * shipSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            shipRotationY += shipTurnSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            shipRotationY -= shipTurnSpeed;

        glClearColor(0.1, 0.1, 0.2, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);






        glUniform1i(glGetUniformLocation(unifiedShader, "useTexture"), 0);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // Svemirska stanica
        glm::mat4 stationModel = glm::mat4(1.0f);
        stationModel = glm::scale(stationModel, glm::vec3(4.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(stationModel));
        glUniform3f(colorLoc, 0.3f, 0.3f, 0.35f);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Transformacija broda
        glm::mat4 shipModel = glm::mat4(1.0f);
        shipModel = glm::translate(shipModel, shipPosition);
        shipModel = glm::rotate(shipModel, shipRotationY, glm::vec3(0, 1, 0));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(shipModel));
        glUniform3f(colorLoc, 0.8f, 0.8f, 0.85f);

        // Iscrtavanje broda
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindVertexArray(pyramidVAO);
        glDrawArrays(GL_TRIANGLES, 0, 18);

        // Iscrtavanje zvezda
        glPointSize(2.0f);
        glm::mat4 starModel = glm::mat4(1.0f);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(starModel));
        glUniform3f(colorLoc, 1.0f, 1.0f, 1.0f);

        glBindVertexArray(starVAO);
        glDrawArrays(GL_POINTS, 0, NUM_STARS);

        // Tekst
        // Pravougaonik sa teksturom

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(unifiedShader, "tex"), 0);
        glUniform1i(glGetUniformLocation(unifiedShader, "useTexture"), 1);

        glm::mat4 ime_view = glm::mat4(1.0f); // nema kamere u orto pogledu
        glm::mat4 ime_model = glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, wHeight - 160.0f, 0.0f));
        ime_model = glm::scale(ime_model, glm::vec3(256.0f, 256.0f, 1.0f));

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(ime_model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(ime_view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projectionO));

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ POSPREMANJE +++++++++++++++++++++++++++++++++++++++++++++++++

    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &pyramidVBO);
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &pyramidVAO);
    glDeleteProgram(unifiedShader);

    glfwTerminate();
    return 0;
}

unsigned int compileShader(GLenum type, const char* source)
{
    std::string content = "";
    std::ifstream file(source);
    std::stringstream ss;
    if (file.is_open())
    {
        ss << file.rdbuf();
        file.close();
        std::cout << "Uspjesno procitao fajl sa putanje \"" << source << "\"!" << std::endl;
    }
    else {
        ss << "";
        std::cout << "Greska pri citanju fajla sa putanje \"" << source << "\"!" << std::endl;
    }
    std::string temp = ss.str();
    const char* sourceCode = temp.c_str();

    int shader = glCreateShader(type);

    int success;
    char infoLog[512];
    glShaderSource(shader, 1, &sourceCode, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        if (type == GL_VERTEX_SHADER)
            printf("VERTEX");
        else if (type == GL_FRAGMENT_SHADER)
            printf("FRAGMENT");
        printf(" sejder ima gresku! Greska: \n");
        printf(infoLog);
    }
    return shader;
}
unsigned int createShader(const char* vsSource, const char* fsSource)
{
    unsigned int program;
    unsigned int vertexShader;
    unsigned int fragmentShader;

    program = glCreateProgram();

    vertexShader = compileShader(GL_VERTEX_SHADER, vsSource);
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSource);

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);
    glValidateProgram(program);

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(program, 512, NULL, infoLog);
        std::cout << "Objedinjeni sejder ima gresku! Greska: \n";
        std::cout << infoLog << std::endl;
    }

    glDetachShader(program, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(fragmentShader);

    return program;
}