// #include <glm/glm.hpp>
// #include <glad/glad.h>

// #include <camera.h>

// namespace Compute {

//     constexpr unsigned int gridSizeX = 12;
//     constexpr unsigned int gridSizeY = 12;
//     constexpr unsigned int gridSizeZ = 24;
//     constexpr unsigned int numClusters = gridSizeX * gridSizeY * gridSizeZ;

//     struct alignas(16) Cluster {
//         glm::vec4 minPoint;
//         glm::vec4 maxPoint;
//         unsigned int count;
//         unsigned int lightIndices[100];
//     };

//     unsigned int clusterGridSSBO;

//     void init_ssbos() {
//         // clusterGridSSBO
//         glGenBuffers(1, &clusterGridSSBO);
//         glBindBuffer(GL_SHADER_STORAGE_BUFFER, clusterGridSSBO);

//         // NOTE: we only need to allocate memory. No need for initialization because
//         // comp shader builds the AABBs.
//         glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Cluster) * numClusters,
//                     nullptr, GL_STATIC_COPY);
//         glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, clusterGridSSBO);
//     }

//     Shader clusterComp;

//     void cull_lights_compute(const Camera &camera) {
//         auto [width, height] = Core::get_framebuffer_size();

//         // build AABBs every frame
//         clusterComp.use();
//         clusterComp.set_float("zNear", 0.1f);
//         clusterComp.set_float("zFar", 300.0f);

//         glm::mat4 projection = glm::perspective(glm::radians(player.camera.zoom), (float)scr_width / (float)scr_height, 0.1f, 300.0f);
        
//         clusterComp.set_mat4("inverseProjection", glm::inverse(camera.projection));
//         clusterComp.set_uvec3("gridSize", {gridSizeX, gridSizeY, gridSizeZ});
//         clusterComp.set_uvec2("screenDimensions", {width, height});

//         glDispatchCompute(gridSizeX, gridSizeY, gridSizeZ);
//         glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
//     }

//     void init() {
//         init_ssbos();
//         // load shaders
//         clusterComp = Shader("clusterShader.comp");
//     }
// }

#ifndef SHADER_COMPUTE_H
#define SHADER_COMPUTE_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader_compute {
public:
    unsigned int ID;

    Shader_compute() : ID(0) { }
    
    bool init(const char* computePath) {
        // 1. retrieve the compute shader source code from filePath
        std::string computeCode;
        std::ifstream cShaderFile;
        // ensure ifstream objects can throw exceptions:
        cShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            // open file
            cShaderFile.open(computePath);
            std::stringstream cShaderStream;
            // read file's buffer contents into stream
            cShaderStream << cShaderFile.rdbuf();
            // close file handler
            cShaderFile.close();
            // convert stream into string
            computeCode = cShaderStream.str();
        }
        catch (std::ifstream::failure& e) {
            std::cout << "ERROR::COMPUTE_SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
            return false;
        }
        const char* cShaderCode = computeCode.c_str();
        
        // 2. compile shader
        unsigned int compute;
        // compute shader
        compute = glCreateShader(GL_COMPUTE_SHADER);
        glShaderSource(compute, 1, &cShaderCode, NULL);
        glCompileShader(compute);
        if (!checkCompileErrors(compute, "COMPUTE")) {
            return false;
        }
        
        // shader Program
        ID = glCreateProgram();
        glAttachShader(ID, compute);
        glLinkProgram(ID);
        if (!checkCompileErrors(ID, "PROGRAM")) {
            return false;
        }
        
        // delete the shader as it's linked into our program now and no longer necessary
        glDeleteShader(compute);
        
        return true;
    }
    
    // activate the compute shader
    void use() const {
        glUseProgram(ID);
    }
    
    // dispatch compute shader with specified work group counts
    void dispatch(unsigned int numGroupsX, unsigned int numGroupsY, unsigned int numGroupsZ) const {
        glDispatchCompute(numGroupsX, numGroupsY, numGroupsZ);
    }
    
    // memory barrier to ensure compute shader writes are visible
    void memoryBarrier(GLbitfield barriers = GL_ALL_BARRIER_BITS) const {
        glMemoryBarrier(barriers);
    }
    
    // utility uniform functions
    void setBool(const std::string &name, bool value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    
    void setInt(const std::string &name, int value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    
    void setFloat(const std::string &name, float value) const {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    
    void setVec2(const std::string &name, const glm::vec2 &value) const {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    
    void setVec2(const std::string &name, float x, float y) const {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }
    
    void setVec3(const std::string &name, const glm::vec3 &value) const {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    
    void setVec3(const std::string &name, float x, float y, float z) const {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
    
    void setVec4(const std::string &name, const glm::vec4 &value) const {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    
    void setVec4(const std::string &name, float x, float y, float z, float w) const {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }
    
    void setMat2(const std::string &name, const glm::mat2 &mat) const {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    
    void setMat3(const std::string &name, const glm::mat3 &mat) const {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    
    void setMat4(const std::string &name, const glm::mat4 &mat) const {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

private:
    // utility function for checking shader compilation/linking errors
    bool checkCompileErrors(GLuint shader, std::string type) {
        GLint success;
        GLchar infoLog[1024];
        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
                return false;
            }
        }
        else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
                return false;
            }
        }
        return true;
    }
};
#endif