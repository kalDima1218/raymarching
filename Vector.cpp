#include "Vector.h"

Vector::Vector() {
    vec_ = {0, 0, 0};
}

Vector::Vector(float x, float y, float z) : Vector::Vector() {
    vec_[0] = x;
    vec_[1] = y;
    vec_[2] = z;
}

Vector::Vector(const Vector &p) {
    *this = p;
}

Vector::Vector(const std::vector<float> &vec) : Vector::Vector() {
    vec_ = vec;
}

void Vector::rotate(float a, float b) {
    std::vector<float> res(3);
    res[0] = vec_[0] * cos(a) * cos(b) - vec_[2] * sin(a) * cos(b) + vec_[1] * sin(b);
    res[1] = -vec_[0] * cos(a) * sin(b) + vec_[2] * sin(a) * sin(b) + vec_[1] * cos(b);
    res[2] = vec_[0] * sin(a) + vec_[2] * cos(a);
    std::swap(vec_, res);
}

void Vector::normalize() {
    float len = sqrtf(powf(vec_[0], 2) + powf(vec_[1], 2) + powf(vec_[2], 2));
    for (size_t i = 0; i < 3; ++i) {
        vec_[i] /= len;
    }
}

float &Vector::operator[](int i) {
    return vec_[i];
}

float Vector::operator[](int i) const {
    return vec_[i];
}

Vector Vector::operator+(const Vector &v) const {
    return {vec_[0] + v.vec_[0], vec_[1] + v.vec_[1], vec_[2] + v.vec_[2]};
}

Vector Vector::operator-(const Vector &v) const {
    return {vec_[0] - v.vec_[0], vec_[1] - v.vec_[1], vec_[2] - v.vec_[2]};
}

Vector Vector::operator*(float k) const {
    return {vec_[0] * k, vec_[1] * k, vec_[2] * k};
}

Vector Vector::operator/(float k) const {
    return {vec_[0] / k, vec_[1] / k, vec_[2] / k};
}

void Vector::operator+=(const Vector &v) {
    for (size_t i = 0; i < 3; ++i) {
        vec_[i] += v.vec_[i];
    }
}

void Vector::operator-=(const Vector &v) {
    for (size_t i = 0; i < 3; ++i) {
        vec_[i] -= v.vec_[i];
    }
}

void Vector::operator*=(float k) {
    for (size_t i = 0; i < 3; ++i) {
        vec_[i] *= k;
    }
}

void Vector::operator/=(float k) {
    for (size_t i = 0; i < 3; ++i) {
        vec_[i] /= k;
    }
}