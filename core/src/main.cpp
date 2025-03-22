#include <dearimgui/imgui.h>
#include <dearimgui/imgui_impl_glfw.h>
#include <dearimgui/imgui_impl_opengl3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

#include "renderer.h"
#include "controller.h"
#include "shader.h"
#include "entity.h"
#include "scene.h"
#include "chunk.h"
#include "physics.h"

// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 800;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

int main() {
    Controller player;
    Renderer renderer;
    if (!renderer.init(SCR_WIDTH, SCR_HEIGHT, "GLOW", player)) {
        return -1;
    };

    Scene scene;
    Model plane;
    plane.load_mesh("../resources/models/plane.obj");
    Model sphere;
    sphere.load_mesh("../resources/models/bunny.obj");

    Model ak47;
    ak47.load_mesh("../resources/models/Ak_47/ak47.obj");

    
    for (int i = -5; i < 5; i++) {
        for (int j = 0; j < 10; j++) {
            // for (int k = 0; k < 10; k++) {
                glm::vec3 pos   = glm::vec3(2.0f * i, j * 1.0f, -2.0f * j); 
                glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
                glm::vec3 color = glm::vec3(0.1f * i, 0.1f * j, 0.1f);
                Entity e(&sphere, pos, true, scale, color);
                scene.include(e);
            // }
        }
    }

    // for (int i = 0; i < 10; i++) {
    //     glm::vec3 pos   = glm::vec3(2.0f * i - 5, 2.0f, 0.0f); 
    //     glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
    //     glm::vec3 color = glm::vec3(.05f * i, 0.5f, 0.2f);
    //     Entity e(&sphere, pos, scale, color, true, 3.0f);
    //     scene.include(e);
    // }

    glm::vec3 pos   = glm::vec3(0.0f, 0.0f, 0.0f); 
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 color = glm::vec3(0.7f, 0.7f, 0.7f);
    Entity e(&plane, pos, false, scale, color);
    scene.include(e);

    std::vector<Chunk*> chunks = std::vector<Chunk*>();

    for (int x = -2; x < 30; x++) {
        for (int z = -2; z < 30; z++) {
            // if (x == 0 && z == 0) continue;
            Chunk* chunk = new Chunk(x, z);
            chunks.push_back(chunk);
        }
    }

    Physics physics;
    physics.load_scene(scene);
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(renderer.window, true); // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();

    // render loop
    int frame = 0;
    printf("RENDERING\n");
    while (renderer.open()) {
        // printf("[%d]", frame++);
        float currentFrame = renderer.get_time();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // input
        player.process_input(renderer.window, deltaTime, scene, &sphere);
        // update player physics
        player.update_player_physics(deltaTime);
        if (player.player_physics.dashing) {
            Entity e(&sphere, player.player_physics.player_position, true, glm::vec3(0.1f), glm::vec3(0.0f, 0.3f, 0.2f), true, 5.0f);
            scene.include(e);
        }
        // render scene
        renderer.render_scene(player, scene, deltaTime, chunks);
        if (!player.key_toggles[(unsigned) 'r'])
            renderer.render_world_geometry(scene, player);

        physics.step(deltaTime);

        // render scene
        renderer.render_scene(player, scene, deltaTime, chunks);
        if (!player.key_toggles[(unsigned) 'r'])
            renderer.render_world_geometry(scene, player);

        renderer.draw_player_holding(player, &ak47);
        // if (player.key_toggles[(unsigned) 'r'])
        //     renderer.render_world_geometry(scene, player);

        // // gui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();        
        ImGui::Begin("Controls");
        ImGui::Text("FPS: %.1f", 1.0f / deltaTime);
        ImGui::Text("Position: (%.1f, %.1f, %.1f)", player.player_physics.player_position.x, player.player_physics.player_position.y, player.player_physics.player_position.z);
        ImGui::Text("Camera Position: (%.1f, %.1f, %.1f)", player.camera.position.x, player.camera.position.y, player.camera.position.z);
        ImGui::Text("Facing: (%.1f, %.1f, %.1f)", player.camera.front.x, player.camera.front.y, player.camera.front.z);
        ImGui::Text("Velocity: (%.1f, %.1f, %.1f)", player.player_physics.velocity.x, player.player_physics.velocity.y, player.player_physics.velocity.z);
        ImGui::Text("On Ground: %s", player.player_physics.isOnGround ? "Yes" : "No");
        ImGui::Text("drawing geom: %s", !player.key_toggles[(unsigned) 'r'] ? "Yes" : "No");
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        renderer.flush();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    renderer.shutdown();
    return 0;
}
