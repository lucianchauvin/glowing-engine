#include "Renderer.h"
#include <dearimgui/imgui.h>
#include <dearimgui/imgui_impl_glfw.h>
#include <dearimgui/imgui_impl_opengl3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ctime>

#include <controller.h>
#include <shader.h>

// void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// settings
const unsigned int SCR_WIDTH = 1800;
const unsigned int SCR_HEIGHT = 1000;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// floor parameters
const float GRID_SIZE = 10;
const float FLOOR_SIZE = GRID_SIZE * 4.0f;
const float FLOOR_Y = 0;

// std::vector<float> sphereVertices;
// std::vector<unsigned int> sphereIndices;

// const int NUM_RANDOM_CUBES = 20;
// struct RandomCube {
//     glm::vec3 position;
//     glm::vec3 color;
//     float scale;
// };
// RandomCube* random_cubes = nullptr;

// // generate sphere mesh
// void generateSphere(float radius, unsigned int rings, unsigned int sectors);

// // initialize random cubes
// void initrandom_cubes() {
//     std::mt19937 rng(static_cast<unsigned int>(time(nullptr)));
//     std::uniform_real_distribution<float> pos_dist_x(-FLOOR_SIZE/2 + 1.0f, FLOOR_SIZE/2 - 1.0f);
//     std::uniform_real_distribution<float> pos_dist_z(-FLOOR_SIZE/2 + 1.0f, FLOOR_SIZE/2 - 1.0f);
//     std::uniform_real_distribution<float> color_dist(0.0f, 1.0f);
//     std::uniform_real_distribution<float> scale_dist(0.5f, 2.0f);
    
//     random_cubes = new RandomCube[NUM_RANDOM_CUBES];
    
//     for (int i = 0; i < NUM_RANDOM_CUBES; i++) {
//         random_cubes[i].position = glm::vec3(
//             pos_dist_x(rng),
//             FLOOR_Y + scale_dist(rng) / 2.0f, // place on top of floor
//             pos_dist_z(rng)
//         );
//         random_cubes[i].color = glm::vec3(
//             color_dist(rng),
//             color_dist(rng),
//             color_dist(rng)
//         );
//         random_cubes[i].scale = scale_dist(rng);
//     }
// }

// // generate floor vertices
// float* generate_floor_vertices() {
//     float* floor_vertices = new float[30]; // 6 vertices * 5 components (position + tex coords)
//     // floor vertices (position, texture coords)
//     float temp[] = {
//         -FLOOR_SIZE/2, FLOOR_Y, -FLOOR_SIZE/2,  0.0f, 0.0f,
//          FLOOR_SIZE/2, FLOOR_Y, -FLOOR_SIZE/2,  FLOOR_SIZE, 0.0f,
//          FLOOR_SIZE/2, FLOOR_Y,  FLOOR_SIZE/2,  FLOOR_SIZE, FLOOR_SIZE,
//          FLOOR_SIZE/2, FLOOR_Y,  FLOOR_SIZE/2,  FLOOR_SIZE, FLOOR_SIZE,
//         -FLOOR_SIZE/2, FLOOR_Y,  FLOOR_SIZE/2,  0.0f, FLOOR_SIZE,
//         -FLOOR_SIZE/2, FLOOR_Y, -FLOOR_SIZE/2,  0.0f, 0.0f
//     };
    
//     memcpy(floor_vertices, temp, 30 * sizeof(float));
//     return floor_vertices;
// }

int main() {
    // glfw: initialize and configure
//     glfwInit();
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//     glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

// // #ifdef __APPLE__
// //     glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
// // #endif

//     // glfw window creation
//     GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
//     if (window == NULL) {
//         std::cout << "Failed to create GLFW window" << std::endl;
//         glfwTerminate();
//         return -1;
//     }

    Controller player;
    Renderer renderer;
    renderer.init(SCR_WIDTH, SCR_HEIGHT, "GLOW");

    // glfwMakeContextCurrent(window);
    // glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glfwSetWindowUserPointer(window, &player);
    // glfwSetCursorPosCallback(window, Controller::mouse_callback);
    // glfwSetScrollCallback(window, Controller::scroll_callback);
    // glfwSetCharCallback(window, Controller::char_callback);

    // tell GLFW to capture our mouse
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    // if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    //     std::cout << "Failed to initialize GLAD" << std::endl;
    //     return -1;
    // }
    // configure global opengl state
    // glEnable(GL_DEPTH_TEST);
    // Setup Dear ImGui context
    // IMGUI_CHECKVERSION();
    // ImGui::CreateContext();
    // ImGuiIO& io = ImGui::GetIO();
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

    // setup Platform/Renderer backends
    // ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    // ImGui_ImplOpenGL3_Init();

    // build and compile our shader program
    // if (!std::filesystem::exists("../resources/shaders/vertex.glsl") ||
    //     !std::filesystem::exists("../resources/shaders/fragment.glsl")) {
    //     std::cerr << "Shader files not found! Ensure they're in the 'shaders/' directory relative to the executable." << std::endl;
    //     return -1;
    // }
    // Shader ourShader("../resources/shaders/vertex.glsl", "../resources/shaders/fragment.glsl");

    // float vertices[] = {
    //     -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    //      0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
    //      0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    //      0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    //     -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    //     -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    // };

    // initialize random cubes and player
    
    // generate floor vertices
    // float* floor_vertices = generate_floor_vertices();

    // // create a single VBO for both cube and floor
    // const size_t cube_vertices_size = sizeof(vertices);
    // const size_t floor_verticesSize = 30 * sizeof(float); // 6 vertices * 5 components
    
    // // combine vertices
    // float* combined_vertices = new float[(cube_vertices_size + floor_verticesSize) / sizeof(float)];
    // memcpy(combined_vertices, vertices, cube_vertices_size);
    // memcpy(combined_vertices + cube_vertices_size / sizeof(float), floor_vertices, floor_verticesSize);
    
    // unsigned int VBO, VAO;
    // glGenVertexArrays(1, &VAO);
    // glGenBuffers(1, &VBO);

    // glBindVertexArray(VAO);

    // glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData(GL_ARRAY_BUFFER, cube_vertices_size + floor_verticesSize, combined_vertices, GL_STATIC_DRAW);

    // // position attribute
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    // glEnableVertexAttribArray(0);
    // // texture coord attribute
    // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    // glEnableVertexAttribArray(1);

    // load and create textures
    // unsigned int texture1, texture2, floorTexture;
    // // texture 1
    // glGenTextures(1, &texture1);
    // glBindTexture(GL_TEXTURE_2D, texture1); 
    // // set the texture wrapping parameters
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // // set texture filtering parameters
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // // load image, create texture and generate mipmaps
    // int width, height, nrChannels;
    // stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    // unsigned char *data = stbi_load("../resources/textures/container.jpg", &width, &height, &nrChannels, 0);
    // if (data) {
    //     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    //     glGenerateMipmap(GL_TEXTURE_2D);
    // }
    // else {
    //     std::cout << "Failed to load texture" << std::endl;
    // }
    // stbi_image_free(data);
    // // texture 2
    // glGenTextures(1, &texture2);
    // glBindTexture(GL_TEXTURE_2D, texture2);
    // // set the texture wrapping parameters
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // // set texture filtering parameters
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // // load image, create texture and generate mipmaps
    // data = stbi_load("../resources/textures/awesomeface.png", &width, &height, &nrChannels, 0);
    // if (data) {
    //     // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
    //     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    //     glGenerateMipmap(GL_TEXTURE_2D);
    // }
    // else {
    //     std::cout << "Failed to load texture" << std::endl;
    // }
    // stbi_image_free(data);
    // // floor texture
    // glGenTextures(1, &floorTexture);
    // glBindTexture(GL_TEXTURE_2D, floorTexture);
    // // set the texture wrapping parameters
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // // set texture filtering parameters
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // // load image, create texture and generate mipmaps
    // data = stbi_load("../resources/textures/floor.jpg", &width, &height, &nrChannels, 0);
    // if (data) {
    //     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    //     glGenerateMipmap(GL_TEXTURE_2D);
    // }
    // else {
    //     std::cout << "Failed to load floor texture, using container texture instead" << std::endl;
    //     // Fallback to container texture if floor texture is missing
    //     floorTexture = texture1;
    // }
    // stbi_image_free(data);
    
    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // ourShader.use(); // don't forget to activate/use the shader before setting uniforms!
    // // either set it manually like so:
    // glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0);
    // // or set it via the texture class
    // ourShader.setInt("texture2", 1);

    // blend ratio
    // int t1_location = glGetUniformLocation(ourShader.ID, "t1");
    // float t1 = 0.5f;    
    // int t2_location = glGetUniformLocation(ourShader.ID, "t2");
    // float t2 = 0.5f;

    // unsigned int sphereVAO = 0, sphereVBO = 0, sphereEBO = 0;    
    // // generate sphere mesh
    // generateSphere(0.5f, 16, 16);  // radius, rings, sectors

    // // sphere vao setup - use separate VAO/VBO/EBO for the sphere
    // glGenVertexArrays(1, &sphereVAO);
    // glGenBuffers(1, &sphereVBO);
    // glGenBuffers(1, &sphereEBO);

    // glBindVertexArray(sphereVAO);
        
    // glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    // glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), &sphereVertices[0], GL_STATIC_DRAW);
        
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(unsigned int), &sphereIndices[0], GL_STATIC_DRAW);
        
    // // position attribute
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    // glEnableVertexAttribArray(0);
    // // texture coord attribute
    // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    // glEnableVertexAttribArray(1);
    // // important: restore the original VAO binding after setting up the sphere
    // glBindVertexArray(VAO);

    // custom color uniform
    // int color_location = glGetUniformLocation(ourShader.ID, "customColor");
    
    // render loop
    while (renderer.open())
    {
        float currentFrame = renderer.get_time();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        player.process_input(renderer.window, deltaTime);
        // update player physics
        player.update_player_physics(deltaTime);
        // render scene
        renderer.render_scene();

        // // gui
        // ImGui_ImplOpenGL3_NewFrame();
        // ImGui_ImplGlfw_NewFrame();
        // ImGui::NewFrame();
        
        // ImGui::Begin("Controls");
        // ImGui::Text("FPS: %.1f", 1.0f / deltaTime);
        // ImGui::Text("Position: (%.1f, %.1f, %.1f)", camera.Position.x, camera.Position.y, camera.Position.z);
        // ImGui::Text("Velocity: (%.1f, %.1f, %.1f)", player_physics.velocity.x, player_physics.velocity.y, player_physics.velocity.z);
        // ImGui::Text("On Ground: %s", player_physics.isOnGround ? "Yes" : "No");
        // ImGui::Checkbox("Wireframe [t]", &key_toggles[(unsigned)'t']);
        // ImGui::SliderFloat("Texture 1 Blend", &t1, 0.0f, 1.0f);
        // ImGui::SliderFloat("Texture 2 Blend", &t2, 0.0f, 1.0f);
        // ImGui::Text("Camera Mode: %s", is_third_person ? "Third Person" : "First Person");
        // ImGui::Text("Press TAB to toggle camera mode");
        // ImGui::End();

        // ImGui::Begin("Settings");
        // ImGui::SliderFloat("GRAVITY", &GRAVITY, 0.1f, 20.0f);
        // ImGui::SliderFloat("JUMP_FORCE", &JUMP_FORCE, 1.0f, 10.0f);
        // ImGui::SliderFloat("FRICTION", &FRICTION, 0.5f, 1.0f);
        // ImGui::SliderFloat("ACCELERATION", &ACCELERATION, 1.0f, 200.0f);
        // ImGui::SliderFloat("MAX_VELOCITY", &MAX_VELOCITY, 0.1f, 10.0f);
        // ImGui::SliderFloat("PLAYER_HEIGHT", &PLAYER_HEIGHT, 1.0f, 10.0f);
        // ImGui::End();
        
        // // render gui
        // ImGui::Render();
        // ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
            // glfwSwapBuffers(window);
            // glfwPollEvents();
    }

    // Cleanup
    // ImGui_ImplOpenGL3_Shutdown();
    // ImGui_ImplGlfw_Shutdown();
    // ImGui::DestroyContext();

    // optional: de-allocate all resources once they've outlived their purpose:
    // glDeleteVertexArrays(1, &VAO);
    // glDeleteBuffers(1, &VBO);

    // glDeleteVertexArrays(1, &sphereVAO);
    // glDeleteBuffers(1, &sphereVBO);
    // glDeleteBuffers(1, &sphereEBO);

    // delete[] random_cubes;
    // delete[] floor_vertices;
    // delete[] combined_vertices;

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // glfwTerminate();
    return 0;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
//     // make sure the viewport matches the new window dimensions; note that width and 
//     // height will be significantly larger than specified on retina displays.
//     glViewport(0, 0, width, height);
// }

// generate sphere mesh
// void generateSphere(float radius, unsigned int rings, unsigned int sectors) {
//     sphereVertices.clear();
//     sphereIndices.clear();
    
//     float const R = 1.0f/(float)(rings-1);
//     float const S = 1.0f/(float)(sectors-1);
    
//     for(unsigned int r = 0; r < rings; r++) {
//         for(unsigned int s = 0; s < sectors; s++) {
//             float const y = sin(-M_PI_2 + M_PI * r * R);
//             float const x = cos(2*M_PI * s * S) * sin(M_PI * r * R);
//             float const z = sin(2*M_PI * s * S) * sin(M_PI * r * R);
            
//             // position
//             sphereVertices.push_back(x * radius);
//             sphereVertices.push_back(y * radius);
//             sphereVertices.push_back(z * radius);
            
//             // texture coordinates
//             sphereVertices.push_back(s*S);
//             sphereVertices.push_back(r*R);
            
//             // add indices
//             if(r < rings-1 && s < sectors-1) {
//                 unsigned int curRow = r * sectors;
//                 unsigned int nextRow = (r + 1) * sectors;
                
//                 sphereIndices.push_back(curRow + s);
//                 sphereIndices.push_back(nextRow + s);
//                 sphereIndices.push_back(nextRow + s + 1);
                
//                 sphereIndices.push_back(curRow + s);
//                 sphereIndices.push_back(nextRow + s + 1);
//                 sphereIndices.push_back(curRow + s + 1);
//             }
//         }
//     }
// }