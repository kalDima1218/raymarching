#include <cmath>
#include <vector>
#include <cstdint>

class Vector {
private:
    std::vector<float> vec_;

public:
    Vector();

    Vector(float x, float y, float z);

    Vector(const Vector &v);

    explicit Vector(const std::vector<float> &vec);

    ~Vector() = default;

    Vector &operator=(const Vector &v);

    void rotate(float a, float b);

    void normalize();

    float &operator[](int i);
    float operator[](int i) const;

    Vector operator+(const Vector &v) const;
    Vector operator-(const Vector &v) const;
    Vector operator*(float k) const;
    Vector operator/(float k) const;

    Vector operator+=(const Vector &v);
    Vector operator-=(const Vector &v);
    Vector operator*=(float k);
    Vector operator/=(float k);
};