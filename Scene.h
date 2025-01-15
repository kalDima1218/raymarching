#include <cmath>
#include <memory>
#include <cstdint>
#include "SDF.h"

class Scene {
private:
    int32_t width_;
    int32_t height_;
    float pixel_size_;
    float fov_;
    Vector light_pos_;
    Vector cam_pos_;
    Vector cam_dir_;
    Vector pix_right_vec_;
    Vector pix_down_vec_;
    Vector display_center_;
    std::shared_ptr<SDF> sdf_;
    std::vector<std::vector<unsigned char>> mutable image_;

public:

    Scene(int32_t width, int32_t height, float a, float b, const Vector &cam_pos, const Vector &light_pos, const std::shared_ptr<SDF> &sdf, float fov);

    // a вокруг оси y, b вокруг оси z
    void set_preset(const Vector &cam_pos, float a, float b);

    const std::vector<std::vector<unsigned char>>& render() const;

    int32_t get_width() const;
    int32_t get_height() const;

    const Vector& get_cam_pos() const;
    const Vector& get_cam_dir() const;
};
