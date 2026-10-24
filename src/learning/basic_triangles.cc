#include <iostream>
#include <chrono>
#include <thread>
#include "glad/glad.h"
#include "GLFW/glfw3.h"

const char *vertex_shader = " #version 330 core\n"
                            "layout (location = 0) in vec3 aPos;\n"
                            "void main()\n"
                            "{\n"
                            "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1);"
                            "}\0";

const char *fragment_shader = "#version 330 core\n"
                              "out vec4 FragColor;\n"
                              "void main()\n"
                              "{\n"
                              "   FragColor = vec4(1.0, 0.5, 0.2, 1.0);\n"
                              "}\0";

void create_triangle_vao(const float *vertices, size_t size, unsigned int *const VAO)
{
    unsigned int VBO;
    glGenVertexArrays(1, VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(*VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
}

void create_square(unsigned int *VAO)
{
    unsigned int EBO;
    const unsigned int index[] = {
        0, 1, 3,
        1, 2, 3};
    glGenBuffers(1, &EBO);
    glBindVertexArray(*VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), &index, GL_STATIC_DRAW);
}

GLFWwindow *init()
{
    if (glfwInit())
    {
        std::cout << "GLFW init successful" << std::endl;
    }
    glfwSetErrorCallback([](int error, const char *description)
                         { std::cerr << "GLFW Error (" << error << "): " << description << std::endl; });
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "My Triangle", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Fail to create window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int width, int height)
                                   { glViewport(0, 0, width, height); });
    return window;
}

void create_program(const char *vertex_shader, const char *frag_shader, unsigned int *const program)
{
    unsigned int vertex_shader_obj = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader_obj, 1, &vertex_shader, NULL);
    glCompileShader(vertex_shader_obj);
    int success;
    char info[512];
    glGetShaderiv(vertex_shader_obj, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex_shader_obj, 512, NULL, info);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION::FAILED" << " " << info << std::endl;
    }

    unsigned int frag_shader_obj = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_shader_obj, 1, &frag_shader, NULL);
    glCompileShader(frag_shader_obj);
    glGetShaderiv(frag_shader_obj, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(frag_shader_obj, 512, NULL, info);
        std::cout << "ERROR::FRAG::VERTEX::COMPILATION::FAILED" << " " << info << std::endl;
    }

    *program = glCreateProgram();
    glAttachShader(*program, vertex_shader_obj);
    glAttachShader(*program, frag_shader_obj);
    glLinkProgram(*program);
    glDeleteShader(vertex_shader_obj);
    glDeleteShader(frag_shader_obj);
}

int main()
{
    GLFWwindow *window = init();
    if (window == nullptr)
    {
        std::cout << "Init failed" << std::endl;
        return -1;
    }

    float narrow_triangle_vertices[] = {
        0.2f, 0.0f, 0.0f,
        -0.2f, 0.0f, 0.0f,
        0.0f, 0.2f, 0.0f};

    float wider_triangle_vertices[] = {
        0.7f, 0.0f, 0.0f,
        -0.7f, 0.0f, 0.0f,
        0.0f, 0.7f, 0.0f};

    float narrow_square_vertices[] = {
        0.0f, 0.0f, 0.0f,
        0.0f, 0.3f, 0.0f,
        0.3f, 0.3f, 0.0f,
        0.3f, 0.0f, 0.0f};

    float wider_square_vertices[] = {
        0.0f, 0.0f, 0.0f,
        0.0f, 0.7f, 0.0f,
        0.7f, 0.7f, 0.0f,
        0.7f, 0.0f, 0.0f};

    // unsigned int narrow_triangle_voa;
    // create_triangle_vao(narrow_triangle_vertices, sizeof(narrow_triangle_vertices), &narrow_triangle_voa);
    // unsigned int wider_triangle_vao;
    // create_triangle_vao(wider_triangle_vertices, sizeof(wider_triangle_vertices), &wider_triangle_vao);
    // unsigned int main_program;
    // create_program(vertex_shader, fragment_shader, &main_program);

    unsigned int narrow_square_vao;
    create_triangle_vao(narrow_square_vertices, sizeof(narrow_square_vertices), &narrow_square_vao);
    unsigned int wider_square_vao;
    create_triangle_vao(wider_square_vertices, sizeof(wider_square_vertices), &wider_square_vao);
    create_square(&narrow_square_vao);
    create_square(&wider_square_vao);
    unsigned int main_program;
    create_program(vertex_shader, fragment_shader, &main_program);

    glUseProgram(main_program);

    int cur_triangle = 0;
    while (!glfwWindowShouldClose(window))
    {
        if (cur_triangle)
        {
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

            glBindVertexArray(wider_square_vao);
        }
        else
        {
            glClearColor(0.5f, 0.2f, 0.3f, 1.0f);

            glBindVertexArray(narrow_square_vao);
        }
        glClear(GL_COLOR_BUFFER_BIT);
        cur_triangle = 1 - cur_triangle;
        // glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}