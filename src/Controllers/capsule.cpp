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
    // Implement the logic to update the capsule here
}

void Capsule::Draw() const {
    // Implement capsule rendering code here, using raylib's Draw functions
}

void Capsule::UpdateCapsule(CapsuleObj* capsule, float delta) {
    // Implement your capsule update logic here
    Vector3 direction = {0};

    if (IsKeyDown(KEY_W)) direction.z -= 1.0f;
    if (IsKeyDown(KEY_S)) direction.z += 1.0f;
    if (IsKeyDown(KEY_A)) direction.x -= 1.0f;
    if (IsKeyDown(KEY_D)) direction.x += 1.0f;

    float len = sqrtf(direction.x * direction.x + direction.z * direction.z);
    if (len > 0.0f) {
        direction.x /= len;
        direction.z /= len;
        capsule->position.x += direction.x * Constants::MOVE_SPEED * delta;
        capsule->position.z += direction.z * Constants::MOVE_SPEED * delta;
    }

    if (IsKeyPressed(KEY_SPACE) && capsule->grounded) {
        capsule->velocity.y = Constants::JUMP_FORCE;
        capsule->grounded = false;
    }

    capsule->velocity.y += Constants::GRAVITY * delta;
    capsule->position.y += capsule->velocity.y * delta;

    if (capsule->position.y <= Constants::GROUND_Y + capsule->height / 2.0f) {
        capsule->position.y = Constants::GROUND_Y + capsule->height / 2.0f;
        capsule->velocity.y = 0.0f;
        capsule->grounded = true;
    }
}

void Capsule::CreateCapsule(CapsuleObj* capsule, World* world) {
    btCollisionShape* capsuleShape = new btCapsuleShape(capsule->radius, capsule->height - capsule->radius * 2);
    btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(capsule->position.x, capsule->position.y, capsule->position.z)));
    btScalar mass = 1.0f;
    btVector3 inertia(0, 0, 0);
    capsuleShape->calculateLocalInertia(mass, inertia);
    btRigidBody::btRigidBodyConstructionInfo capsuleCI(mass, motionState, capsuleShape, inertia);
    capsule->body = new btRigidBody(capsuleCI);
    capsule->body->setActivationState(DISABLE_DEACTIVATION);
    world->dynamicsWorld->addRigidBody(capsule->body);
}
