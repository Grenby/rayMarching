#define GL_GLEXT_PROTOTYPES

#include "objects.h"
#include <iostream>
#include <fstream>
#include <SDL2/SDL_opengl.h>

using namespace std;

Shader::Shader(const char *vertexPath, const char *fragmentPath) {
    this->idProgram = glCreateProgram();
    load(vertexPath,fragmentPath);
}

Shader::Shader() {
    this->idProgram = glCreateProgram();
}

Shader::~Shader() {
    glDeleteProgram(this->idProgram);
}

int Shader::load(const char *vertexPath, const char *fragmentPath) const {
    GLchar *vertex;
    GLchar *fragment;
    GLint lenV;
    GLint lenF;

    fstream file;
    file.open(vertexPath, ios::out | ios::in);

    if (file.is_open()) {
        fprintf(stdout,"Vertex shader open\n");
        file.seekg(0, ios::end);
        lenV = file.tellg();
        file.seekg(0, ios::beg);
        vertex = new char[lenV];
        file.read(vertex, lenV);
        file.close();
    } else {
        fprintf(stderr, "Vertex shader don't open\n");
        return false;
    }

    file.open(fragmentPath, ios::out | ios::in);
    if (file.is_open()) {
        fprintf(stdout,"Fragment shader open\n");
        file.seekg(0, ios::end);
        lenF = file.tellg();
        file.seekg(0, ios::beg);
        fragment = new char[lenF];
        file.read(fragment, lenF);
        file.close();
    } else {
        fprintf(stderr, "Fragment shader don't open\n");
        return false;
    }
    int res = create(vertex,fragment,lenV,lenF);

    delete[] vertex;
    delete[] fragment;

    return res;
}

int Shader::create(const GLchar *vertex, const GLchar *fragment,GLint lenV, GLint lenF) const {
    GLuint idVertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint idFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(idVertexShader, 1, (const GLchar **) &vertex, &lenV);
    glShaderSource(idFragmentShader, 1, (const GLchar **) &fragment, &lenF);

    glCompileShader(idVertexShader);
    glCompileShader(idFragmentShader);


    GLint status;
    glGetShaderiv(idVertexShader, GL_COMPILE_STATUS, &status);

    if (status == GL_FALSE) {
        fprintf(stderr, "vertex shader compilation failed\n");
        return false;
    }else{
        fprintf(stdout,"Vertex shader compilation is successful\n");
        glAttachShader(idProgram, idVertexShader);
    }

    glGetShaderiv(idFragmentShader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        fprintf(stderr, "fragment shader compilation failed\n");
        return false;
    }else {
        fprintf(stdout, "Fragment shader compilation is successful\n");
        glAttachShader(idProgram, idFragmentShader);
    }
    return true;
}

void Shader::begin(){
    start = true;
    glUseProgram(this->idProgram);
}

void Shader::end() {
    if (start){
        start = false;
        glUseProgram(0);
    }
}

GLuint Shader::getIdProgram() const {
    return idProgram;
}

void Shader::link() const {
    glLinkProgram(this->idProgram);
}

