//
// Created by dima on 28.03.2021.
//

#ifndef RAYMARCHING_OBJECTS_H
#define RAYMARCHING_OBJECTS_H

#include <boost/qvm/vec.hpp>
#include <string>
#include "SDL2/SDL_opengl.h"

#define vec3 boost::qvm::vec<float,3>
#define mat3 boost::qvm::mat<float,3,3>

#define vec4 boost::qvm::vec<float,4>
#define mat4 boost::qvm::mat<float,4,4>

class Shader{
private:
    GLuint idProgram;
    bool start = false;
public:

    Shader();

    Shader(const GLchar * vertexPath, const GLchar * fragmentPath);

    virtual ~Shader();

    int load(const GLchar * vertexPath, const GLchar * fragmentPath) const;

    int create(const GLchar * vertex, const GLchar * fragment,GLint lenV,GLint lenF) const;

    void link() const;

    void begin();

    void end();

    GLuint getIdProgram() const;

};
#endif //RAYMARCHING_OBJECTS_H
