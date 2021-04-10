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


struct camera {
    vec3
            pos = {0, 0, 0},
            dir = {0, 0, 1},
            up = {0, 1, 0},
            right = {1,0,0};
    void look(const vec3 &point){
        dir = point - pos;
        normalize(dir);
    };

};


using namespace std;
const float angle = 67. / 180. * M_PI;

const vec3 lightColor{.8, 0.6, .6};
vec3 lightPos{0, 5, 0};
vec3 point{0, 0, 10};

camera cam;

typedef enum t_attrib_id {
    attrib_position,
    attrib_direction
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


    const GLchar *  s1 = "../shaders/rayMarching/fractalCubeTorus.glsl";
    const GLchar *  s2 = "../shaders/rayMarching/torSmoothSphere.glsl";
    const GLchar *  s3 = "../shaders/rayMarching/fractalCube.glsl";


    Shader sh("../shaders/rayMarching/vertex.glsl",s3);
    glBindAttribLocation(sh.getIdProgram(), attrib_position, "i_pos");
    glBindAttribLocation(sh.getIdProgram(), attrib_direction, "i_direction");
    sh.link();

//    GLuint simplePos,simpleColor;
//    Shader simpleSh ("../shaders/simple.vertex.glsl","../shaders/simple.fragment.glsl");
//    glBindAttribLocation(simpleSh.getIdProgram(), simplePos, "i_position");
//    glBindAttribLocation(simpleSh.getIdProgram(), simpleColor, "i_color");
//    simpleSh.link();

    const auto d = (float) (height / 2. / tan(angle / 2));
    vec3 tmp{};
    auto *direction_buffer_data = new GLfloat[4 * width * height]{0};
    auto *positions_buffer_data = new GLfloat[2 * width * height]{0};

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            int index = (x * height + y) * 2;
            positions_buffer_data[index] = (float) (x - width / 2.) / width * 2;
            positions_buffer_data[index + 1] = (float) (height / 2. - y) / height * 2;

            tmp.a[0] = (float) (x - width / 2.0);
            tmp.a[1] = (float) (-y + height / 2.0);
            tmp.a[2] = d;
            normalize(tmp);

            index = (x * height + y) * 3;

            direction_buffer_data[index] = tmp.a[0];
            direction_buffer_data[index + 1] = tmp.a[1];
            direction_buffer_data[index + 2] = tmp.a[2];
        }
    }

    GLuint
            idVertexArray,
            idPositionBuffer,
            idDirectionBuffer;


    glGenVertexArrays(1, &idVertexArray);
    glGenBuffers(1, &idPositionBuffer);
    glGenBuffers(1, &idDirectionBuffer);



    sh.begin();

    glBindVertexArray(idVertexArray);

    //positions
    glBindBuffer(GL_ARRAY_BUFFER, idPositionBuffer);

    glVertexAttribPointer(attrib_position, 2, GL_FLOAT, GL_TRUE, sizeof(float) * 2, nullptr);
    glBufferData(GL_ARRAY_BUFFER, width * height * 2 * sizeof(GLfloat), positions_buffer_data, GL_STATIC_DRAW);
    glEnableVertexAttribArray(attrib_position);

    //directions
    glBindBuffer(GL_ARRAY_BUFFER, idDirectionBuffer);

    glBufferData(GL_ARRAY_BUFFER, width * height * 3 * sizeof(GLfloat), direction_buffer_data, GL_STATIC_DRAW);
    glVertexAttribPointer(attrib_direction, 3, GL_FLOAT, GL_TRUE, sizeof(float) * 3, nullptr);
    glEnableVertexAttribArray(attrib_direction);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    int run = 1;
    bool FullScreen = windowFlags & SDL_WINDOW_FULLSCREEN_DESKTOP;

    bool printFPS = false;
    bool rotateCamera = true;

    timeval time_now{};
    time_t timer;


    const GLuint matrix_location = glGetUniformLocation(sh.getIdProgram(), "transfrom");
    const GLuint camera_pos_location = glGetUniformLocation(sh.getIdProgram(), "u_camera_pos");
    const GLuint light_pos_location = glGetUniformLocation(sh.getIdProgram(), "u_light_pos");
    const GLuint light_color_location = glGetUniformLocation(sh.getIdProgram(), "u_light_color");


    mat3 i_m{};

    auto *matrix_array = new GLfloat[9];

    glUniform3f(light_pos_location, lightPos.a[0], lightPos.a[1], lightPos.a[2]);
    glUniform4f(light_color_location, lightColor.a[0], lightColor.a[1], lightColor.a[2], 1);

    sh.end();

//    simpleSh.begin();
//    GLfloat vertices[] = {
//            // Positions         // Colors
//            0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,  // Bottom Right
//            -0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,  // Bottom Left
//            0.0f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f   // Top
//    };
//    GLuint VBO, VAO;
//    glGenVertexArrays(1, &VAO);
//    glGenBuffers(1, &VBO);
//
//    glBindVertexArray(VAO);
//    glBindBuffer(GL_ARRAY_BUFFER, VBO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//    // Position attribute
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
//    glEnableVertexAttribArray(0);
//    // Color attribute
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
//    glEnableVertexAttribArray(1);
//
//    glBindVertexArray(0); // Unbind VAO
//    simpleSh.end();

    while (run) {
        gettimeofday(&time_now, nullptr);
        timer = (time_now.tv_sec * 1000) + (time_now.tv_usec / 1000);

        float s =0.5;
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
                    case SDLK_w:
                        cam.pos += s*cam.dir;
                        break;
                    case SDLK_d:
                        cam.pos += s*cam.right;
                        break;
                    case SDLK_s:
                        cam.pos -= s*cam.dir;
                        break;
                    case SDLK_a:
                        cam.pos -= s*cam.right;
                        break;
                    case SDLK_r:
                        rotateCamera = !rotateCamera;
                        cout<<"rotate:"<<rotateCamera<<'\n';
                        break;
                    default:
                        break;
                }
            } else if (Event.type == SDL_QUIT)
                run = 0;
        }


        if (rotateCamera){
            float x = cam.pos.a[0];
            float z = cam.pos.a[2];
            float a = 0.01f;

            cam.pos.a[0] = point.a[0] + (float) (x - point.a[0]) * cos(a) - (float) (z - point.a[2]) * sin(a);
            cam.pos.a[2] = point.a[2] + (float) (x - point.a[0]) * sin(a) + (float) (z - point.a[2]) * cos(a);

            x = lightPos.a[0];
            z = lightPos.a[2];

            lightPos.a[0] = point.a[0] + (float) (x - point.a[0]) * cos(a) - (float) (z - point.a[2]) * sin(a);
            lightPos.a[2] = point.a[2] + (float) (x - point.a[0]) * sin(a) + (float) (z - point.a[2]) * cos(a);

        }

        cam.look(point);
        cam.right = cross(cam.up, cam.dir);

        i_m.a[0][0] = cam.right.a[0];
        i_m.a[0][1] = cam.right.a[1];
        i_m.a[0][2] = cam.right.a[2];

        i_m.a[1][0] = cam.up.a[0];
        i_m.a[1][1] = cam.up.a[1];
        i_m.a[1][2] = cam.up.a[2];

        i_m.a[2][0] = cam.dir.a[0];
        i_m.a[2][1] = cam.dir.a[1];
        i_m.a[2][2] = cam.dir.a[2];

        inverse(i_m);

        for (int i = 0; i < 9; i++) {
            matrix_array[i] = i_m.a[i / 3][i % 3];
//            cout<<matrix_array[i]<<' ';
        }

        sh.begin();

        glUniformMatrix3fv(matrix_location, 1, GL_FALSE, matrix_array);
        glUniform3f(camera_pos_location, cam.pos.a[0], cam.pos.a[1], cam.pos.a[2]);
        glUniform3f(light_pos_location, lightPos.a[0], lightPos.a[1], lightPos.a[2]);

        glClear(GL_COLOR_BUFFER_BIT);

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

    delete[] matrix_array;

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
