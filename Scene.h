#include <cmath>
#include <memory>
#include "SDF.h"

class Scene {
private:
    int _width;
    int _height;
    float _pixel_size;
    float _fov;
    Vector _light_pos;
    Vector _cam_pos;
    Vector _cam_dir;
    Vector _pix_right_vec;
    Vector _pix_down_vec;
    Vector _display_center;
    std::shared_ptr<SDF> _sdf;

    std::vector<std::vector<unsigned char>> _image;

public:

    Scene(int width, int height, float a, float b, const Vector &cam_pos, const Vector &light_pos, const std::shared_ptr<SDF> &sdf, float fov);

    // a вокруг оси y, b вокруг оси z
    void set_preset(const Vector &cam_pos, float a, float b);

    std::vector<std::vector<unsigned char>> &render();
};
