#ifndef RENDERER_H
#define RENDERER_H

#include <filesystem>
#include <ctime>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "renderer_debug.h"
#include "shader.h"
// #include "shader_compute.h"
#include "scene.h"
#include "player.h"
#include "model_ass.h"
#include "material_disney.h"
#include "light.h"
#include "crosshair.h"
#include "text.h"

class Renderer {
public:
    Renderer(){};
    ~Renderer(){};

    float get_time() { return static_cast<float>(glfwGetTime()); }
    bool open() { return !glfwWindowShouldClose(window); }

    bool init(int width, int height, const char* title) {
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

        // tell GLFW to capture our mouse
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        // glad: load all OpenGL function pointers
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return false;
        }
        // configure global opengl state
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        // SHADERS
        /*shader manager?*/ // build and compile our shader program
        /*shader manager?*/ if (!std::filesystem::exists("../resources/shaders/vertex.glsl") ||
        /*shader manager?*/     !std::filesystem::exists("../resources/shaders/fragment.glsl")) {
        /*shader manager?*/     std::cerr << "Shader files not found! Ensure they're in the 'shaders/' directory relative to the executable." << std::endl;
        /*shader manager?*/     return false;
        /*shader manager?*/ }
        /*shader manager?*/ our_shader.init("../resources/shaders/vertex.glsl", "../resources/shaders/fragment.glsl");
        /*shader manager?*/ our_shader.use(); // don't forget to activate/use the shader before setting uniforms!
        /*shader manager?*/ // either set it manually like so:
        // /*shader manager?*/ glUniform1i(glGetUniformLocation(our_shader.ID, "texture1"), .5);
        /*shader manager?*/ // or set it via the texture class
        // /*shader manager?*/ our_shader.setInt("texture2", .5);

        // weapon_shader2.init("../resources/shaders/weapon_v.glsl", "../resources/shaders/weapon_f.glsl");
        weapon_shader2.init("../resources/shaders/weapon2_v.glsl", "../resources/shaders/weapon2_f.glsl");
        // weapon_shader2.init("../resources/shaders/fres_v.glsl", "../resources/shaders/fres_f.glsl");

        debug_shader.init("../resources/shaders/debug_v.glsl", "../resources/shaders/debug_f.glsl");
        disney_shader.init("../resources/shaders/disney_v.glsl", "../resources/shaders/disney_f.glsl");

        //setup_buffers();

        deferred_shader.init("../resources/shaders/deferred_v.glsl", "../resources/shaders/deferred_f.glsl");
        deferred_lighting_shader.init("../resources/shaders/deferred_light_v.glsl", "../resources/shaders/deferred_light_f.glsl");

        debug_gbuffer_shader.init("../resources/shaders/deferred_light_v.glsl", "../resources/shaders/deferred_lighting_debug_f.glsl");
        
        deferred_shader.use();
        deferred_shader.setInt("texture_diffuse1", 0);
        deferred_shader.setInt("texture_specular1", 1);

        deferred_shader.use();
        deferred_shader.setInt("texture_diffuse1", 0);
        deferred_shader.setInt("texture_specular1", 1);

        deferred_lighting_shader.use();
        deferred_lighting_shader.setInt("g_position", 0);
        deferred_lighting_shader.setInt("g_normal", 1);
        deferred_lighting_shader.setInt("g_albedo_specular", 2);

        skybox_shader.init("../resources/shaders/skybox_v.glsl", "../resources/shaders/skybox_f.glsl");
        crosshair_shader.init("../resources/shaders/crosshair_v.glsl", "../resources/shaders/crosshair_f.glsl");
        hud_text_shader.init("../resources/shaders/text_hud_v.glsl", "../resources/shaders/text_hud_f.glsl");

        debug_renderer.init();

        return true;
    }

    void sync_callbacks(Player& player) {
        glfwSetWindowUserPointer(window, &player);
        glfwSetCursorPosCallback(window, Player::mouse_callback);
        glfwSetScrollCallback(window, Player::scroll_callback);
        glfwSetCharCallback(window, Player::char_callback);
    }

    bool setup_buffers() {
        // Create and bind G-buffer framebuffer
        glGenFramebuffers(1, &g_buffer);
        glBindFramebuffer(GL_FRAMEBUFFER, g_buffer);
    
        // 1. Position buffer
        glGenTextures(1, &g_position);
        glBindTexture(GL_TEXTURE_2D, g_position);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, scr_width, scr_height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, g_position, 0);
        
        // 2. Normal buffer
        glGenTextures(1, &g_normal);
        glBindTexture(GL_TEXTURE_2D, g_normal);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, scr_width, scr_height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, g_normal, 0);
        
        // 3. Color + specular buffer
        glGenTextures(1, &g_albedo_specular);
        glBindTexture(GL_TEXTURE_2D, g_albedo_specular);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, scr_width, scr_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, g_albedo_specular, 0);
        
        // Tell OpenGL which color attachments we'll use for rendering
        unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
        glDrawBuffers(3, attachments);
        
        // Create and attach depth buffer
        unsigned int rboDepth;
        glGenRenderbuffers(1, &rboDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, scr_width, scr_height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
        
        // Check if framebuffer is complete
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "Framebuffer not complete!" << std::endl;
            return false;
        }
        
        // Unbind framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        unsigned int quadVBO;
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        
        // Print debug info
        printf("Quad VAO created: %u\n", quadVAO);
        
        return true;
    }

    void draw_player_model(Player& player, Model_ass& player_model) {
        our_shader.use();

        our_shader.setVec3("lightPos", glm::vec3(2.0f, 2.0f, 2.0f));
        our_shader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));

        our_shader.setVec3("viewPos", player.camera.position);

        glm::mat4 projection = glm::perspective(glm::radians(player.camera.zoom), (float)scr_width / (float)scr_height, 0.1f, 300.0f);
        our_shader.setMat4("projection", projection);
        glm::mat4 view = player.camera.get_view_matrix();
        our_shader.setMat4("view", view);

        glm::mat4 model = player.get_model_matrix();
        our_shader.setMat4("model", model);
    
        our_shader.setVec3("objectColor", glm::vec3(0.0f, 0.5f, 0.0f));
    
        player_model.draw(our_shader);
    }

    void render_scene(Player& player, Scene& scene, float deltaTime) {
        // glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!
        // our_shader.use();

        // our_shader.setVec3("lightPos", glm::vec3(2.0f, 2.0f, 2.0f));
        // our_shader.setVec3("viewPos", player.camera.position);
        // our_shader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));

        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, texture1);
        // glActiveTexture(GL_TEXTURE1);
        // glBindTexture(GL_TEXTURE_2D, texture2);

        // glm::mat4 projection = glm::perspective(glm::radians(player.camera.zoom), (float)scr_width / (float)scr_height, 0.1f, 300.0f);
        // our_shader.setMat4("projection", projection);
        // glm::mat4 view = player.camera.get_view_matrix();
        // our_shader.setMat4("view", view);

        // for (Entity entity : scene.entities) {
        //     glm::mat4 model = entity.get_model_matrix();
        //     our_shader.setMat4("model", model);
        //     our_shader.setVec3("objectColor", entity.get_color());
        //     entity.draw(our_shader);
            
        //     debug_renderer.add_axes(entity.physics.position, entity.physics.orientation);
        // }

        // Clear the buffers
        glClearColor(0.2f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        disney_shader.use();
        
        disney_shader.setVec3("light_position", light.position);
        disney_shader.setVec3("light_color", light.color);
        disney_shader.setFloat("light_intensity", light.intensity);
        debug_renderer.add_sphere(light.position, 0.1f, light.color);

        // Setup camera matrices
        glm::mat4 projection = glm::perspective(glm::radians(player.camera.zoom), (float)scr_width / (float)scr_height, 0.1f, 300.0f);
        disney_shader.setMat4("projection", projection);
        
        glm::mat4 view = player.camera.get_view_matrix();
        disney_shader.setMat4("view", view);
        disney_shader.setVec3("view_position", player.camera.position);
        
        material.apply(disney_shader);
        for (Entity& entity : scene.entities) {
            // Calculate and set transformation matrices
            glm::mat4 model = entity.get_model_matrix();
            disney_shader.setMat4("model", model);
            
            // Calculate normal matrix (inverse transpose of the model matrix)
            glm::mat3 normal_matrix = glm::transpose(glm::inverse(glm::mat3(model)));
            disney_shader.setMat3("normal_matrix", normal_matrix);
            
            // Draw the entity
            entity.draw(disney_shader);
            
            debug_renderer.add_axes(entity.physics.position, entity.physics.orientation);
        }
        
        render_skybox(scene.skybox, view, projection);

        // flush(); !!
    }
    
    void render_debug(Player& player) {
        glm::mat4 projection = glm::perspective(glm::radians(player.camera.zoom), (float)scr_width / (float)scr_height, 0.1f, 300.0f);
        our_shader.setMat4("projection", projection);
        glm::mat4 view = player.camera.get_view_matrix();
        our_shader.setMat4("view", view);

        debug_renderer.render(debug_shader, projection, view);
    }

    void render_scene_deferred(Player& player, Scene& scene, float deltaTime) {
        // 1. Geometry Pass: Render scene to G-buffer
        glBindFramebuffer(GL_FRAMEBUFFER, g_buffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        
        // Prepare matrices
        glm::mat4 projection = glm::perspective(glm::radians(player.camera.zoom), 
                                                (float)scr_width / (float)scr_height, 
                                                0.1f, 300.0f);
        glm::mat4 view = player.camera.get_view_matrix();

        // Use deferred geometry shader for G-buffer pass
        deferred_shader.use();
        deferred_shader.setMat4("projection", projection);
        deferred_shader.setMat4("view", view);

        // Render scene entities to G-buffer
        for (Entity& entity : scene.entities) {
            glm::mat4 model = entity.get_model_matrix();
            deferred_shader.setMat4("model", model);
            
            // Calculate normal matrix (inverse transpose of the model matrix)
            glm::mat3 normal_matrix = glm::transpose(glm::inverse(glm::mat3(model)));
            deferred_shader.setMat3("normal_matrix", normal_matrix);
            
            entity.draw(deferred_shader);
        }

        // 2. Lighting Pass: Render lighting using G-buffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // Back to default framebuffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        
        // Use lighting shader
        deferred_lighting_shader.use();
        
        // Bind G-buffer textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, g_position);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, g_normal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, g_albedo_specular);

        // Set lighting uniforms
        deferred_lighting_shader.setVec3("viewPos", player.camera.position);
        
        // Set light parameters
        deferred_lighting_shader.setVec3("light.Position", light.position);
        deferred_lighting_shader.setVec3("light.Color", light.color);
        deferred_lighting_shader.setFloat("light.Linear", 0.09f);
        deferred_lighting_shader.setFloat("light.Quadratic", 0.032f);
        deferred_lighting_shader.setFloat("light.Intensity", light.intensity);

        // glUniform1i(glGetUniformLocation(deferred_lighting_shader.ID, "debug_mode"), 999);

        // Render a screen-filling quad
        // render_quad();
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
        // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glEnable(GL_DEPTH_TEST);

        // Optional: Render debug information
        if (!player.key_toggles[(unsigned)'r']) {
            // debug_renderer.render(debug_shader, projection, view);
            debug_visualize_gbuffer(player);
        }
    }

    void debug_visualize_gbuffer(Player& player) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
        debug_gbuffer_shader.use();
    
        // Bind G-buffer textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, g_position);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, g_normal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, g_albedo_specular);
    
        const char* mode_names[] = {
            "Position", "Normal", "Albedo", "Specular", "Depth"
        };
        static int current_mode = 0;
    
        for (int i = 0; i < 4; ++i) {
            debug_gbuffer_shader.setInt("debug_mode", i);
    
            int x = (i % 2) * (scr_width / 2);
            int y = (i / 2) * (scr_height / 2);
            glViewport(x, y, scr_width / 2, scr_height / 2);
    
            // render_quad();
            glBindVertexArray(quadVAO);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            glBindVertexArray(0);    
        }
    
        // Reset viewport
        glViewport(0, 0, scr_width, scr_height);
    }
    
    void draw_player_stuff(Player& player, glm::vec3& clr, glm::vec3& emis_clr, glm::vec3& fres_clr, float expon, const Skybox& skybox) {
        // glDisable(GL_DEPTH_TEST);
        weapon_shader2.use();
        
        glm::mat4 projection = glm::perspective(glm::radians(player.camera.zoom), (float)scr_width / (float)scr_height, 0.1f, 300.0f);
        weapon_shader2.setMat4("projection", projection);
        
        glm::mat4 fullView = player.camera.get_view_matrix();
        glm::mat4 rotationOnlyView = glm::mat4(glm::mat3(fullView));
        weapon_shader2.setMat4("view", rotationOnlyView);
        

        // REPLACE FROM HERE -------------------------------------------------------------
        glm::mat4 model = glm::mat4(1.0f);
        
        // FIX player.camera.yaw pitch roll maybe geeruc
        model = glm::rotate(model, -glm::radians(player.camera.yaw + 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(player.camera.pitch), glm::vec3(1.0f, 0.0f, 0.0f));
        
        model = glm::translate(model, player.controller->get_weapon_position());
        weapon_shader2.setMat4("model", model);
        weapon_shader2.setVec3("viewPos", player.camera.position);

        // HERE ------------------------------------
        // USE WEAPON + HANDS + QUATS + ANIMATION + OH GOD 

        // weapon_shader2.setVec3("lightPos", glm::vec3(5.0f, 5.0f, 5.0f));
        // float timeValue = glfwGetTime();
        // weapon_shader2.setFloat("time", timeValue);

        // // d) Wave settings
        // weapon_shader2.setFloat("time", timeValue);
        // weapon_shader2.setFloat("waveAmplitude", amp);
        // weapon_shader2.setFloat("waveFrequency", frq);
        // weapon_shader2.setFloat("waveSpeed", spd);

        // f) Base color & camera position
        weapon_shader2.setVec3("uColor", clr);
        // emiss 
        // sparkle
        weapon_shader2.setVec3("uEmission", emis_clr); // e.g. (0, 0.6, 1.0)
        weapon_shader2.setVec3("uFresnelColor", fres_clr);    // _Emission
        weapon_shader2.setFloat("uFresnelExponent", expon); // _FresnelExponent
        // wep.draw(weapon_shader2);
        weapon_shader2.setInt("skybox", 0);
        skybox.bind();
        player.controller->draw_hud(weapon_shader2);

        // glDepthMask(GL_TRUE);
        // glEnable(GL_DEPTH_TEST);
    }

    void render_skybox(const Skybox& skybox, const glm::mat4& view, const glm::mat4& projection) {
        glDepthFunc(GL_LEQUAL);
        skybox_shader.use();

        glm::mat4 viewNoTranslation = glm::mat4(glm::mat3(view));

        skybox_shader.setMat4("view", viewNoTranslation);
        skybox_shader.setMat4("projection", projection);

        skybox.draw();

        glDepthFunc(GL_LESS);
    }

    void draw_model_at(Model_ass& model, glm::vec3 pos) {

    }

    void render_crosshair(const Crosshair& crosshair) {
        crosshair_shader.use();
        crosshair.draw(crosshair_shader, scr_width, scr_height);
    }

    void render_hud_text(const Text& text) {
        glm::mat4 projection = glm::ortho(0.0f, (float)scr_width, 0.0f, (float)scr_height);
        
        glDisable(GL_DEPTH_TEST);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        text.draw(hud_text_shader, projection);
     
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
    }

    void debug_sphere_at(float x, float y, float z) {
        debug_renderer.add_sphere(glm::vec3(x, y, z), 1.0f, glm::vec3(1.0f, 1.0f, 0.0f));
    }
    void debug_sphere_at(glm::vec3 pos) {
        debug_renderer.add_sphere(pos, 1.0f, glm::vec3(1.0f, 1.0f, 0.0f));
    }
    
    void flush() {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    void shutdown() {
        glDeleteFramebuffers(1, &g_buffer);
        glDeleteTextures(1, &g_position);
        glDeleteTextures(1, &g_normal);
        glDeleteTextures(1, &g_albedo_specular);
        
        glDeleteVertexArrays(1, &quadVAO);
        
        glfwTerminate();
    }

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    }

// private:
    GLFWwindow* window;
    int scr_width, scr_height;
    Renderer_debug debug_renderer;

    Light light;

    Material_disney material;

    Shader our_shader, weapon_shader, weapon_shader2, debug_shader, disney_shader;
    Shader skybox_shader;
    Shader crosshair_shader;
    Shader hud_text_shader;

    // deferred pipeline
    Shader deferred_shader, deferred_lighting_shader, debug_gbuffer_shader;
    unsigned int g_buffer, g_position, g_normal, g_albedo_specular;
    
    unsigned int quadVAO;

};
#endif
