//
// Created by Rushin Shah on 21/10/26.
//

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <optional>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../../include/image/image_loader.h"

GLFWwindow * init() {
    if (!glfwInit()) {
        std::cout << "Failure initializing glfw" << std::endl;
        return nullptr;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    constexpr int width = 800, height = 600;
    GLFWwindow * window = glfwCreateWindow(width, height, "Transformation and Camera", nullptr, nullptr);
    if (!window) {
        std::cout << "Failure creating window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Fail to load opengl function pointer" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* _, int width, int height) {
        glViewport(0, 0, width, height);
    });
    return window;
}

static void create_triangle(const float vertices[], const size_t sz, unsigned int* VAO) {
    unsigned VBO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, VAO);
    glBindVertexArray(*VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,sz, vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

class Program {
public:
    unsigned int ID;
    Program(const std::string& vert_shader_loc, const std::string& frag_shader_loc) {
        vert_shader_file.open(vert_shader_loc);
        frag_shader_file.open(frag_shader_loc);
        create_program();
    }
    int use() const {
        if (!prog_created) {
            std::cout << "FAILED::PROGRAM::USING" << std::endl;
            return -1;
        }
        glUseProgram(ID);
        return 1;
    }
private:
    std::ifstream vert_shader_file, frag_shader_file;
    bool prog_created = false;

    std::optional<std::string> read_source(std::ifstream* file) noexcept{
        if (!file->is_open()) {
            std::cout << "File not able to open" << std::endl;
            return std::nullopt;
        }
        std::string src = "";
        std::string line;
        while (std::getline(*file, line)) {
            if (src != "") src += "\n";
            src += line;
        }
        return src;
    }

    int create_shader(std::ifstream* file, unsigned int shader, unsigned int * shaderID) noexcept {
        const auto src = read_source(file);
        if (src.has_value()) {

            const char * src_c = src->c_str();
            *shaderID = glCreateShader(shader);
            glShaderSource(*shaderID, 1, &src_c, nullptr);
            glCompileShader(*shaderID);
            int success;
            char log[512];
            glGetShaderiv(*shaderID, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(*shaderID, 512, nullptr, log);
                std::cout << "ERROR::COMPILE::SHADER" << " " << log << std::endl;
                return -1;
            }
            std::cout << "Shader Compiled successfully" << std::endl;
            return 1;
        }
        return -1;
    }

    void create_program() {
        unsigned int vert_shader_id, frag_shader_id;
        int success = create_shader(&vert_shader_file, GL_VERTEX_SHADER, &vert_shader_id);
        if (success == -1) {
            std::cout << "VERTEX_SHADER_COMPILATION_FAILED" << std::endl;
            return;
        }
        success = create_shader(&frag_shader_file, GL_FRAGMENT_SHADER, &frag_shader_id);
        if (success == -1) {
            std::cout << "FRAG_SHADER_COMPILATION_FAILED" << std::endl;
            return;
        }
        ID = glCreateProgram();
        glAttachShader(ID, vert_shader_id);
        glAttachShader(ID, frag_shader_id);
        glLinkProgram(ID);
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if (!success) {
            char log[512];
            glGetProgramInfoLog(ID, 512, nullptr, log);
            std::cout << "ERROR::PROGRAM::LINK::FAILED " << log << std::endl;
            return;
        }
        prog_created = true;

    }
};

unsigned char* load_image(const std::string& loc, int * width, int* height, int* nrchannels) {
    const char * loc_c = loc.c_str();
    unsigned char * data = stbi_load(loc_c, width, height, nrchannels, 0);
    return data;
}

int load_texture(const std::string& loc, const int image_internal_format, const Program& prog, const std::string& shader_var, int shader_var_pos) {
    int width, height, nrchannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = load_image(loc, &width, &height, &nrchannels);
    if (!data) {
        std::cout << "FAILED::IMAGE::LOADING" << " " << loc << std::endl;
        return -1;
    }
    unsigned int texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0 + shader_var_pos);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, image_internal_format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    const char* shader_var_c = shader_var.c_str();
    glUniform1i(glGetUniformLocation(prog.ID, shader_var_c), shader_var_pos);
    stbi_image_free(data);
    return 1;
}

int main() {
    GLFWwindow * window = init();
    if (!window) {
        return -1;
    }
    const float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    glEnable(GL_DEPTH_TEST);
    unsigned int VAO;
    create_triangle(vertices, sizeof(vertices), &VAO);
    Program prog("/Users/rusher/OpenGL_Course/shaders/shader.vsh", "/Users/rusher/OpenGL_Course/shaders/shader.fsh");
    if (prog.use() == -1) {
        std::cout << "Program creation failed" << std::endl;
        glfwTerminate();
        return -1;
    }
    float visibility = 0.25f;
    glfwSetWindowUserPointer(window, &visibility);
    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        float *val = static_cast<float *>(glfwGetWindowUserPointer(window));
        if (key == GLFW_KEY_UP && action == GLFW_PRESS)
        {
            *val += 0.08f;
            *val = std::min(*val, 1.0f);
        }
        if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
        {
            *val -= 0.08f;
            *val = std::max(*val, 0.0f);
        }
    });
    glBindVertexArray(VAO);
    load_texture("/Users/rusher/OpenGL_Course/artifacts/container.jpg", GL_RGB, prog, "texture", 0);
    load_texture("/Users/rusher/OpenGL_Course/artifacts/awesomeface.png", GL_RGBA, prog, "smily_text", 1);
    std::vector cube_pos {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };
    struct Camera{
        glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
        glm::vec3 cameraLook = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
        float prev_screen_time = 0;
        float cur_window_time = (float)glfwGetTime();
        float xpos = 0.0f;
        float ypos = 0.0f;
        float pitch = 0.0f;
        float yaw = 0.f;
        const float velocity = 5.0f;
        const float angle_per_unit_move = 0.3f;
        bool first_in = true;
        float fov = 20.0f;
    } camera;


    glfwSetWindowUserPointer(window, &camera);
    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        auto* camera = static_cast<Camera *>(glfwGetWindowUserPointer(window));
        const float delta = camera->cur_window_time - camera->prev_screen_time;
        camera->prev_screen_time = camera->cur_window_time;


        if (key == GLFW_KEY_W) {
            camera->cameraPos += camera->velocity * delta * camera->cameraLook;
        } else if (key == GLFW_KEY_S) {
            camera->cameraPos -= camera->velocity * delta * camera->cameraLook;
        } else if (key == GLFW_KEY_A) {
            camera->cameraPos += camera->velocity * delta * glm::normalize(glm::cross(camera->cameraUp, camera->cameraLook));
        } else if (key == GLFW_KEY_D) {
            camera->cameraPos -= camera->velocity * delta * glm::normalize(glm::cross(camera->cameraUp, camera->cameraLook));
        }

    });

    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
        auto* camera = static_cast<Camera *>(glfwGetWindowUserPointer(window));
        float diffx = xpos - camera->xpos;
        float diffy = ypos - camera->ypos;
        camera->xpos = xpos;
        camera->ypos = ypos;
        if (camera->first_in) {
            camera->first_in = false;
            return;
        }
        diffx *= camera->angle_per_unit_move;
        diffy *= camera->angle_per_unit_move;
        camera->pitch += diffy;
        if (camera->pitch < -89.0f) {
            camera->pitch = -89.0f;
        }
        if (camera->pitch > 89.0f) {
            camera->pitch = 89.0f;
        }
        camera->yaw += diffx;
        auto dir = glm::vec3(1.0f);
        dir.x = sin(glm::radians(camera->yaw)) * cos(glm::radians(camera->pitch));
        dir.y = -sin(glm::radians(camera->pitch));
        dir.z = -cos(glm::radians(camera->yaw)) * cos(glm::radians(camera->pitch));
        camera->cameraLook = glm::normalize(dir);
    });
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset)
    {
        auto* camera = static_cast<Camera *>(glfwGetWindowUserPointer(window));
        camera->fov -= (float)yoffset;
        if (camera->fov < 1.0f)
            camera->fov = 1.0f;
        if (camera->fov > 45.0f)
            camera->fov = 45.0f;
    });
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClearColor(0.5f, 0.4f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUniform1f(glGetUniformLocation(prog.ID, "visibility"), visibility);
        camera.prev_screen_time = camera.cur_window_time;
        camera.cur_window_time = (float)glfwGetTime();

        for (int i = 0; i < 10; i++) {
            auto model = glm::mat4(1.0f);
            model = glm::translate(model, cube_pos[i]);
            const float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            // constexpr float radius = 10.0f;
            // const float cameraX = sin((float)glfwGetTime()) * radius;
            // const float cameraZ = cos((float)glfwGetTime()) * radius;
            // auto view = glm::lookAt(glm::vec3(cameraX, 0.0f, cameraZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            auto view = glm::lookAt(camera.cameraPos, camera.cameraLook + camera.cameraPos, camera.cameraUp);
            auto projection = glm::perspective(glm::radians(camera.fov), 800.0f / 600.0f, 0.1f, 100.0f);
            // auto projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);
            // float aspect = 800.0f / 600.0f;
            // float halfH  = 3.0f;              // world units visible above/below center
            // float halfW  = halfH * aspect;    // keep cubes square
            // auto projection = glm::ortho(-halfW, halfW, -halfH, halfH, 0.1f, 100.0f);
            glUniformMatrix4fv(glGetUniformLocation(prog.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix4fv(glGetUniformLocation(prog.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(prog.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(window);
    }

    return 0;
}