#include <iostream>
#include <memory>
#include <string>
#include "Scene.h"
#include "lodepng.h"

void image_rendering(const Scene &scene) {
    int32_t w = 128;
    int32_t h = 128;
    Vector cam_pos = {-2, -2, 0};
    const std::vector<std::vector<unsigned char>>& image_from_render = scene.render();
    const char *filename = "test.png";
    std::vector<unsigned char> image(4 * w * h);
    for (int32_t i = 0; i < h; ++i) {
        for (int32_t j = 0; j < w; ++j) {
            for (int32_t k = 0; k < 3; ++k) {
                image[4 * (i * w + j) + k] = image_from_render[i][j];
            }
            image[4 * (i * w + j) + 3] = 255;
        }
    }
    lodepng::encode(filename, image, w, h);
}

void console_rendering(Scene &scene) {
    std::vector<char> chars = {'@', '@', '&', 'B', '9', '#', '$', 'S', 'G', 'W', 'M', 'H', '3', '5', '2', 'A', 'X', '%', 's', 'r', 'i', ';', ':', ',', '.', ' '};
    std::string line;
    std::string clear;
    for (int32_t i = 0; i < scene.get_width(); i++) {
        clear += ' ';
    }
    Vector cam_pos = {-2, -2, 0};
    int k = 0;
    for (int32_t iter = 0; iter < 1; ++iter) {
//        system("clear");
//        system("cls");
        std::vector<std::vector<unsigned char>> image_from_render = scene.render();
        for (int32_t j = 0; j < 15; j++) {
            std::cout << '\n';
        }
        scene.set_preset(cam_pos, 0, static_cast<float>(k) * 0.1f);
        for (int32_t i = 0; i < scene.get_height(); i++) {
            for (int32_t j = 0; j < scene.get_width(); j++) {
                line += chars[static_cast<int32_t>(round(image_from_render[i][j] / 10.2))];
                line += chars[static_cast<int32_t>(round(image_from_render[i][j] / 10.2))];
            }
            if (line != clear) {
                std::cout << line << '\n';
            }
            line.clear();
        }
        k++;
    }
}

int main() {
    int32_t w = 128;
    int32_t h = 128;

    std::vector<std::unique_ptr<SDF>> s(3);
    s[0] = std::make_unique<Sphere>(0, 0, 0, 0, 0, 0.65);
    s[1] = std::make_unique<Cube>(0, 0, 0, 0, 0, 1);
    s[2] = std::make_unique<Subtraction>(std::move(s[1]), std::move(s[0]));

    Vector cam_pos = {-2, -2, 0};

    Scene scene(w, h, 0, -0.7, cam_pos, {-5, 5, 0}, std::move(s[2]), 1.3);

    image_rendering(scene);
    console_rendering(scene);

    return 0;
}
