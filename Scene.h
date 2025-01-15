#include <cmath>
#include <memory>
#include <cstdint>
#include "SDF.h"

class Scene {
private:
    int width_;
    int height_;
    float pixel_size_;
    float fov_;
    Vector light_pos_;
    Vector cam_pos_;
    Vector cam_dir_;
    Vector pix_right_vec_;
    Vector pix_down_vec_;
    Vector display_center_;
    std::shared_ptr<SDF> sdf_;
    std::vector<std::vector<unsigned char>> image_;

public:

    Scene(int width, int height, float a, float b, const Vector &cam_pos, const Vector &light_pos, const std::shared_ptr<SDF> &sdf, float fov);

    // a вокруг оси y, b вокруг оси z
    void set_preset(const Vector &cam_pos, float a, float b);

    std::vector<std::vector<unsigned char>> &render();
};
