#pragma once

#include <glm/glm.hpp>

// includ this stuff cus
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>
#include <Jolt/Math/Vec3.h>
#include <Jolt/Math/Quat.h>

namespace JPH {
    class BodyInterface;
    class PhysicsSystem;
    class TempAllocatorImpl;
    class JobSystemThreadPool;
    class Body;
    class Shape;
    using BodyID = class BodyID;
}
// includ this stuff cus

namespace Physics {

    bool init();
    void shutdown();
    void update(float deltaTime = 1.0f / 60.0f);
    void optimize_broad_phase();
    // todo add call to somewhere OptimizeBroadPhase();
    // Optional step: Before starting the physics simulation you can optimize the broad phase. This improves collision detection performance (it's pointless here because we only have 2 bodies).
    // You should definitely not call this every frame or when e.g. streaming in a new level section as it is an expensive operation.
    // Instead insert all new objects in batches instead of 1 at a time to keep the broad phase efficient.

    JPH::BodyID addBox(const glm::vec3& pos, const glm::vec3& size, bool isStatic = false);
    JPH::BodyID addSphere(const glm::vec3& pos, float radius, bool isStatic = false);
    void removeBody(JPH::BodyID id);

    glm::vec3 getBodyPosition(JPH::BodyID id);
    void setBodyPosition(JPH::BodyID id, const glm::vec3& pos);
    glm::vec3 getBodyVelocity(JPH::BodyID id);
    void setBodyVelocity(JPH::BodyID id, const glm::vec3& vel);

    JPH::BodyInterface& getBodyInterface();

    //glm::vec3 toGlm(const JPH::RVec3& v);
    //glm::quat toGlm(const JPH::Quat& q);
    //JPH::RVec3 toJolt(const glm::vec3& v);
    //JPH::Quat toJolt(const glm::quat& q);
}
