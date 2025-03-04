#include <camera.h>

#include <dearimgui/imgui.h>
#include <dearimgui/imgui_impl_glfw.h>
#include <dearimgui/imgui_impl_opengl3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <shader.h>

#include <iostream>
#include <filesystem>
#include <random>
#include <ctime>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void process_input(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool toggle_mouse_lock = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

/*const*/ float GRAVITY = 9.8f;
/*const*/ float JUMP_FORCE = 3.0f;
/*const*/ float FRICTION = .937f;
/*const*/ float ACCELERATION = 128.0f;
/*const*/ float MAX_VELOCITY = 5.3f;
/*const*/ float PLAYER_HEIGHT = 1.8f;

const float GRID_SIZE = 10;
// floor parameters
const float FLOOR_SIZE = GRID_SIZE * 4.0f;
const float FLOOR_Y = -GRID_SIZE;

// player physics state
struct player_physics {
    glm::vec3 velocity = glm::vec3(0.0f);
    bool isOnGround = false;
    bool isJumping = false;
} player_physics;

const int NUM_RANDOM_CUBES = 20;
struct RandomCube {
    glm::vec3 position;
    glm::vec3 color;
    float scale;
};
RandomCube* random_cubes = nullptr;

bool key_toggles[256] = {false};
static void char_callback(GLFWwindow *window, unsigned int key) {
    key_toggles[key] = !key_toggles[key];
};

// forward declaration for collision detection
bool check_floor_collision(const glm::vec3& position, float height);

// initialize random cubes
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
            FLOOR_Y + scale_dist(rng) / 2.0f, // place on top of floor
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

// function to update player physics
void update_player_physics(float deltaTime) {
    // apply gravity
    if (!player_physics.isOnGround) {
        player_physics.velocity.y -= GRAVITY * deltaTime;
    }
    // move player
    glm::vec3 new_position = camera.Position + player_physics.velocity * deltaTime;
    // check floor collision
    bool floor_collision = check_floor_collision(new_position, PLAYER_HEIGHT);
    if (floor_collision) {
        player_physics.isOnGround = true;
        player_physics.velocity.y = 0.0f;
        new_position.y = FLOOR_Y + PLAYER_HEIGHT; // snap to floor
    } else {
        player_physics.isOnGround = false;
    }
    // update camera position
    camera.Position = new_position;
}

// check collision with the floor
bool check_floor_collision(const glm::vec3& position, float height) {
    return position.y - height <= FLOOR_Y;
}

// generate floor vertices
float* generate_floor_vertices() {
    float* floor_vertices = new float[30]; // 6 vertices * 5 components (position + tex coords)
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

int main() {
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

// #ifdef __APPLE__
//     glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
// #endif

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetCharCallback(window, char_callback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // configure global opengl state
    glEnable(GL_DEPTH_TEST);
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

    // setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();

    // build and compile our shader program
    if (!std::filesystem::exists("../resources/shaders/vertex.glsl") ||
        !std::filesystem::exists("../resources/shaders/fragment.glsl")) {
        std::cerr << "Shader files not found! Ensure they're in the 'shaders/' directory relative to the executable." << std::endl;
        return -1;
    }
    Shader ourShader("../resources/shaders/vertex.glsl", "../resources/shaders/fragment.glsl");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    // initialize random cubes and player
    initrandom_cubes();
    camera.Position = glm::vec3(0.0f, FLOOR_Y + PLAYER_HEIGHT, 0.0f);
    
    // generate floor vertices
    float* floor_vertices = generate_floor_vertices();

    // create a single VBO for both cube and floor
    const size_t cube_vertices_size = sizeof(vertices);
    const size_t floor_verticesSize = 30 * sizeof(float); // 6 vertices * 5 components
    
    // combine vertices
    float* combined_vertices = new float[(cube_vertices_size + floor_verticesSize) / sizeof(float)];
    memcpy(combined_vertices, vertices, cube_vertices_size);
    memcpy(combined_vertices + cube_vertices_size / sizeof(float), floor_vertices, floor_verticesSize);
    
    unsigned int VBO, VAO;
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

    // load and create textures
    unsigned int texture1, texture2, floorTexture;
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
    
    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    ourShader.use(); // don't forget to activate/use the shader before setting uniforms!
    // either set it manually like so:
    glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0);
    // or set it via the texture class
    ourShader.setInt("texture2", 1);

    // blend ratio
    int t1_location = glGetUniformLocation(ourShader.ID, "t1");
    float t1 = 0.5f;    
    int t2_location = glGetUniformLocation(ourShader.ID, "t2");
    float t2 = 0.5f;
    
    // custom color uniform
    int color_location = glGetUniformLocation(ourShader.ID, "customColor");

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        process_input(window);
        
        // update player physics
        update_player_physics(deltaTime);

        if (key_toggles[(unsigned) 't']) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } 
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

        // activate shader
        ourShader.use();

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glUniform1f(t1_location, t1);
        glUniform1f(t2_location, t2);
        
        // set default color (white)
        glUniform3f(color_location, 1.0f, 1.0f, 1.0f);

        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
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
        
        // gui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        ImGui::Begin("Controls");
        ImGui::Text("FPS: %.1f", 1.0f / deltaTime);
        ImGui::Text("Position: (%.1f, %.1f, %.1f)", camera.Position.x, camera.Position.y, camera.Position.z);
        ImGui::Text("Velocity: (%.1f, %.1f, %.1f)", player_physics.velocity.x, player_physics.velocity.y, player_physics.velocity.z);
        ImGui::Text("On Ground: %s", player_physics.isOnGround ? "Yes" : "No");
        ImGui::Checkbox("Wireframe [t]", &key_toggles[(unsigned)'t']);
        ImGui::SliderFloat("Texture 1 Blend", &t1, 0.0f, 1.0f);
        ImGui::SliderFloat("Texture 2 Blend", &t2, 0.0f, 1.0f);
        ImGui::End();

        ImGui::Begin("Settings");
        ImGui::SliderFloat("GRAVITY", &GRAVITY, 0.1f, 20.0f);
        ImGui::SliderFloat("JUMP_FORCE", &JUMP_FORCE, 1.0f, 10.0f);
        ImGui::SliderFloat("FRICTION", &FRICTION, 0.5f, 1.0f);
        ImGui::SliderFloat("ACCELERATION", &ACCELERATION, 1.0f, 200.0f);
        ImGui::SliderFloat("MAX_VELOCITY", &MAX_VELOCITY, 0.1f, 10.0f);
        ImGui::SliderFloat("PLAYER_HEIGHT", &PLAYER_HEIGHT, 1.0f, 10.0f);
        ImGui::End();
        
        // render gui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // optional: de-allocate all resources once they've outlived their purpose:
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    delete[] random_cubes;
    delete[] floor_vertices;
    delete[] combined_vertices;

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

void process_input(GLFWwindow *window)
{
    // if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    //     glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        if (toggle_mouse_lock)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        toggle_mouse_lock = !toggle_mouse_lock;
    }

    // get movement direction in camera space
    glm::vec3 movement(0.0f);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        movement.z += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        movement.z -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        movement.x -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        movement.x += 1.0f;
   
    // normalize movement vector if the player is moving diagonally
    if (glm::length(movement) > 0.0f) {
        movement = glm::normalize(movement);
    }

    // can jump when on ground
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && player_physics.isOnGround) {
        player_physics.velocity.y = JUMP_FORCE;
        player_physics.isOnGround = false;
        player_physics.isJumping = true;
    }

    // convert camera-relative movement to world space
    glm::vec3 forward = glm::normalize(glm::vec3(camera.Front.x, 0.0f, camera.Front.z));
    glm::vec3 right = glm::normalize(glm::cross(forward, camera.WorldUp));
    
    glm::vec3 acceleration = forward * movement.z + right * movement.x;
    acceleration *= ACCELERATION * deltaTime;

    // apply acceleration
    player_physics.velocity.x += acceleration.x;
    player_physics.velocity.z += acceleration.z;

    // apply friction when on ground
    if (player_physics.isOnGround) {
        player_physics.velocity.x *= FRICTION;
        player_physics.velocity.z *= FRICTION;
    }

    // limit horizontal velocity
    float horizontal_speed = glm::length(glm::vec2(player_physics.velocity.x, player_physics.velocity.z));
    if (horizontal_speed > MAX_VELOCITY) {
        float scale = MAX_VELOCITY / horizontal_speed;
        player_physics.velocity.x *= scale;
        player_physics.velocity.z *= scale;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}