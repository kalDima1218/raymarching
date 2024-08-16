#include <cmath>
#include "utility.h"

class SDF{
protected:
    Vector _pos;
    
    float _a;
    float _b;
public:
    SDF(){}

    SDF(float pos_x, float pos_y, float pos_z, float a, float b) {
        _pos[0] = pos_x;
        _pos[1] = pos_y;
        _pos[2] = pos_z;

        rotate(a, b);
    }

    void rotate(float a, float b){
        _a = a;
        _b = b;
    }

    virtual float _get_dist(float, float, float){ return 0; }

    float get_dist(Vector &pos){
        return _get_dist(pos[0], pos[1], pos[2]);
    }

    Vector get_normal(Vector pos){
        float dist = get_dist(pos);
        Vector normal(dist - _get_dist(pos[0] - 0.00001, pos[1], pos[2]), dist - _get_dist(pos[0], pos[1] - 0.00001, pos[2]), dist - _get_dist(pos[0], pos[1], pos[2] - 0.00001));
        normal.normalize();
        return normal;
    }
};

class Sphere : public SDF{
private:
    float _r;
public:
    Sphere(float pos_x, float pos_y, float pos_z, float a, float b, float r) : SDF(pos_x, pos_y, pos_z, a, b){
        _r = r;
    }

    float _get_dist(float pos_x, float pos_y, float pos_z) override{
        return sqrt(pow(pos_x - _pos[0], 2.0) + pow(pos_y - _pos[1], 2.0) + pow(pos_z - _pos[2], 2.0)) - _r;
    }
};

class Cube : public SDF{
private:
    float _side;
public:
    Cube(float pos_x, float pos_y, float pos_z, float a, float b, float side) : SDF(pos_x, pos_y, pos_z, a, b){
        _side = side;
    }

    float _get_dist(float pos_x, float pos_y, float pos_z) override{
        return std::max(std::max(std::abs(pos_x - _pos[0]), std::abs(pos_y - _pos[1])), std::abs(pos_z - _pos[2])) - _side/2;
    }
};

class Union : public SDF{
private:
    SDF *obj[2];
public:
    Union(SDF *obj_1, SDF *obj_2) : SDF(){
        obj[0] = obj_1;
        obj[1] = obj_2;
    }

    float _get_dist(float pos_x, float pos_y, float pos_z) override{
        return std::min(obj[0]->_get_dist(pos_x, pos_y, pos_z), obj[1]->_get_dist(pos_x, pos_y, pos_z));
    }
};

class Subtraction : public SDF{
private:
    SDF *obj[2];
public:
    Subtraction(SDF *obj_1, SDF *obj_2) : SDF(){
        obj[0] = obj_1;
        obj[1] = obj_2;
    }

    float _get_dist(float pos_x, float pos_y, float pos_z) override{
        return std::max(obj[0]->_get_dist(pos_x, pos_y, pos_z), -obj[1]->_get_dist(pos_x, pos_y, pos_z));
    }
};

class Intersection : public SDF{
private:
    SDF *obj[2];
public:
    Intersection(SDF *obj_1, SDF *obj_2) : SDF(){
        obj[0] = obj_1;
        obj[1] = obj_2;
    }

    float _get_dist(float pos_x, float pos_y, float pos_z) override{
        return std::max(obj[0]->_get_dist(pos_x, pos_y, pos_z), obj[1]->_get_dist(pos_x, pos_y, pos_z));
    }
};