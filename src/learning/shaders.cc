#include <iostream>
#include <fstream>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class shader
{
    std::ifstream shader_file;
    std::string source;

public:
    explicit shader(const std::string &file)
    {
        shader_file.open(file);
    }
    const char *get_shader_source()
    {
        std::string source_line;
        while (std::getline(shader_file, source_line))
        {
            if (source != "")
            {
                source += "\n";
            }
            source += source_line;
        }
        return source.c_str();
    }
    ~shader()
    {
        shader_file.close();
    }
};

void create_triangle_vao(const float *vertices, size_t sz, unsigned int *VAO)
{
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, VAO);
    glBindVertexArray(*VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sz, vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void create_program(const char *vertex_shader, const char *frag_shader, unsigned int *program)
{
    int success;
    char info[512];
    unsigned int vertex_shader_obj = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader_obj, 1, &vertex_shader, NULL);
    glCompileShader(vertex_shader_obj);
    glGetShaderiv(vertex_shader_obj, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex_shader_obj, 512, NULL, info);
        std::cout << "FAILED::COMPILATION::VERTEX::SHADER" << " " << info << std::endl;
    }

    unsigned int frag_shader_obj = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_shader_obj, 1, &frag_shader, NULL);
    glCompileShader(frag_shader_obj);

    glGetShaderiv(frag_shader_obj, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(frag_shader_obj, 512, NULL, info);
        std::cout << "FAILED::COMPILATION::FRAGMENT::SHADER" << " " << info << std::endl;
    }

    *program = glCreateProgram();
    glAttachShader(*program, vertex_shader_obj);
    glAttachShader(*program, frag_shader_obj);
    glLinkProgram(*program);
    glDeleteShader(vertex_shader_obj);
    glDeleteShader(frag_shader_obj);
}

GLFWwindow *init()
{
    if (!glfwInit())
    {
        std::cout << "GLFW Init failed";
        return nullptr;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "Shaders tutorial", NULL, NULL);
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Fail to init opengl Pointers" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *_, int width, int height)
                                   { glViewport(0, 0, width, height); });
    return window;
}

int main()
{
    GLFWwindow *window = init();
    if (window == nullptr)
    {
        std::cout << "Init failed" << std::endl;
        return -1;
    }

    shader vertex_shader("./shader.vsh");
    shader frag_shader("./shader.fsh");
    glClearColor(0.3f, 0.5f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    unsigned int program;
    create_program(vertex_shader.get_shader_source(), frag_shader.get_shader_source(), &program);

    unsigned int VAO;
    const float vertices[] = {
        0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f};
    create_triangle_vao(vertices, sizeof(vertices), &VAO);
    glBindVertexArray(VAO);
    glUseProgram(program);
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.3f, 0.5f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}