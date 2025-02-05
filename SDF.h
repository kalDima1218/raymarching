#include <cmath>
#include <memory>
#include "Vector.h"

class SDF {
protected:
    Vector pos_;

    float a_ = 0;
    float b_ = 0;

public:
    SDF() = default;

    SDF(float pos_x, float pos_y, float pos_z, float a, float b);

    virtual ~SDF() = default;

    void rotate(float a, float b);

    virtual float get_dist(float pos_x, float pos_y, float pos_z) = 0;

    float get_dist(Vector &pos);

    Vector get_normal(Vector pos);
};

class Sphere : public SDF {
private:
    float r_;

public:
    Sphere(float pos_x, float pos_y, float pos_z, float a, float b, float r);

    float get_dist(float pos_x, float pos_y, float pos_z) override;
};

class Cube : public SDF {
private:
    float side_;

public:
    Cube(float pos_x, float pos_y, float pos_z, float a, float b, float side);

    float get_dist(float pos_x, float pos_y, float pos_z) override;
};

class Union : public SDF {
private:
    std::vector<std::unique_ptr<SDF>> obj_;

public:
    Union(std::unique_ptr<SDF> &&obj_1, std::unique_ptr<SDF> &&obj_2);

    float get_dist(float pos_x, float pos_y, float pos_z) override;
};

class Subtraction : public SDF {
private:
    std::vector<std::unique_ptr<SDF>> obj_;

public:
    Subtraction(std::unique_ptr<SDF> &&obj_1, std::unique_ptr<SDF> &&obj_2);

    float get_dist(float pos_x, float pos_y, float pos_z) override;
};

class Intersection : public SDF {
private:
    std::vector<std::unique_ptr<SDF>> obj_;

public:
    Intersection(std::unique_ptr<SDF> &&obj_1, std::unique_ptr<SDF> &&obj_2);

    float get_dist(float pos_x, float pos_y, float pos_z) override;
};