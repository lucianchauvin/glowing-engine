// Jolt Physics Library (https://github.com/jrouwe/JoltPhysics)
// SPDX-FileCopyrightText: 2025 Jorrit Rouwe
// SPDX-License-Identifier: CC0-1.0
// This file is in the public domain. It serves as an example to start building your own application using Jolt Physics. Feel free to copy paste without attribution!

// The Jolt headers don't include Jolt.h. Always include Jolt.h before including any other Jolt header.
// You can use Jolt.h in your precompiled header to speed up compilation.
#include <Jolt/Jolt.h>

// Jolt includes
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>

// STL includes
#include <iostream>
#include <cstdarg>
#include <thread>

// Disable common warnings triggered by Jolt, you can use JPH_SUPPRESS_WARNING_PUSH / JPH_SUPPRESS_WARNING_POP to store and restore the warning state
JPH_SUPPRESS_WARNINGS

// All Jolt symbols are in the JPH namespace
using namespace JPH;

// If you want your code to compile using single or double precision write 0.0_r to get a Real value that compiles to double or float depending if JPH_DOUBLE_PRECISION is set or not.
using namespace JPH::literals;

// We're also using STL classes in this example
using namespace std;

// Callback for traces, connect this to your own trace function if you have one
static void TraceImpl(const char *inFMT, ...)
{
	// Format the message
	va_list list;
	va_start(list, inFMT);
	char buffer[1024];
	vsnprintf(buffer, sizeof(buffer), inFMT, list);
	va_end(list);

	// Print to the TTY
	cout << buffer << endl;
}

#ifdef JPH_ENABLE_ASSERTS

// Callback for asserts, connect this to your own assert handler if you have one
static bool AssertFailedImpl(const char *inExpression, const char *inMessage, const char *inFile, uint inLine)
{
	// Print to the TTY
	cout << inFile << ":" << inLine << ": (" << inExpression << ") " << (inMessage != nullptr? inMessage : "") << endl;

	// Breakpoint
	return true;
};

#endif // JPH_ENABLE_ASSERTS

// Layer that objects can be in, determines which other objects it can collide with
// Typically you at least want to have 1 layer for moving bodies and 1 layer for static bodies, but you can have more
// layers if you want. E.g. you could have a layer for high detail collision (which is not used by the physics simulation
// but only if you do collision testing).
namespace Layers
{
	static constexpr ObjectLayer NON_MOVING = 0;
	static constexpr ObjectLayer MOVING = 1;
	static constexpr ObjectLayer NUM_LAYERS = 2;
};

/// Class that determines if two object layers can collide
class ObjectLayerPairFilterImpl : public ObjectLayerPairFilter
{
public:
	virtual bool					ShouldCollide(ObjectLayer inObject1, ObjectLayer inObject2) const override
	{
		switch (inObject1)
		{
		case Layers::NON_MOVING:
			return inObject2 == Layers::MOVING; // Non moving only collides with moving
		case Layers::MOVING:
			return true; // Moving collides with everything
		default:
			JPH_ASSERT(false);
			return false;
		}
	}
};

// Each broadphase layer results in a separate bounding volume tree in the broad phase. You at least want to have
// a layer for non-moving and moving objects to avoid having to update a tree full of static objects every frame.
// You can have a 1-on-1 mapping between object layers and broadphase layers (like in this case) but if you have
// many object layers you'll be creating many broad phase trees, which is not efficient. If you want to fine tune
// your broadphase layers define JPH_TRACK_BROADPHASE_STATS and look at the stats reported on the TTY.
namespace BroadPhaseLayers
{
	static constexpr BroadPhaseLayer NON_MOVING(0);
	static constexpr BroadPhaseLayer MOVING(1);
	static constexpr uint NUM_LAYERS(2);
};

// BroadPhaseLayerInterface implementation
// This defines a mapping between object and broadphase layers.
class BPLayerInterfaceImpl final : public BroadPhaseLayerInterface
{
public:
									BPLayerInterfaceImpl()
	{
		// Create a mapping table from object to broad phase layer
		mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
		mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
	}

	virtual uint					GetNumBroadPhaseLayers() const override
	{
		return BroadPhaseLayers::NUM_LAYERS;
	}

	virtual BroadPhaseLayer			GetBroadPhaseLayer(ObjectLayer inLayer) const override
	{
		JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
		return mObjectToBroadPhase[inLayer];
	}

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
	virtual const char *			GetBroadPhaseLayerName(BroadPhaseLayer inLayer) const override
	{
		switch ((BroadPhaseLayer::Type)inLayer)
		{
		case (BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:	return "NON_MOVING";
		case (BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:		return "MOVING";
		default:													JPH_ASSERT(false); return "INVALID";
		}
	}
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

private:
	BroadPhaseLayer					mObjectToBroadPhase[Layers::NUM_LAYERS];
};

/// Class that determines if an object layer can collide with a broadphase layer
class ObjectVsBroadPhaseLayerFilterImpl : public ObjectVsBroadPhaseLayerFilter
{
public:
	virtual bool				ShouldCollide(ObjectLayer inLayer1, BroadPhaseLayer inLayer2) const override
	{
		switch (inLayer1)
		{
		case Layers::NON_MOVING:
			return inLayer2 == BroadPhaseLayers::MOVING;
		case Layers::MOVING:
			return true;
		default:
			JPH_ASSERT(false);
			return false;
		}
	}
};

// An example contact listener
class MyContactListener : public ContactListener
{
public:
	// See: ContactListener
	virtual ValidateResult	OnContactValidate(const Body &inBody1, const Body &inBody2, RVec3Arg inBaseOffset, const CollideShapeResult &inCollisionResult) override
	{
		cout << "Contact validate callback" << endl;

		// Allows you to ignore a contact before it is created (using layers to not make objects collide is cheaper!)
		return ValidateResult::AcceptAllContactsForThisBodyPair;
	}

	virtual void			OnContactAdded(const Body &inBody1, const Body &inBody2, const ContactManifold &inManifold, ContactSettings &ioSettings) override
	{
		cout << "A contact was added" << endl;
	}

	virtual void			OnContactPersisted(const Body &inBody1, const Body &inBody2, const ContactManifold &inManifold, ContactSettings &ioSettings) override
	{
		cout << "A contact was persisted" << endl;
	}

	virtual void			OnContactRemoved(const SubShapeIDPair &inSubShapePair) override
	{
		cout << "A contact was removed" << endl;
	}
};

// An example activation listener
class MyBodyActivationListener : public BodyActivationListener
{
public:
	virtual void		OnBodyActivated(const BodyID &inBodyID, uint64 inBodyUserData) override
	{
		cout << "A body got activated" << endl;
	}

	virtual void		OnBodyDeactivated(const BodyID &inBodyID, uint64 inBodyUserData) override
	{
		cout << "A body went to sleep" << endl;
	}
};






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
    // Register allocation hook. In this example we'll just let Jolt use malloc / free but you can override these if you want (see Memory.h).
	// This needs to be done before any other Jolt function is called.
	RegisterDefaultAllocator();
	// Install trace and assert callbacks
	Trace = TraceImpl;
	JPH_IF_ENABLE_ASSERTS(AssertFailed = AssertFailedImpl;)
	// Create a factory, this class is responsible for creating instances of classes based on their name or hash and is mainly used for deserialization of saved data.
	// It is not directly used in this example but still required.
	Factory::sInstance = new Factory();
	// Register all physics types with the factory and install their collision handlers with the CollisionDispatch class.
	// If you have your own custom shape types you probably need to register their handlers with the CollisionDispatch before calling this function.
	// If you implement your own default material (PhysicsMaterial::sDefault) make sure to initialize it before this function or else this function will create one for you.
	RegisterTypes();
	// We need a temp allocator for temporary allocations during the physics update. We're
	// pre-allocating 10 MB to avoid having to do allocations during the physics update.
	// B.t.w. 10 MB is way too much for this example but it is a typical value you can use.
	// If you don't want to pre-allocate you can also use TempAllocatorMalloc to fall back to
	// malloc / free.
	TempAllocatorImpl temp_allocator(10 * 1024 * 1024);
	// We need a job system that will execute physics jobs on multiple threads. Typically
	// you would implement the JobSystem interface yourself and let Jolt Physics run on top
	// of your own job scheduler. JobSystemThreadPool is an example implementation.
	JobSystemThreadPool job_system(cMaxPhysicsJobs, cMaxPhysicsBarriers, thread::hardware_concurrency() - 1);
	// This is the max amount of rigid bodies that you can add to the physics system. If you try to add more you'll get an error.
	// Note: This value is low because this is a simple test. For a real project use something in the order of 65536.
	const uint cMaxBodies = 1024;
	// This determines how many mutexes to allocate to protect rigid bodies from concurrent access. Set it to 0 for the default settings.
	const uint cNumBodyMutexes = 0;
	// This is the max amount of body pairs that can be queued at any time (the broad phase will detect overlapping
	// body pairs based on their bounding boxes and will insert them into a queue for the narrowphase). If you make this buffer
	// too small the queue will fill up and the broad phase jobs will start to do narrow phase work. This is slightly less efficient.
	// Note: This value is low because this is a simple test. For a real project use something in the order of 65536.
	const uint cMaxBodyPairs = 1024;
	// This is the maximum size of the contact constraint buffer. If more contacts (collisions between bodies) are detected than this
	// number then these contacts will be ignored and bodies will start interpenetrating / fall through the world.
	// Note: This value is low because this is a simple test. For a real project use something in the order of 10240.
	const uint cMaxContactConstraints = 1024;
	// Create mapping table from object layer to broadphase layer
	// Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
	// Also have a look at BroadPhaseLayerInterfaceTable or BroadPhaseLayerInterfaceMask for a simpler interface.
	BPLayerInterfaceImpl broad_phase_layer_interface;
	// Create class that filters object vs broadphase layers
	// Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
	// Also have a look at ObjectVsBroadPhaseLayerFilterTable or ObjectVsBroadPhaseLayerFilterMask for a simpler interface.
	ObjectVsBroadPhaseLayerFilterImpl object_vs_broadphase_layer_filter;
	// Create class that filters object vs object layers
	// Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
	// Also have a look at ObjectLayerPairFilterTable or ObjectLayerPairFilterMask for a simpler interface.
	ObjectLayerPairFilterImpl object_vs_object_layer_filter;
	// Now we can create the actual physics system.
	PhysicsSystem physics_system;
	physics_system.Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints, broad_phase_layer_interface, object_vs_broadphase_layer_filter, object_vs_object_layer_filter);
	// A body activation listener gets notified when bodies activate and go to sleep
	// Note that this is called from a job so whatever you do here needs to be thread safe.
	// Registering one is entirely optional.
	MyBodyActivationListener body_activation_listener;
	physics_system.SetBodyActivationListener(&body_activation_listener);
	// A contact listener gets notified when bodies (are about to) collide, and when they separate again.
	// Note that this is called from a job so whatever you do here needs to be thread safe.
	// Registering one is entirely optional.
	MyContactListener contact_listener;
	physics_system.SetContactListener(&contact_listener);
	// The main way to interact with the bodies in the physics system is through the body interface. There is a locking and a non-locking
	// variant of this. We're going to use the locking version (even though we're not planning to access bodies from multiple threads)
	BodyInterface &body_interface = physics_system.GetBodyInterface();



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
    // PHYS
    BoxShapeSettings floor_shape_settings(Vec3(10.0f, 1.0f, 10.0f));
	floor_shape_settings.SetEmbedded(); // A ref counted object on the stack (base class RefTarget) should be marked as such to prevent it from being freed when its reference count goes to 0.
    ShapeSettings::ShapeResult floor_shape_result = floor_shape_settings.Create();
	ShapeRefC floor_shape = floor_shape_result.Get(); // We don't expect an error here, but you can check floor_shape_result for HasError() / GetError()
    BodyCreationSettings floor_settings(floor_shape, RVec3(0.0_r, -1.0_r, 0.0_r), Quat::sIdentity(), EMotionType::Static, Layers::NON_MOVING);
	// Create the actual rigid body
	Body *floor = body_interface.CreateBody(floor_settings); // Note that if we run out of bodies this can return nullptr
	// Add it to the world
	body_interface.AddBody(floor->GetID(), EActivation::DontActivate);
    // Now create a dynamic body to bounce on the floor
	// Note that this uses the shorthand version of creating and adding a body to the world
    // std::vector<BodyID> bodies;
    // for (int i = 0; i < 10; i++) {
    //     glm::vec3 pos   = glm::vec3(2.0f * i - 5, 2.0f, 0.0f); 
    //     glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
    //     glm::vec3 color = glm::vec3(.05f * i, 0.5f, 0.2f);
    //     Entity e(&sphere, pos, false, scale, color, false, 3.0f);
    //     scene.include(e);

    //     BodyCreationSettings sphere_settings(new SphereShape(1.0f), RVec3(pos.x, pos.y, pos.z), Quat::sIdentity(), EMotionType::Dynamic, Layers::MOVING);
    //     BodyID sphere_id = body_interface.CreateAndAddBody(sphere_settings, EActivation::Activate);
    //     bodies.push_back(sphere_id);
    //     // Now you can interact with the dynamic body, in this case we're going to give it a velocity.
    //     // (note that if we had used CreateBody then we could have set the velocity straight on the body before adding it to the physics system)
    //     body_interface.SetLinearVelocity(sphere_id, Vec3(0.0f, i, 0.0f));
    // }
    // physics_system.OptimizeBroadPhase();

	pos   = glm::vec3(0.0f, 2.0f, -5.0f); 
	scale = glm::vec3(1.0f, 1.0f, 1.0f);
	color = glm::vec3(.05f, 0.5f, 0.2f);
	Entity e2(&sphere, pos, false, scale, color, false, 3.0f);
	scene.include(e2);

	BodyCreationSettings sphere_settings(new SphereShape(1.0f), RVec3(0.0_r, 2.0_r, -5.0_r), Quat::sIdentity(), EMotionType::Dynamic, Layers::MOVING);
	BodyID sphere_id = body_interface.CreateAndAddBody(sphere_settings, EActivation::Activate);

	// Now you can interact with the dynamic body, in this case we're going to give it a velocity.
	// (note that if we had used CreateBody then we could have set the velocity straight on the body before adding it to the physics system)
	body_interface.SetLinearVelocity(sphere_id, Vec3(0.0f, 10.0f, 0.0f));

	// We simulate the physics world in discrete time steps. 60 Hz is a good rate to update the physics system.
	// const float cDeltaTime = 1.0f / 60.0f;

	// Optional step: Before starting the physics simulation you can optimize the broad phase. This improves collision detection performance (it's pointless here because we only have 2 bodies).
	// You should definitely not call this every frame or when e.g. streaming in a new level section as it is an expensive operation.
	// Instead insert all new objects in batches instead of 1 at a time to keep the broad phase efficient.
	physics_system.OptimizeBroadPhase();

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
        // player.update_player_physics(deltaTime);
		player.update_plane_physics(deltaTime);		
        if (player.player_physics.dashing) {
            Entity e(&sphere, player.player_physics.position, true, glm::vec3(0.1f), glm::vec3(0.0f, 0.3f, 0.2f), true, 5.0f);
            scene.include(e);
        }
        
        // physics.step(deltaTime);
        // const int cCollisionSteps = 1;
        // const float cDeltaTime = 1.0f / 60.0f;
        printf("here\n");
        // physics_system.Update(cDeltaTime, cCollisionSteps, &temp_allocator, &job_system);
		RVec3 p = body_interface.GetCenterOfMassPosition(sphere_id);
		scene.entities[1].physics.position.x = p.GetX();
		scene.entities[1].physics.position.y = p.GetY();
		scene.entities[1].physics.position.z = p.GetZ();
        // for (int i = 0; i < 10; i++) {
        //     RVec3 p = body_interface.GetCenterOfMassPosition(bodies[i]);
        //     scene.entities[i+1].physics.position.x = p.GetX();
        //     scene.entities[i+1].physics.position.y = p.GetY();
        //     scene.entities[i+1].physics.position.z = p.GetZ();
        // }

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

    // for (int i = 0; i < 10; i++) {
    //     body_interface.RemoveBody(bodies[i]);
    //     // Destroy the sphere. After this the sphere ID is no longer valid.
    // 	body_interface.DestroyBody(bodies[i]);
    // }
	// Remove the sphere from the physics system. Note that the sphere itself keeps all of its state and can be re-added at any time.
	body_interface.RemoveBody(sphere_id);

	// Destroy the sphere. After this the sphere ID is no longer valid.
	body_interface.DestroyBody(sphere_id);
    // // Remove and destroy the floor
	body_interface.RemoveBody(floor->GetID());
	body_interface.DestroyBody(floor->GetID());
	// Unregisters all types with the factory and cleans up the default material
	UnregisterTypes();
	// Destroy the factory
	delete Factory::sInstance;
	Factory::sInstance = nullptr;

    renderer.shutdown();
    return 0;
}
