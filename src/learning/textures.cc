#include <iostream>
#include <fstream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../../include/image/image_loader.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

unsigned char *load_image(const std::string &loc, int *width, int *height, int *nchannels)
{
    const char *loc_str = loc.c_str();
    unsigned char *data = stbi_load(loc_str, width, height, nchannels, 0);
    return data;
}

void create_triangle_vao(const float *vertices, size_t sz, unsigned int *VBO, unsigned int *VAO)
{
    glGenBuffers(1, VBO);
    glGenVertexArrays(1, VAO);
    glBindVertexArray(*VAO);
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glBufferData(GL_ARRAY_BUFFER, sz, vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

void create_rectangle(const int *indexes, size_t sz, unsigned int *VAO)
{
    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindVertexArray(*VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sz, indexes, GL_STATIC_DRAW);
}

GLFWwindow *init()
{
    if (!glfwInit())
    {
        std::cout << "GLFW init failed" << std::endl;
        return nullptr;
    }
    int width = 800, height = 600;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    GLFWwindow *window = glfwCreateWindow(width, height, "Hello Textures", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Glad init failed" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int width, int height)
                                   { glViewport(0, 0, width, height); });
    return window;
}

class Program
{
private:
    std::ifstream vertex_shader_file, frag_shader_file;
    unsigned int vertex_shader, frag_shader;
    bool is_prog_created = false;

    void compile_shaders()
    {
        std::string vertex_shader_src = read_shader(&vertex_shader_file);
        std::string frag_shader_src = read_shader(&frag_shader_file);
        const char *vertex_shader_source = vertex_shader_src.c_str();
        const char *frag_shader_source = frag_shader_src.c_str();

        vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr);
        glShaderSource(frag_shader, 1, &frag_shader_source, nullptr);
        glCompileShader(vertex_shader);
        int success;
        char log[512];
        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertex_shader, 512, nullptr, log);
            std::cout << "ERROR::SHADER::VERTEX_SHADER " << log << std::endl;
        }
        glCompileShader(frag_shader);
        glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(frag_shader, 512, nullptr, log);
            std::cout << "ERROR::SHADER::FRAG_SHADER " << log << std::endl;
        }
    }

    std::string read_shader(std::ifstream *shader)
    {
        if (!(shader->is_open()))
        {
            std::cout << "Fail to open file!" << std::endl;
            return "";
        }
        std::string source_line;
        std::string prog = "";
        while (std::getline(*shader, source_line))
        {
            if (prog != "")
                prog += "\n";
            prog += source_line;
        }
        return prog;
    }

public:
    unsigned int prog;
    Program(const std::string &vertex_shader_loc, const std::string &frag_shader_loc)
    {
        vertex_shader_file.open(vertex_shader_loc);
        frag_shader_file.open(frag_shader_loc);
    }
    void use()
    {
        if (!is_prog_created)
        {
            is_prog_created = true;
            compile_shaders();
            prog = glCreateProgram();
            glAttachShader(prog, vertex_shader);
            glAttachShader(prog, frag_shader);
            glLinkProgram(prog);
            int success;
            char info[512];
            glGetProgramiv(prog, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(prog, 512, nullptr, info);
                std::cout << "ERROR::PROGRAM " << info << std::endl;
                return;
            }
        }
        glUseProgram(prog);
    }
};

void process_key_pressed(GLFWwindow *window, int key, int scancode, int action, int mods)
{
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
};

int main()
{

    GLFWwindow *window = init();

    if (window == nullptr)
    {
        std::cout << "Issue creating window or initializing function pointers" << std::endl;
        return -1;
    }
    Program prog("/Users/rusher/OpenGL_Course/shader.vsh", "/Users/rusher/OpenGL_Course/shader.fsh");
    int width, height, nchannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *image = load_image("/Users/rusher/OpenGL_Course/container.jpg", &width, &height, &nchannels);
    unsigned int texture_wooden_container;
    glGenTextures(1, &texture_wooden_container);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_wooden_container);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (image)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(image);

    unsigned int smily_texture;
    glGenTextures(1, &smily_texture);
    unsigned char *smily_image = load_image("/Users/rusher/OpenGL_Course/awesomeface.png", &width, &height, &nchannels);

    glBindTexture(GL_TEXTURE_2D, smily_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (smily_image)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, smily_image);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(smily_image);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_wooden_container);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, smily_texture);

    unsigned int VAO, VBO;
    const float vertices[] = {
        0.0f, 0.0f, 0.0f, 0.7f, 0.2f, 0.1f, 0.0f, 0.0f,
        0.5f, 0.0f, 0.0f, 0.2f, 0.2f, 0.6f, 1.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 0.7f, 0.2f, 0.1f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.0f, 0.2f, 0.2f, 0.6f, 1.0f, 1.0f};

    create_triangle_vao(vertices, sizeof(vertices), &VBO, &VAO);
    const int indexes[] = {
        0, 1, 2,
        1, 2, 3};
    create_rectangle(indexes, sizeof(indexes), &VAO);
    glBindVertexArray(VAO);
    prog.use();

    glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
    std::cout << vec.x << vec.y << vec.z << std::endl;
    glUniform1i(glGetUniformLocation(prog.prog, "text"), 0);
    glUniform1i(glGetUniformLocation(prog.prog, "smily_text"), 1);
    float smily_visible = 0.3f;
    glfwSetWindowUserPointer(window, &smily_visible);
    glfwSetKeyCallback(window, process_key_pressed);

    while (!glfwWindowShouldClose(window))
    {
        glUniform1f(glGetUniformLocation(prog.prog, "visibility"), smily_visible);
        glfwPollEvents();

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -2.0f));
        // glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.0f, 100.0f);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 3.0f, 100.f);
        glUniformMatrix4fv(glGetUniformLocation(prog.prog, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(prog.prog, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(prog.prog, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glClearColor(0.7f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glfwSwapBuffers(window);
    }
    glfwTerminate();
    return 0;
}