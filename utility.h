#include <cmath>

float* rotate_vector(float *vec, float a, float b){
    auto *res = new float[3];
    res[0] = vec[0]*cos(a)*cos(b) - vec[2]*sin(a)*cos(b) + vec[1]*sin(b);
    res[1] = -vec[0]*cos(a)*sin(b) + vec[2]*sin(a)*sin(b) + vec[1]*cos(b);
    res[2] = vec[0]*sin(a) + vec[2]*cos(a);
    return res;
}

void inplace_rotate_vector(float *vec, float a, float b){
    auto *res = new float[3];
    res[0] = vec[0]*cos(a)*cos(b) - vec[2]*sin(a)*cos(b) + vec[1]*sin(b);
    res[1] = -vec[0]*cos(a)*sin(b) + vec[2]*sin(a)*sin(b) + vec[1]*cos(b);
    res[2] = vec[0]*sin(a) + vec[2]*cos(a);
    *vec = *res;
    delete[] res;
}

float* normalize(float *vec){
    auto *res = new float[3];
    float len = sqrt(pow(vec[0], 2) + pow(vec[1], 2) + pow(vec[2], 2));
    res[0] = vec[0]/len;
    res[1] = vec[1]/len;
    res[2] = vec[2]/len;
    return res;
}

void inplace_normalize(float *vec){
    float len = sqrt(pow(vec[0], 2) + pow(vec[1], 2) + pow(vec[2], 2));
    vec[0]/=len;
    vec[1]/=len;
    vec[2]/=len;
}

float* sum_vectors(float *a, float *b){
    auto *res = new float[3];
    res[0] = a[0] + b[0];
    res[1] = a[1] + b[1];
    res[2] = a[2] + b[2];
    return res;
}

void inplace_sum_vectors(float *a, float *b){
    a[0]+=b[0];
    a[1]+=b[1];
    a[2]+=b[2];
}

float* multiply_vector(float *vec, float l){
    auto *res = new float[3];
    res[0] = l * vec[0];
    res[1] = l * vec[1];
    res[2] = l * vec[2];
    return res;
}

void inplace_multiply_vector(float *vec, float l){
    vec[0]*=l;
    vec[1]*=l;
    vec[2]*=l;
}