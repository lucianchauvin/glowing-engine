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
#include "model.h"
#include "model_ass.h"

// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 800;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

float waveAmplitude = 0.03f;
float waveFrequency = 5.0f;
float waveSpeed = 2.0f;

glm::vec3 clr = glm::vec3(0.0f, 0.4f, 0.0f);
glm::vec3 emis = glm::vec3(0.0f);
glm::vec3 fres = glm::vec3(0.0f, 0.0f, 0.0f);
float expon = 1.0f;

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

    // Model ak47;
    // ak47.load_mesh("../resources/models/Ak_47/ak47.obj");

    // Model_ass model_ass("../resources/models/sword/scene.gltf");
    // Model_ass model_ass("../resources/models/gun/scene.gltf");
    // Model_ass model_ass("../resources/models/grn_sword/scene.gltf");
    // Model_ass model_ass("../resources/models/arms/scene.gltf");
    // Model_ass model_ass("../resources/models/shield/scene.obj");
    // Model_ass model_ass("../resources/models/backpack/backpack.obj");
    Model_ass holding("../resources/models/qbz/qbz.obj");
    Model_ass fly("../resources/models/plane/scene.gltf");
    // Model_ass model_ass("../resources/models/ebonchill/scene.gltf");
    // Model_ass model_ass("../resources/models/hogwarts/Hogwarts.obj");

    // for (int i = -5; i < 5; i++) {
    //     for (int j = 0; j < 10; j++) {
    //         // for (int k = 0; k < 10; k++) {
    //             glm::vec3 pos   = glm::vec3(2.0f * i, j * 1.0f, -2.0f * j); 
    //             glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
    //             glm::vec3 color = glm::vec3(0.1f * i, 0.1f * j, 0.1f);
    //             Entity e(&sphere, pos, true, scale, color);
    //             scene.include(e);
    //         // }
    //     }
    // }

    glm::vec3 pos   = glm::vec3(0.0f, 0.0f, 0.0f); 
    glm::vec3 scale = glm::vec3(10.0f, 1.0f, 10.0f);
    glm::vec3 color = glm::vec3(0.7f, 0.7f, 0.7f);
    Entity e(&plane, pos, false, scale, color);
    scene.include(e);

    std::vector<Chunk*> chunks = std::vector<Chunk*>();
    // for (int x = -2; x < 30; x++) {
    //     for (int z = -2; z < 30; z++) {
    //         // if (x == 0 && z == 0) continue;
    //         Chunk* chunk = new Chunk(x, z);
    //         chunks.push_back(chunk);
    //     }
    // }

    Physics physics;
    physics.load_scene(scene);
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    // ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(renderer.window, true); // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();

    // render loop
    // int frame = 0;
    printf("RENDERING\n");
    // uint step = 0;
    while (renderer.open()) {
		// ++step;        // printf("[%d]", frame++);
        float currentFrame = renderer.get_time();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // input
        player.process_input(renderer.window, deltaTime, scene, &sphere);
        player.process_input_plane(renderer.window, deltaTime, scene, &sphere);
		// update player physics
        player.update_player_physics(deltaTime);
		// player.update_plane_physics(deltaTime);		
        if (player.player_physics.dashing) {
            Entity e(&sphere, player.player_physics.position, true, glm::vec3(0.1f), glm::vec3(0.0f, 0.3f, 0.2f), true, 5.0f);
            scene.include(e);
        }
        
        // physics.step(deltaTime);
        printf("here\n");

        // render scene
        renderer.render_scene(player, scene, deltaTime, chunks);
        if (!player.key_toggles[(unsigned) 'r'])
            renderer.render_world_geometry(scene, player);
		renderer.draw_player_model(player, fly);
        // renderer.render_ass(player, model_ass);
        renderer.draw_player_holding(player, holding, clr, emis, fres, expon);
        
        // if (player.key_toggles[(unsigned) 'r'])
        //     renderer.render_world_geometry(scene, player);

        // // gui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();        
        ImGui::Begin("Controls");
        ImGui::Text("FPS: %.1f", 1.0f / deltaTime);
        ImGui::Text("Position: (%.1f, %.1f, %.1f)", player.player_physics.position.x, player.player_physics.position.y, player.player_physics.position.z);
        ImGui::Text("Camera Position: (%.1f, %.1f, %.1f)", player.camera.position.x, player.camera.position.y, player.camera.position.z);
        ImGui::Text("Facing: (%.1f, %.1f, %.1f)", player.camera.front.x, player.camera.front.y, player.camera.front.z);
        ImGui::Text("Velocity: (%.1f, %.1f, %.1f)", player.player_physics.velocity.x, player.player_physics.velocity.y, player.player_physics.velocity.z);
        ImGui::Text("On Ground: %s", player.player_physics.isOnGround ? "Yes" : "No");
        ImGui::Text("drawing geom: %s", !player.key_toggles[(unsigned) 'r'] ? "Yes" : "No");
        // glm::vec3 wep_pos(0.0f, 0.0f, 0.0f);
        // glm::vec3 wep_rot(0.0f, 0.0f, 0.0f);
    //     ImGui::SliderFloat3("wep offset", &player.wep_pos.x, -1.0f, 3.0f);
    //     ImGui::SliderFloat3("wep rot", &player.wep_rot.x, -90.0f, 90.0f);'
        ImGui::SliderFloat("waveAmplitude", &waveAmplitude, -10.0, 10.0f); // 0.03f;
        ImGui::SliderFloat("waveFrequency", &waveFrequency, -10.0, 10.0f); // 5.0f;
        ImGui::SliderFloat("waveSpeed", &waveSpeed, -10.0, 10.0f); // 2.0f;
        
        // ImGui::SliderFloat("fresnelPower", &fresnelPower, -10.0, 10.0f); // 3.0f;
        ImGui::SliderFloat3("color", &clr.x, 0.0f, 1.0f);
        ImGui::SliderFloat3("emis", &emis.x, 0.0f, 1.0f); // 1.0f;     
        ImGui::SliderFloat3("fresclr", &fres.x, 0.0, 1.0f); // 1.0f;     
        ImGui::SliderFloat("exp", &expon, -5.0, 25.0f); // 1.0f;        
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
