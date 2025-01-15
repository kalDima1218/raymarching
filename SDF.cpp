#include "SDF.h"

SDF::SDF(float pos_x, float pos_y, float pos_z, float a, float b) {
    pos_[0] = pos_x;
    pos_[1] = pos_y;
    pos_[2] = pos_z;

    rotate(a, b);
}

void SDF::rotate(float a, float b) {
    a_ = a;
    b_ = b;
}

float SDF::get_dist(float pos_x, float pos_y, float pos_z) {
    return 0;
}

float SDF::get_dist(Vector &pos) {
    return get_dist(pos[0], pos[1], pos[2]);
}

Vector SDF::get_normal(Vector pos) {
    float dist = get_dist(pos);
    Vector normal(dist - get_dist(pos[0] - 0.00001f, pos[1], pos[2]),
                  dist - get_dist(pos[0], pos[1] - 0.00001f, pos[2]),
                  dist - get_dist(pos[0], pos[1], pos[2] - 0.00001f));
    normal.normalize();
    return normal;
}


float Sphere::get_dist(float pos_x, float pos_y, float pos_z) {
    return sqrtf(powf(pos_x - pos_[0], 2.0f) + powf(pos_y - pos_[1], 2.0f) + powf(pos_z - pos_[2], 2.0f)) - r_;
}

Sphere::Sphere(float pos_x, float pos_y, float pos_z, float a, float b, float r) : SDF(pos_x, pos_y, pos_z, a, b) {
    r_ = r;
}


Cube::Cube(float pos_x, float pos_y, float pos_z, float a, float b, float side) : SDF(pos_x, pos_y, pos_z, a, b) {
    side_ = side;
}

float Cube::get_dist(float pos_x, float pos_y, float pos_z) {
    return std::max(std::max(std::abs(pos_x - pos_[0]), std::abs(pos_y - pos_[1])), std::abs(pos_z - pos_[2])) -
           side_ / 2;
}


Union::Union(const std::shared_ptr<SDF> &obj_1, const std::shared_ptr<SDF> &obj_2) : SDF() {
    obj_.resize(2);
    obj_[0] = obj_1;
    obj_[1] = obj_2;
}

float Union::get_dist(float pos_x, float pos_y, float pos_z) {
    return std::min(obj_[0]->get_dist(pos_x, pos_y, pos_z), obj_[1]->get_dist(pos_x, pos_y, pos_z));
}


Subtraction::Subtraction(const std::shared_ptr<SDF> &obj_1, const std::shared_ptr<SDF> &obj_2) : SDF() {
    obj_.resize(2);
    obj_[0] = obj_1;
    obj_[1] = obj_2;
}

float Subtraction::get_dist(float pos_x, float pos_y, float pos_z) {
    return std::max(obj_[0]->get_dist(pos_x, pos_y, pos_z), -obj_[1]->get_dist(pos_x, pos_y, pos_z));
}


Intersection::Intersection(const std::shared_ptr<SDF> &obj_1, const std::shared_ptr<SDF> &obj_2) : SDF() {
    obj_.resize(2);
    obj_[0] = obj_1;
    obj_[1] = obj_2;
}

float Intersection::get_dist(float pos_x, float pos_y, float pos_z) {
    return std::max(obj_[0]->get_dist(pos_x, pos_y, pos_z), obj_[1]->get_dist(pos_x, pos_y, pos_z));
}