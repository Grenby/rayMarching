#define GL_GLEXT_PROTOTYPES

#include <stdio.h>
#include <iostream>
#include <sys/time.h>
#include <cmath>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <vector>

#include <boost/qvm/vec.hpp>
#include <boost/qvm/vec_operations.hpp>
#include <boost/qvm/mat_operations.hpp>

#include <boost/qvm/map_mat_vec.hpp>
#include <boost/qvm/mat.hpp>
#include <cstring>

#include <iostream>
#include <fstream>

#include "objects.h"


#define vec2 boost::qvm::vec<float,2>
#define vec3 boost::qvm::vec<float,3>
#define vec4 boost::qvm::vec<float,4>

#define mat3 boost::qvm::mat<float,3,3>
#define mat4 boost::qvm::mat<float,4,4>


using namespace std;

typedef enum t_attrib_id {
    attrib_position
} t_attrib_id;

void sdl_init() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

}

int main() {

    sdl_init();

    const int width = 1920;
    const int height = 1080;

    uint32_t windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
    SDL_Window *window = SDL_CreateWindow("start window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height,
                                          windowFlags);

    SDL_GLContext context = SDL_GL_CreateContext(window);

    glDisable(GL_DEPTH_TEST);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glViewport(0, 0, width, height);


    Shader sh("../shaders/fractal2D/fractal.vertex.glsl","../shaders/fractal2D/fractal.fragment.glsl");
    glBindAttribLocation(sh.getIdProgram(), attrib_position, "i_pos");
    sh.link();

    vec3 tmp{};
    auto *positions_buffer_data = new GLfloat[2 * width * height]{0};

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            int index = (x * height + y) * 2;
            positions_buffer_data[index] = (float)x;
            positions_buffer_data[index + 1] = (float) (height - y);
        }
    }

    GLuint
            idVertexArray,
            idPositionBuffer;

    glGenVertexArrays(1, &idVertexArray);
    glGenBuffers(1, &idPositionBuffer);

    sh.begin();
    glBindVertexArray(idVertexArray);

    //positions
    glBindBuffer(GL_ARRAY_BUFFER, idPositionBuffer);

    glVertexAttribPointer(attrib_position, 2, GL_FLOAT, GL_TRUE, sizeof(float) * 2, nullptr);
    glBufferData(GL_ARRAY_BUFFER, width * height * 2 * sizeof(GLfloat), positions_buffer_data, GL_STATIC_DRAW);
    glEnableVertexAttribArray(attrib_position);

    glBindBuffer(GL_ARRAY_BUFFER,0);

    glBindVertexArray(0);

    int run = 1;

    bool FullScreen = windowFlags & SDL_WINDOW_FULLSCREEN_DESKTOP;

    bool printFPS = false;
    bool rotation = false;
    timeval time_now{};
    time_t timer;

    vec2 windowSize{width,height};
    vec2 addition_const{-.12,-0.77};
    const GLuint window_location = glGetUniformLocation(sh.getIdProgram(), "u_window");
    const GLuint const_location = glGetUniformLocation(sh.getIdProgram(), "u_c");

    glUniform2f(window_location, windowSize.a[0], windowSize.a[1]);
    glUniform2f(const_location, addition_const.a[0], addition_const.a[1]);

    sh.end();

    float angle = 0.001;

    while (run) {
        gettimeofday(&time_now, nullptr);
        timer = (time_now.tv_sec * 1000) + (time_now.tv_usec / 1000);

        SDL_Event Event;
        while (SDL_PollEvent(&Event)) {
            if (Event.type == SDL_KEYDOWN) {
                switch (Event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        run = 0;
                        break;
                    case SDLK_f:
                        FullScreen = !FullScreen;
                        if (FullScreen)
                            SDL_SetWindowFullscreen(window, windowFlags | SDL_WINDOW_FULLSCREEN_DESKTOP);
                        else
                            SDL_SetWindowFullscreen(window, windowFlags);
                        break;
                    case SDLK_p:
                        printFPS = !printFPS;
                        break;
                    case SDLK_r:
                        rotation = !rotation;
                        cout<<"rotate:"<<rotation<<'\n';
                        break;
                    default:
                        break;
                }
            } else if (Event.type == SDL_QUIT)
                run = 0;
        }


        if (rotation) {
            float x = -0.12f;
            float z =+0.4;

            angle += 0.1f;
            addition_const.a[0] = (float) (x) * cos(angle) - (float) (z) * sin(angle);
            addition_const.a[1] = (float) (x) * sin(angle) + (float) (z) * cos(angle);
        }


        sh.begin();


        glClear(GL_COLOR_BUFFER_BIT);

        glUniform2f(const_location, addition_const.a[0], addition_const.a[1]);

        glBindVertexArray(idVertexArray);
//        glBindBuffer(GL_ARRAY_BUFFER, idDirectionBuffer);
//        glBufferData(GL_ARRAY_BUFFER, width * height * 4 * sizeof(GLfloat), direction_buffer_data, GL_STREAM_DRAW);
//        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_POINTS, 0, width * height);

        glBindVertexArray(0);

        sh.end();

//        simpleSh.begin();
//        glBindVertexArray(VAO);
//        glDrawArrays(GL_TRIANGLES, 0, 3);
//        glBindVertexArray(0);
//        simpleSh.end();

        SDL_GL_SwapWindow(window);

        if (printFPS) {
            gettimeofday(&time_now, nullptr);
            timer = ((time_now.tv_sec * 1000) + (time_now.tv_usec / 1000) - timer);
            std::cout << "update:" << timer << '\n';
        }
    }

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}