#include <btBulletDynamicsCommon.h>



struct bulletObject {
    int id;
    float r, g, b;
    bool hit;
    btRigidBody* body;
    bulletObject(btRigidBody* b, int i, float r0, float g0, float b0) : body(b), id(i), r(r0), g(g0), b(b0), hit(false) {}
};
