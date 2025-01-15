#include <cmath>
#include <vector>
#include <cstdint>

class Vector {
private:
    std::vector<float> vec_;

public:
    Vector();

    Vector(float x, float y, float z);

    Vector(const Vector &p);

    explicit Vector(const std::vector<float> &vec);

    void rotate(float a, float b);

    void normalize();

    float &operator[](int i);
    float operator[](int i) const;

    Vector operator+(const Vector &v);
    Vector operator-(const Vector &v);
    Vector operator*(float k);
    Vector operator/(float k);

    void operator+=(const Vector &v);
    void operator-=(const Vector &v);
    void operator*=(float k);
    void operator/=(float k);
};