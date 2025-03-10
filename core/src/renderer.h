#ifndef RENDERER_H
#define RENDERER_H

#include <map>
#include <vector>
#include <filesystem>
#include <random>
#include <ctime>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "shader.h"
#include "scene.h"
#include "controller.h"
#include "chunk.h"

class Renderer {
public:
    Renderer(){};
    ~Renderer(){};

    float get_time() { return static_cast<float>(glfwGetTime()); }

    bool open() { return !glfwWindowShouldClose(window); }

    bool init(int width, int height, const char* title, Controller& player) {
        scr_width = width;
        scr_height = height;

        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow(width, height, title, NULL, NULL);
        if (window == NULL) {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return false;
        } else {
            std::cout << "GLFW window created successfully" << std::endl;
        }

        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        glfwSetWindowUserPointer(window, &player);
        glfwSetCursorPosCallback(window, Controller::mouse_callback);
        glfwSetScrollCallback(window, Controller::scroll_callback);
        glfwSetCharCallback(window, Controller::char_callback);
        // tell GLFW to capture our mouse
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        // glad: load all OpenGL function pointers
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return false;
        }
        // configure global opengl state
        glEnable(GL_DEPTH_TEST);
        // glEnable(GL_CULL_FACE);
        // SHADERS
        /*shader manager?*/ // build and compile our shader program
        /*shader manager?*/ if (!std::filesystem::exists("../resources/shaders/vertex.glsl") ||
        /*shader manager?*/     !std::filesystem::exists("../resources/shaders/fragment.glsl")) {
        /*shader manager?*/     std::cerr << "Shader files not found! Ensure they're in the 'shaders/' directory relative to the executable." << std::endl;
        /*shader manager?*/     return false;
        /*shader manager?*/ }
        /*shader manager?*/ ourShader.init("../resources/shaders/vertex.glsl", "../resources/shaders/fragment.glsl");
        /*shader manager?*/ ourShader.use(); // don't forget to activate/use the shader before setting uniforms!
        /*shader manager?*/ // either set it manually like so:
        /*shader manager?*/ glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0);
        /*shader manager?*/ // or set it via the texture class
        /*shader manager?*/ ourShader.setInt("texture2", 1);

        setup_buffers();
        load_textures();
        return true;
    }

    bool setup_buffers() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        return true; // ?
    }

    bool load_textures() {
        // texture 1
        glGenTextures(1, &texture1);
        glBindTexture(GL_TEXTURE_2D, texture1); 
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // load image, create texture and generate mipmaps
        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
        unsigned char *data = stbi_load("../resources/textures/container.jpg", &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);
        // texture 2
        glGenTextures(1, &texture2);
        glBindTexture(GL_TEXTURE_2D, texture2);
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // load image, create texture and generate mipmaps
        data = stbi_load("../resources/textures/awesomeface.png", &width, &height, &nrChannels, 0);
        if (data) {
            // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);
        // floor texture
        glGenTextures(1, &floorTexture);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // load image, create texture and generate mipmaps
        data = stbi_load("../resources/textures/floor.jpg", &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else {
            std::cout << "Failed to load floor texture, using container texture instead" << std::endl;
            // Fallback to container texture if floor texture is missing
            floorTexture = texture1;
        }
        stbi_image_free(data);

        return true;
    }

    void render_scene(Controller player, Scene& scene, float deltaTime, std::vector<Chunk*> chunks) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!
        ourShader.use();

        ourShader.setVec3("lightPos", glm::vec3(2.0f, 2.0f, 2.0f));
        ourShader.setVec3("viewPos", player.camera.position);
        ourShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        glm::mat4 projection = glm::perspective(glm::radians(player.camera.zoom), (float)scr_width / (float)scr_height, 0.1f, 300.0f);
        ourShader.setMat4("projection", projection);
        glm::mat4 view = player.camera.get_view_matrix();
        ourShader.setMat4("view", view);

        for (Entity entity : scene.entities) {
            glm::mat4 model = entity.get_model_matrix();
            ourShader.setMat4("model", model);
            ourShader.setVec3("objectColor", entity.get_color());
            entity.draw(ourShader);
        }

        ourShader.setVec3("lightPos", glm::vec3(0.0f, 100.0f, 0.0f));
        ourShader.setVec3("viewPos", player.camera.position);
        ourShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        std::vector<int> idxs;
        int i = 0;
        for (Entity& entity : scene.timed_entities) {
            // printf("[BEFORE] %f\n", entity.ttl);
            glm::mat4 model = entity.get_model_matrix();
            ourShader.setMat4("model", model);
            ourShader.setVec3("objectColor", glm::vec3(1.0f, 0.0f, 1.0f) * (entity.ttl / entity.max_ttl));
            
            if (!entity.draw(ourShader, deltaTime)) {
                // printf("REMOVE!!\n");
                idxs.push_back(i);
            }
            
            // printf("[AFTER] %f\n", entity.ttl);
            i++;
        }
        
        for (int j = idxs.size() - 1; j >= 0; j--) {
            scene.timed_entities.erase(scene.timed_entities.begin() + idxs[j]);
        }

        // chunkj stuff
        float translate_height = 0.0f;
        for (Chunk* c : chunks) {
            if (c->in_chunk(player.player_physics.player_position)) {
                translate_height = c->height_at(player.player_physics.player_position);
                glm::ivec2 xz = c->chunk_pos_chunk();
                printf("player in chunk: [%d, %d]\n", xz.x, xz.y);
                printf("height: [%f]", translate_height);
                break;
            }
        } 

        ourShader.setVec3("lightPos", glm::vec3(0.0f, 100.0f, 0.0f));
        ourShader.setVec3("viewPos", player.camera.position);
        ourShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        ourShader.setVec3("objectColor", glm::vec3(1.0f, 0.0f, 1.0f));
        for (Chunk* c : chunks) {
            // printf("drawing chunk\n");
            glm::mat4 model = glm::mat4(1.0f);
            glm::vec2 chunk_pos = c->chunk_pos_world();
            model = glm::translate(model, glm::vec3(chunk_pos.x, -2.0f - translate_height, chunk_pos.y));
            ourShader.setMat4("model", model);
            c->draw(ourShader);
        }

        // flush(); !!
    }

    void flush() {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    void shutdown() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glfwTerminate();
    }

// private:
    GLFWwindow* window;
    int scr_width, scr_height;

    Shader ourShader;
    unsigned int VBO, VAO;
    unsigned int color_location;
    unsigned int texture1, texture2, floorTexture;

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    }
};

#endif