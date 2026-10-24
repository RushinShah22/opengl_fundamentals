//
// Created by Rushin Shah on 24/10/26.
//

#ifndef OPENGL_SHADER_H
#define OPENGL_SHADER_H
#include <string>
#include <glad/glad.h>
#include <fstream>

class Shader {
public:
    unsigned int ID;
    inline int use() const noexcept;
    Shader(const std::string& vert_shader_loc, const std::string& frag_shader_loc);
    inline void set_int(GLint loc, int val);
private:
    std::ifstream vert_shader_file, frag_shader_file;
    std::string vert_shader_src, frag_shader_src;
    bool is_program_successfully_created;
    inline int read_shader_src(std::ifstream* file, std::string* src) noexcept;
    inline int compile_shader(const std::string* src, GLuint shader_type, GLuint * shader) const noexcept;
    inline void create_program() noexcept;
};
#endif //OPENGL_SHADER_H
