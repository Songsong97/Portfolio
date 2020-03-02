#include "scene.h"

Scene::Scene() : camera() {
    testSoftShadow();
}

bool Scene::getIntersection(Ray ray, Intersection *intersection) const {
#ifdef RAY_MARCHING
    float maxDist = 50.f;
    float epsilon = 0.0001;
    float t = 0;
    Point3f p = ray.origin;
    while (t < maxDist) {
        float sd = FLT_MAX;
        Primitive *pp;
        for (int i = 0; i < primitives.size(); i++) {
            Primitive *primitive = primitives[i].get();
            float temp = primitive->sdf(p);
            if (temp < sd) {
                sd = temp;
                pp = primitive;
            }
        }
        t += sd;
        p = ray.origin + t * ray.direction;
        if (sd < epsilon) {
            *intersection = pp->generateIntersection(p);
            intersection->t = t;
            return true;
        }
    }
    return false;
#else
    float minT = FLT_MAX;
    bool isHit = false;
    for (int i = 0; i < primitives.size(); i++) {
        Primitive *primitive = primitives[i].get();
        Opt<Intersection> in = primitive->getIntersection(ray);
        if (in) {
            Intersection v = in.value();
            if (v.t < minT) {
                minT = v.t;
                *intersection = v;
                isHit = true;
            }
        }
    }
    return isHit;
#endif
}

/*------------Below are test scene construction----------------
 */

void Scene::test1() {
    uPtr<Primitive> p = mkU<Primitive>("Sphere");
    p->surface = mkU<Sphere>();
    primitives.push_back(std::move(p));
}

void Scene::test2() {
    uPtr<Primitive> p = mkU<Primitive>("SquarePlane");
    p->surface = mkU<SquarePlane>();
    primitives.push_back(std::move(p));
}

void Scene::test3() {
    uPtr<Primitive> p = mkU<Primitive>("Sphere");
    p->surface = mkU<Sphere>(Transform(Vector3f(-1, 1, 0), Vector3f(), Vector3f(2, 4, 2)));
    primitives.push_back(std::move(p));

    p = mkU<Primitive>("SquarePlane");
    p->surface = mkU<SquarePlane>(Transform(Vector3f(0, 0, -5), Vector3f(0, 30, 0), Vector3f(10, 10, 1)));
    primitives.push_back(std::move(p));
}

void Scene::test4() {
    uPtr<Primitive> p = mkU<Primitive>("Sphere1");
    p->surface = mkU<Sphere>(Transform(Vector3f(0, 0, 1), Vector3f(0, 0, 45), Vector3f(2, 1, 1)));
    primitives.push_back(std::move(p));

    p = mkU<Primitive>("Sphere2");
    p->surface = mkU<Sphere>(Transform(Vector3f(0.25, 0, 0), Vector3f(0, 0, 0), Vector3f(0.5, 0.5, 5)));
    primitives.push_back(std::move(p));

    p = mkU<Primitive>("SquarePlane");
    p->surface = mkU<SquarePlane>(Transform(Vector3f(0, -0.5, 0), Vector3f(90, 0, 0), Vector3f(5, 5, 1)));
    primitives.push_back(std::move(p));
}

void Scene::cornell_box() {
    // Set camera
    camera.eye = Vector3f(0, 0.5, 12);
    camera.fov = 50;
    camera.recomputeAttributes();

    // Set light source
    uPtr<Light> l = mkU<PointLight>(Point3f(0, 4.9, 0), Color3f(1, 1, 1));
    lights.push_back(std::move(l));

    uPtr<Primitive> p = mkU<Primitive>("Back Wall");
    p->surface = mkU<SquarePlane>(Transform(Vector3f(0, 0, -5), Vector3f(0, 0, 0), Vector3f(10, 10, 1)));
    p->material = mkU<LambertMaterial>(Color3f(1, 1, 1));
    primitives.push_back(std::move(p));

    p = mkU<Primitive>("Right Wall");
    p->surface = mkU<SquarePlane>(Transform(Vector3f(5, 0, 0), Vector3f(0, -90, 0), Vector3f(10, 10, 1)));
    p->material = mkU<LambertMaterial>(Color3f(0, 1, 0));
    primitives.push_back(std::move(p));

    p = mkU<Primitive>("Left Wall");
    p->surface = mkU<SquarePlane>(Transform(Vector3f(-5, 0, 0), Vector3f(0, 90, 0), Vector3f(10, 10, 1)));
    p->material = mkU<LambertMaterial>(Color3f(1, 0, 0));
    primitives.push_back(std::move(p));

    p = mkU<Primitive>("Ceiling");
    p->surface = mkU<SquarePlane>(Transform(Vector3f(0, 5, 0), Vector3f(90, 0, 0), Vector3f(10, 10, 1)));
    p->material = mkU<LambertMaterial>(Color3f(1, 1, 1));
    primitives.push_back(std::move(p));

    p = mkU<Primitive>("Floor");
    p->surface = mkU<SquarePlane>(Transform(Vector3f(0, -5, 0), Vector3f(-90, 0, 0), Vector3f(10, 10, 1)));
    p->material = mkU<LambertMaterial>(Color3f(1, 1, 1));
    primitives.push_back(std::move(p));

    p = mkU<Primitive>("Shiny Sphere");
    p->surface = mkU<Sphere>(Transform(Vector3f(-2, -5, 0), Vector3f(0, 0, 0), Vector3f(5, 5, 5)));
    p->material = mkU<BlinnPhongMaterial>(Color3f(0.75, 0, 0.75));
    primitives.push_back(std::move(p));

    p = mkU<Primitive>("Transparent Sphere 1");
    p->surface = mkU<Sphere>(Transform(Vector3f(-3, 0, 2), Vector3f(0, 0, 0), Vector3f(4, 4, 4)));
    p->material = mkU<BlinnPhongMaterial>(Color3f(1, 1, 1)); // refractive_specular
    primitives.push_back(std::move(p));

    p = mkU<Primitive>("Inside Transparent Sphere 1");
    p->surface = mkU<Sphere>(Transform(Vector3f(-3, 0, 2), Vector3f(0, 0, 0), Vector3f(2.5, 2.5, 2.5)));
    p->material = mkU<LambertMaterial>(Color3f(1, 1, 1)); // air_bubble_mat
    primitives.push_back(std::move(p));

    p = mkU<Primitive>("Transparent Sphere Green");
    p->surface = mkU<Sphere>(Transform(Vector3f(3, -2, -3), Vector3f(0, 0, 0), Vector3f(4, 4, 4)));
    p->material = mkU<BlinnPhongMaterial>(Color3f(0.15, 1, 0.15)); // refractive_specular2
    primitives.push_back(std::move(p));

    p = mkU<Primitive>("Inside Opaque 1");
    p->surface = mkU<Sphere>(Transform(Vector3f(3, -2, -3), Vector3f(0, 0, 0), Vector3f(1, 1, 1)));
    p->material = mkU<LambertMaterial>(Color3f(1, 1, 1));
    primitives.push_back(std::move(p));

    p = mkU<Primitive>("Reflective Sphere Blue1");
    p->surface = mkU<Sphere>(Transform(Vector3f(-3, 3.5, -3), Vector3f(0, 0, 0), Vector3f(3, 3, 3)));
    p->material = mkU<BlinnPhongMaterial>(Color3f(0.25, 0.5, 1)); // mirrored_specular2
    primitives.push_back(std::move(p));

    p = mkU<Primitive>("Transparent Sphere 3");
    p->surface = mkU<Sphere>(Transform(Vector3f(3, 3.5, -3), Vector3f(0, 0, 0), Vector3f(3, 3, 3)));
    p->material = mkU<BlinnPhongMaterial>(Color3f(1, 1, 1)); //refractive_specular
    primitives.push_back(std::move(p));

    p = mkU<Primitive>("Reflective Sphere Blue2");
    p->surface = mkU<Sphere>(Transform(Vector3f(3, 3.5, -3), Vector3f(0, 0, 0), Vector3f(1, 1, 1)));
    p->material = mkU<BlinnPhongMaterial>(Color3f(0.25, 0.5, 1)); // mirrored_specular2
    primitives.push_back(std::move(p));

    p = mkU<Primitive>("Yellow Sphere");
    p->surface = mkU<Sphere>(Transform(Vector3f(0, 0, 0), Vector3f(0, 0, 0), Vector3f(2, 2, 2)));
    p->material = mkU<LambertMaterial>(Color3f(1, 1, 0.25));
    primitives.push_back(std::move(p));
}

void Scene::cornell_box_2lights_simple() {
    cornell_box();

    // Change light source
    lights.erase(lights.begin());
    uPtr<Light> l = mkU<PointLight>(Point3f(-2, 4.9, 0), Color3f(1, 0.5, 0.5));
    lights.push_back(std::move(l));
    l = mkU<PointLight>(Point3f(2, 4.9, 0), Color3f(0.5, 1, 0.5));
    lights.push_back(std::move(l));
}
