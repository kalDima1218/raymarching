#include <thread>
#include <cmath>
#include "sdf.h"

class Scene{
private:
    int _width;
    int _height;
    float _light_pos[3] = {-5, 5, 0};
    float *_cam_pos;
    float *_cam_dir = new float[3];
    float *_pix_right_vec = new float[3];
    float *_pix_down_vec = new float[3];
    float *_display_center = new float[3];
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

    ~Scene(){
        delete[] _cam_pos;
        delete[] _cam_dir;
        delete[] _pix_right_vec;
        delete[] _pix_down_vec;
        delete[] _display_center;
    }

    // a вокруг оси y, b вокруг оси z
    void set_preset(float *cam_pos, float a, float b){
        _cam_pos = cam_pos;
        _cam_dir[0] = 1;
        _cam_dir[1] = 0;
        _cam_dir[2] = 0;
        _pix_right_vec[0] = 0;
        _pix_right_vec[1] = _pixel_size;
        _pix_right_vec[2] = 0;
        _pix_down_vec[0] = 0;
        _pix_down_vec[1] = 0;
        _pix_down_vec[2] = -_pixel_size;
        inplace_rotate_vector(_cam_dir, a, b);
        inplace_rotate_vector(_pix_right_vec, a, b);
        inplace_rotate_vector(_pix_down_vec, a, b);
        *_display_center = *_cam_pos;
        inplace_sum_vectors(_display_center, _cam_dir);
    }

    void render (){
        float *ray_dir = new float[3];
        float *pos = new float[3];
        float *light_dir = new float[3];
        for(int row = -_height/2; row < _height/2; ++row){
            for(int col = -_width/2; col < _width/2; ++col){
                ray_dir[0] = _display_center[0] + row*_pix_down_vec[0] + col*_pix_right_vec[0] - _cam_pos[0];
                ray_dir[1] = _display_center[1] + row*_pix_down_vec[1] + col*_pix_right_vec[1] - _cam_pos[1];
                ray_dir[2] = _display_center[2] + row*_pix_down_vec[2] + col*_pix_right_vec[2] - _cam_pos[2];
                inplace_normalize(ray_dir);
                pos[0] = _cam_pos[0];
                pos[1] = _cam_pos[1];
                pos[2] = _cam_pos[2];
                float dist = 0;
                float delta = _sdf->get_dist(pos);
                for(int i = 0; i < 100 and dist < 100 and delta > 0.0001; ++i){
                    dist+=delta;
                    pos[0]+=delta*ray_dir[0];
                    pos[1]+=delta*ray_dir[1];
                    pos[2]+=delta*ray_dir[2];
                    delta = _sdf->get_dist(pos);
                }
                if(delta <= 0.0001){
                    light_dir[0] = _light_pos[0] - pos[0];
                    light_dir[1] = _light_pos[1] - pos[1];
                    light_dir[2] = _light_pos[2] - pos[2];
                    inplace_normalize(light_dir);
                    float *normal = _sdf->get_normal(pos);
                    image[int(row + (_height / 2))][int(col + (_width / 2))] = (light_dir[0]*normal[0] + light_dir[1]*normal[1] + light_dir[2]*normal[2]) * 170 + 60;
                    delete[] normal;
                }
                else{
                    image[int(row + (_height / 2))][int(col + (_width / 2))] = 255;
                }
            }
        }
        delete[] ray_dir;
        delete[] pos;
        delete[] light_dir;
    }
};
