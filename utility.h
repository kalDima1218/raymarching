#include <cmath>

class Vector {
private:
    float _vec[3]{};
public:
    Vector(){}

    Vector(float x, float y, float z){
        _vec[0] = x;
        _vec[1] = y;
        _vec[2] = z;
    }

    Vector(const float *vec){
        for(int i = 0; i < 3; ++i){
            _vec[i] = vec[i];
        }
    }

    Vector(const Vector &p){
        for(int i = 0; i < 3; ++i){
            _vec[i] = p._vec[i];
        }
    }

    void rotate(float a, float b){
        float res[3];
        res[0] = _vec[0] * cos(a) * cos(b) - _vec[2] * sin(a) * cos(b) + _vec[1] * sin(b);
        res[1] = -_vec[0] * cos(a) * sin(b) + _vec[2] * sin(a) * sin(b) + _vec[1] * cos(b);
        res[2] = _vec[0] * sin(a) + _vec[2] * cos(a);
        std::swap(_vec, res);
    }

    void normalize(){
        float len = sqrt(pow(_vec[0], 2) + pow(_vec[1], 2) + pow(_vec[2], 2));
        _vec[0]/=len;
        _vec[1]/=len;
        _vec[2]/=len;
    }

    float& operator[](int i){
        return _vec[i];
    }

    Vector operator+(const Vector &v){
        return {_vec[0] + v._vec[0], _vec[1] + v._vec[1], _vec[2] + v._vec[2]};
    }

    Vector operator-(const Vector &v){
        return {_vec[0] - v._vec[0], _vec[1] - v._vec[1], _vec[2] - v._vec[2]};
    }

    Vector operator*(float k){
        return {_vec[0] * k, _vec[1] * k, _vec[2] * k};
    }

    void operator+=(const Vector &v){
        for(int i = 0; i < 3; ++i){
            _vec[i]+=v._vec[i];
        }
    }

    void operator*=(float k){
        for(float & i : _vec) {
            i *= k;
        }
    }

    void operator/=(float k){
        for(float & i : _vec) {
            i /= k;
        }
    }
};