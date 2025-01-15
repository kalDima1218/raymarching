#include <iostream>
#include <iomanip>
#include <memory>
#include <string>
#include "Scene.h"
#include "lodepng.h"

void image_rendering(const std::shared_ptr<SDF> &s) {
    int w = 128;
    int h = 128;
    Vector cam_pos = {-2, -2, 0};
    Scene scene(w, h, 0, -0.7, cam_pos, {-5, 5, 0}, s, 1.3);
    std::vector<std::vector<unsigned char>> image_from_render = scene.render();
    const char* filename = "test.png";
    std::vector<unsigned char> image(4 * w * h);
    for(int i = 0; i < h; ++i){
        for(int j = 0; j < w; ++j){
            for(int k = 0; k < 3; ++k){
                image[4*(i*w + j) + k] = image_from_render[i][j];
            }
            image[4*(i*w + j) + 3] = 255;
        }
    }
    lodepng::encode(filename, image, w, h);
}

void console_rendering(const std::shared_ptr<SDF> &s){
    std::vector<char> chars = {'@', '@', '&', 'B', '9', '#', '$', 'S', 'G', 'W', 'M', 'H', '3', '5', '2', 'A', 'X', '%', 's', 'r', 'i', ';', ':', ',', '.', ' '};
    std::string line;
    std::string clear;
    int w = 128;
    int h = 128;
    for(int i = 0; i < w; i++){
        clear+="  ";
    }
    Vector cam_pos = {-2, -2, 0};
    Scene scene(w, h, 0, -0.7, cam_pos, {-5, 5, 0}, s, 1.3);
    int k = 0;
    for(int i = 0; i < 1; ++i){
//        system("clear");
        //system("cls");
        std::vector<std::vector<unsigned char>> image_from_render = scene.render();
        for(int j = 0; j < 15; j++) {
            std::cout << std::endl;
        }
        scene.set_preset(cam_pos, 0, static_cast<float>(k) * 0.1f);
        for(int i = 0; i < h; i++) {
            for(int j = 0; j < w; j++) {
                line += chars[int(round(image_from_render[i][j] / 10.2))];
                line += chars[int(round(image_from_render[i][j] / 10.2))];
            }
            if(line != clear) {
                std::cout << line << std::endl;
            }
            line.clear();
        }
        k++;
    }
}

int main(){
    std::vector<std::shared_ptr<SDF>> s(3);
    s[0] = std::make_shared<Sphere>(0, 0, 0, 0, 0, 0.65);
    s[1] = std::make_shared<Cube>(0, 0, 0, 0, 0, 1);
    s[2] = std::make_shared<Subtraction>(s[1], s[0]);

    console_rendering(s[2]);
    image_rendering(s[2]);

    return 0;
}
