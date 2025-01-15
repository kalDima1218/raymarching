#include "Scene.h"

Scene::Scene(int width, int height, float a, float b, const Vector &cam_pos, const Vector &light_pos, const std::shared_ptr<SDF> &sdf, float fov) {
    _width = width;
    _height = height;
    _light_pos = light_pos;
    _image = std::vector<std::vector<unsigned char>>(_height, std::vector<unsigned char>(_width));
    _sdf = sdf;
    _fov = fov;
    _pixel_size = 2 * (tan(_fov / 2) / static_cast<float>(height));
    set_preset(cam_pos, a, b);
}

// a вокруг оси y, b вокруг оси z
void Scene::set_preset(const Vector &cam_pos, float a, float b) {
    _cam_pos = Vector(cam_pos);
    _cam_dir = Vector(1, 0, 0);
    _pix_right_vec = Vector(0, _pixel_size, 0);
    _pix_down_vec = Vector(0, 0, -_pixel_size);
    _cam_dir.rotate(a, b);
    _pix_right_vec.rotate(a, b);
    _pix_down_vec.rotate(a, b);
    _display_center = _cam_pos + _cam_dir;
}

std::vector<std::vector<unsigned char>> &Scene::render() {
    Vector ray_dir;
    Vector pos;
    Vector light_dir;
    for (int row = -_height / 2; row < _height / 2; ++row) {
        for (int col = -_width / 2; col < _width / 2; ++col) {
            ray_dir = _display_center + _pix_down_vec * row + _pix_right_vec * col - _cam_pos;
            ray_dir.normalize();
            pos = _cam_pos;
            float dist = 0;
            float delta = _sdf->get_dist(pos);
            for (int i = 0; i < 100 and dist < 100 and delta > 0.0001; ++i) {
                dist += delta;
                pos += ray_dir * delta;
                delta = _sdf->get_dist(pos);
            }
            if (delta <= 0.0001) {
                light_dir = _light_pos - pos;
                light_dir.normalize();
                Vector normal = _sdf->get_normal(pos);
                _image[int(row + (_height / 2))][int(col + (_width / 2))] = std::abs(light_dir[0] * normal[0] + light_dir[1] * normal[1] + light_dir[2] * normal[2]) * 170 + 60;
            } else {
                _image[int(row + (_height / 2))][int(col + (_width / 2))] = 255;
            }
        }
    }
    return _image;
}