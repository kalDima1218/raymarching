#include <thread>
#include <cmath>
#include "sdf.h"

class Scene{
private:
    int _width;
    int _height;
    Vector _light_pos = {-5, 5, 0};
    Vector _cam_pos;
    Vector _cam_dir;
    Vector _pix_right_vec;
    Vector _pix_down_vec;
    Vector _display_center;
    float _pixel_size;
    float _fov = 1.3;
    SDF* _sdf;
public:
    unsigned char** image;
public:
    Scene(int width, int height, float a, float b, float *cam_pos, SDF* sdf){
        _width = width;
        _height = height;

        image = new unsigned char*[_height];
        for (int i = 0; i < _height; i++) {
            image[i] = new unsigned char[_width];
        }

        _sdf = sdf;

        _pixel_size = 2 * (tan(_fov / 2) / height);

        set_preset(cam_pos, a, b);
    }

    // a вокруг оси y, b вокруг оси z
    void set_preset(float *cam_pos, float a, float b){
        _cam_pos = Vector(cam_pos);
        _cam_dir = Vector(1, 0, 0);
        _pix_right_vec = Vector(0, _pixel_size, 0);
        _pix_down_vec = Vector(0, 0, -_pixel_size);
        _cam_dir.rotate(a, b);
        _pix_right_vec.rotate(a, b);
        _pix_down_vec.rotate(a, b);
        _display_center = _cam_pos + _cam_dir;
    }

    void render (){
        Vector ray_dir;
        Vector pos;
        Vector light_dir;
        for(int row = -_height/2; row < _height/2; ++row){
            for(int col = -_width/2; col < _width/2; ++col){
                ray_dir = _display_center + _pix_down_vec*row + _pix_right_vec*col - _cam_pos;
                ray_dir.normalize();
                pos = _cam_pos;
                float dist = 0;
                float delta = _sdf->get_dist(pos);
                for(int i = 0; i < 100 and dist < 100 and delta > 0.0001; ++i){
                    dist+=delta;
                    pos+=ray_dir*delta;
                    delta = _sdf->get_dist(pos);
                }
                if(delta <= 0.0001){
                    light_dir = _light_pos - pos;
                    light_dir.normalize();
                    Vector normal = _sdf->get_normal(pos);
                    image[int(row + (_height / 2))][int(col + (_width / 2))] = std::abs(light_dir[0]*normal[0] + light_dir[1]*normal[1] + light_dir[2]*normal[2]) * 170 + 60;
                }
                else{
                    image[int(row + (_height / 2))][int(col + (_width / 2))] = 255;
                }
            }
        }
    }
};
