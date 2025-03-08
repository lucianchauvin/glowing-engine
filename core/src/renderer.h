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

// floor parameters
const float GRID_SIZE = 10;
const float FLOOR_SIZE = GRID_SIZE * 4.0f;
const float FLOOR_Y = 0;

class Renderer {
public:
    Renderer(){};
    ~Renderer(){};

    float get_time() {
        return static_cast<float>(glfwGetTime());
    }

    bool open() {
        return !glfwWindowShouldClose(window);
    }

    bool init(int width, int height, const char* title, Controller player) {
        scr_width = width;
        scr_height = height;

        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // #ifdef __APPLE__
    //     glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    // #endif
    
        // glfw window creation
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
        // ---------------------------------------
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return false;
        }
        // configure global opengl state
        glEnable(GL_DEPTH_TEST);
        // SHADERS
        // SHADERS
        // SHADERS
        // build and compile our shader program
        if (!std::filesystem::exists("../resources/shaders/vertex.glsl") ||
            !std::filesystem::exists("../resources/shaders/fragment.glsl")) {
            std::cerr << "Shader files not found! Ensure they're in the 'shaders/' directory relative to the executable." << std::endl;
            return false;
        }
        ourShader.init("../resources/shaders/vertex.glsl", "../resources/shaders/fragment.glsl");
        ourShader.use(); // don't forget to activate/use the shader before setting uniforms!
        // either set it manually like so:
        glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0);
        // or set it via the texture class
        ourShader.setInt("texture2", 1);
        color_location = glGetUniformLocation(ourShader.ID, "customColor");


        // TODO move to real spot
        float vertices[] = {
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
             0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        };
        initrandom_cubes();
        float* floor_vertices = generate_floor_vertices();
        // create a single VBO for both cube and floor
        
        cube_vertices_size = sizeof(vertices);
        floor_verticesSize = 30 * sizeof(float); // 6 vertices * 5 components
        // combine vertices
        combined_vertices = new float[(cube_vertices_size + floor_verticesSize) / sizeof(float)];
        memcpy(combined_vertices, vertices, cube_vertices_size);
        memcpy(combined_vertices + cube_vertices_size / sizeof(float), floor_vertices, floor_verticesSize);
        
        setup_buffers();
        load_textures();
        return true;
    }

    bool setup_buffers() {
        // le cube
        // int vao vbo
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, cube_vertices_size + floor_verticesSize, combined_vertices, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        //   o
        // o   o
        //   o
        // int vao vbo
        // generate sphere mesh
        generateSphere(0.5f, 16, 16);  // radius, rings, sectors
        // sphere vao setup - use separate VAO/VBO/EBO for the sphere
        glGenVertexArrays(1, &sphereVAO);
        glGenBuffers(1, &sphereVBO);
        glGenBuffers(1, &sphereEBO);
    
        glBindVertexArray(sphereVAO);
            
        glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
        glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), &sphereVertices[0], GL_STATIC_DRAW);
            
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(unsigned int), &sphereIndices[0], GL_STATIC_DRAW);
            
        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        // important: restore the original VAO binding after setting up the sphere
        glBindVertexArray(VAO);

        return true;
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

    void render_scene_2(Controller player, Scene scene) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!
        ourShader.use();

        ourShader.setVec3("lightPos", glm::vec3(2.0f, 2.0f, 2.0f));  // Light source position
        ourShader.setVec3("viewPos", player.camera.position);  // Camera position
        ourShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f)); // White light
        ourShader.setVec3("objectColor", glm::vec3(1.0f, 1.0f, 1.0f)); // Default object color

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        glm::mat4 projection = glm::perspective(glm::radians(player.camera.Zoom), (float)scr_width / (float)scr_height, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);
        glm::mat4 view = player.camera.GetViewMatrix();
        ourShader.setMat4("view", view);

        for (Entity entity : scene.entites) {
            glm::mat4 model = entity.get_model_matrix();
            ourShader.setVec3("customColor", entity.get_color());
            ourShader.setMat4("model", model);
            entity.draw(ourShader);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    void render_scene(Controller player) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!
        // activate shader
        ourShader.use();
        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        // need player info
        glUniform3f(color_location, 1.0f, 1.0f, 1.0f);
        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(player.camera.Zoom), (float)scr_width / (float)scr_height, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);
        // camera/view transformation
        glm::mat4 view = player.camera.GetViewMatrix();
        ourShader.setMat4("view", view);

        // draw floor
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTexture);

        glm::mat4 model = glm::mat4(1.0f);
        ourShader.setMat4("model", model);
        glUniform3f(color_location, 1.0f, 1.0f, 1.0f); // White color for floor
        // draw the floor (6 vertices after all the cube vertices)
        glDrawArrays(GL_TRIANGLES, 36, 6);

        // draw random cubes
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        
        for (int i = 0; i < NUM_RANDOM_CUBES; i++) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, random_cubes[i].position);
            model = glm::scale(model, glm::vec3(random_cubes[i].scale));
            ourShader.setMat4("model", model);
            
            // use custom color for random cubes
            glUniform3f(color_location, 
                random_cubes[i].color.r, 
                random_cubes[i].color.g, 
                random_cubes[i].color.b);
            
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        if (player.is_third_person) {
            // explicitly bind the sphere VAO
            glBindVertexArray(sphereVAO);
            
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture1);
            
            ourShader.setMat4("model", glm::translate(glm::mat4(1.0f), player.player_physics.player_position));
            glUniform3f(color_location, 1.0f, 0.2f, 0.2f);
            glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);

            glm::vec3 pos = glm::vec3(player.player_physics.player_position);
            pos.y += player.PLAYER_HEIGHT;
            ourShader.setMat4("model", glm::scale(glm::translate(glm::mat4(1.0f), pos), glm::vec3(0.25f, 0.25f, 0.25f)));
            glUniform3f(color_location, 1.0f, 0.2f, 0.2f);
            glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);

            glBindVertexArray(VAO);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    void shutdown() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    
        glDeleteVertexArrays(1, &sphereVAO);
        glDeleteBuffers(1, &sphereVBO);
        glDeleteBuffers(1, &sphereEBO);

        // TODO MOVE
        delete[] random_cubes;
        delete[] floor_vertices;
        delete[] combined_vertices;

        glfwTerminate();
    }

    GLFWwindow* get_window() const { return window; }

// private:
    GLFWwindow* window;
    int scr_width, scr_height;

    Shader ourShader;
    unsigned int VBO, VAO;
    unsigned int sphereVAO = 0, sphereVBO = 0, sphereEBO = 0;    
    unsigned int texture1, texture2, floorTexture;
    int color_location;

    float* combined_vertices;
    std::vector<float> sphereVertices;
    std::vector<unsigned int> sphereIndices;
    float* floor_vertices;
    size_t cube_vertices_size;
    size_t floor_verticesSize;
    
    const int NUM_RANDOM_CUBES = 20;
    struct RandomCube {
        glm::vec3 position;
        glm::vec3 color;
        float scale;
    };
    RandomCube* random_cubes = nullptr;

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    }

    // TODO move to scene
    void generateSphere(float radius, unsigned int rings, unsigned int sectors) {
        sphereVertices.clear();
        sphereIndices.clear();
        
        float const R = 1.0f/(float)(rings-1);
        float const S = 1.0f/(float)(sectors-1);
        
        for(unsigned int r = 0; r < rings; r++) {
            for(unsigned int s = 0; s < sectors; s++) {
                float const y = sin(-M_PI_2 + M_PI * r * R);
                float const x = cos(2*M_PI * s * S) * sin(M_PI * r * R);
                float const z = sin(2*M_PI * s * S) * sin(M_PI * r * R);
                
                // position
                sphereVertices.push_back(x * radius);
                sphereVertices.push_back(y * radius);
                sphereVertices.push_back(z * radius);
                
                // texture coordinates
                sphereVertices.push_back(s*S);
                sphereVertices.push_back(r*R);
                
                // add indices
                if(r < rings-1 && s < sectors-1) {
                    unsigned int curRow = r * sectors;
                    unsigned int nextRow = (r + 1) * sectors;
                    
                    sphereIndices.push_back(curRow + s);
                    sphereIndices.push_back(nextRow + s);
                    sphereIndices.push_back(nextRow + s + 1);
                    
                    sphereIndices.push_back(curRow + s);
                    sphereIndices.push_back(nextRow + s + 1);
                    sphereIndices.push_back(curRow + s + 1);
                }
            }
        }
    }

    float* generate_floor_vertices() {
        floor_vertices = new float[30]; // 6 vertices * 5 components (position + tex coords)
        // floor vertices (position, texture coords)
        float temp[] = {
            -FLOOR_SIZE/2, FLOOR_Y, -FLOOR_SIZE/2,  0.0f, 0.0f,
            FLOOR_SIZE/2, FLOOR_Y, -FLOOR_SIZE/2,  FLOOR_SIZE, 0.0f,
            FLOOR_SIZE/2, FLOOR_Y,  FLOOR_SIZE/2,  FLOOR_SIZE, FLOOR_SIZE,
            FLOOR_SIZE/2, FLOOR_Y,  FLOOR_SIZE/2,  FLOOR_SIZE, FLOOR_SIZE,
            -FLOOR_SIZE/2, FLOOR_Y,  FLOOR_SIZE/2,  0.0f, FLOOR_SIZE,
            -FLOOR_SIZE/2, FLOOR_Y, -FLOOR_SIZE/2,  0.0f, 0.0f
        };
        
        memcpy(floor_vertices, temp, 30 * sizeof(float));
        return floor_vertices;
    }

    void initrandom_cubes() {
        std::mt19937 rng(static_cast<unsigned int>(time(nullptr)));
        std::uniform_real_distribution<float> pos_dist_x(-FLOOR_SIZE/2 + 1.0f, FLOOR_SIZE/2 - 1.0f);
        std::uniform_real_distribution<float> pos_dist_z(-FLOOR_SIZE/2 + 1.0f, FLOOR_SIZE/2 - 1.0f);
        std::uniform_real_distribution<float> color_dist(0.0f, 1.0f);
        std::uniform_real_distribution<float> scale_dist(0.5f, 2.0f);
        
        random_cubes = new RandomCube[NUM_RANDOM_CUBES];
        
        for (int i = 0; i < NUM_RANDOM_CUBES; i++) {
            random_cubes[i].position = glm::vec3(
                pos_dist_x(rng),
                0 + scale_dist(rng) / 2.0f, // place on top of floor
                pos_dist_z(rng)
            );
            random_cubes[i].color = glm::vec3(
                color_dist(rng),
                color_dist(rng),
                color_dist(rng)
            );
            random_cubes[i].scale = scale_dist(rng);
        }
    }
};

#endif