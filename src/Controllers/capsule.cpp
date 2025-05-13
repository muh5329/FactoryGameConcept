#include <Controllers/capsule.h>
#include <Common/constants.hpp>

Capsule::Capsule()
    : position(Vector3{0, 0, 0}),
      radius(1.0f),
      height(2.0f),
      velocity(Vector3{0, 0, 0}),
      grounded(false),
      body(nullptr) {}

void Capsule::Update(float deltaTime) {
    // Get position from physics body
    if (body) {
        btTransform trans;
        body->getMotionState()->getWorldTransform(trans);
        btVector3 pos = trans.getOrigin();
        position = { pos.getX(), pos.getY(), pos.getZ() };
    }
}

void Capsule::Draw() const {
    DrawCapsule(position, height, radius, RED); // Replace with custom rendering if needed
}

void Capsule::UpdateCapsule(CapsuleObj* capsule, float delta) {
    if (!capsule->body) return;

    btVector3 velocity = capsule->body->getLinearVelocity();
    btVector3 movement(0, velocity.getY(), 0); // Keep vertical component as-is

    Vector3 input = { 0 };
    if (IsKeyDown(KEY_W)) input.z -= 1.0f;
    if (IsKeyDown(KEY_S)) input.z += 1.0f;
    if (IsKeyDown(KEY_A)) input.x -= 1.0f;
    if (IsKeyDown(KEY_D)) input.x += 1.0f;

    float length = sqrtf(input.x * input.x + input.z * input.z);
    if (length > 0.0f) {
        input.x /= length;
        input.z /= length;

        movement.setX(input.x * Constants::MOVE_SPEED);
        movement.setZ(input.z * Constants::MOVE_SPEED);
    }

    // Apply movement
    capsule->body->setLinearVelocity(movement);

    // Jump
    if (IsKeyPressed(KEY_SPACE) && capsule->grounded) {
        capsule->body->applyCentralImpulse(btVector3(0, Constants::JUMP_FORCE, 0));
        capsule->grounded = false;
    }

    // Check for ground contact
    btTransform trans;
    capsule->body->getMotionState()->getWorldTransform(trans);
    float y = trans.getOrigin().getY();

    if (y <= Constants::GROUND_Y + capsule->height / 2.0f + 0.01f) {
        capsule->grounded = true;
    } else {
        capsule->grounded = false;
    }
}

void Capsule::CreateCapsule(CapsuleObj* capsule, World* world) {
    btCollisionShape* capsuleShape = new btCapsuleShape(capsule->radius, capsule->height - 2 * capsule->radius);

    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin(btVector3(capsule->position.x, capsule->position.y, capsule->position.z));

    btScalar mass = 1.0f;
    btVector3 localInertia(0, 0, 0);
    capsuleShape->calculateLocalInertia(mass, localInertia);

    btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, capsuleShape, localInertia);
    capsule->body = new btRigidBody(rbInfo);
    capsule->body->setActivationState(DISABLE_DEACTIVATION);
    capsule->body->setFriction(0.9f);
    capsule->body->setSleepingThresholds(0, 0);
    capsule->body->setAngularFactor(btVector3(0, 0, 0)); // Prevent rotation

    world->dynamicsWorld->addRigidBody(capsule->body);
}
