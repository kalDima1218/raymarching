#include "Scene.h"

Scene::Scene(int32_t width, int32_t height, float a, float b, const Vector &cam_pos, const Vector &light_pos, std::unique_ptr<SDF> &&sdf, float fov) {
    width_ = width;
    height_ = height;
    light_pos_ = light_pos;
    std::swap(sdf_, sdf);
    fov_ = fov;
    pixel_size_ = 2 * (tan(fov_ / 2) / static_cast<float>(height));
    image_ = std::vector<std::vector<unsigned char>>(height_, std::vector<unsigned char>(width_));
    set_preset(cam_pos, a, b);
}

// a вокруг оси y, b вокруг оси z
void Scene::set_preset(const Vector &cam_pos, float a, float b) {
    cam_pos_ = Vector(cam_pos);
    cam_dir_ = Vector(1, 0, 0);
    pix_right_vec_ = Vector(0, pixel_size_, 0);
    pix_down_vec_ = Vector(0, 0, -pixel_size_);
    cam_dir_.rotate(a, b);
    pix_right_vec_.rotate(a, b);
    pix_down_vec_.rotate(a, b);
    display_center_ = cam_pos_ + cam_dir_;
}

const std::vector<std::vector<unsigned char>> &Scene::render() const {
    Vector ray_dir;
    Vector pos;
    Vector light_dir;
    for (int32_t row = -height_ / 2; row < height_ / 2; ++row) {
        for (int32_t col = -width_ / 2; col < width_ / 2; ++col) {
            ray_dir = display_center_ + pix_down_vec_ * row + pix_right_vec_ * col - cam_pos_;
            ray_dir.normalize();
            pos = cam_pos_;
            float dist = 0;
            float delta = sdf_->get_dist(pos);
            for (uint32_t i = 0; i < 100 && dist < 100 && delta > 0.0001; ++i) {
                dist += delta;
                pos += ray_dir * delta;
                delta = sdf_->get_dist(pos);
            }
            if (delta <= 0.0001) {
                light_dir = light_pos_ - pos;
                light_dir.normalize();
                Vector normal = sdf_->get_normal(pos);
                image_[static_cast<int>(row + (height_ / 2))][static_cast<int>(col + (width_ / 2))] = static_cast<unsigned char>(std::abs(light_dir[0] * normal[0] + light_dir[1] * normal[1] + light_dir[2] * normal[2]) * 170 + 60);
            } else {
                image_[static_cast<int>(row + (height_ / 2))][static_cast<int>(col + (width_ / 2))] = 255;
            }
        }
    }
    return image_;
}

int32_t Scene::get_width() const {
    return width_;
}

int32_t Scene::get_height() const {
    return height_;
}

const Vector &Scene::get_cam_pos() const {
    return cam_pos_;
}

const Vector &Scene::get_cam_dir() const {
    return cam_dir_;
}