#include <iostream>
#include <iomanip>
#include <string>
#include "raymarching.h"
#include "lodepng.h"

using namespace std;

void image_rendering(SDF* s) {
    int w = 128;
    int h = 128;
    float *cam_pos = new float[3]{-2, 0, 0};
    Scene scene(w, h, 0, -1.4, cam_pos, s);
    scene.render();
    const char* filename = "test.png";
    vector<unsigned char> image(4*w*h);
    for(int i = 0; i < h; ++i){
        for(int j = 0; j < w; ++j){
            for(int k = 0; k < 3; ++k){
                image[4*(i*w + j) + k] = scene.image[i][j];
            }
            image[4*(i*w + j) + 3] = 255;
        }
    }
    //Encode the image
    unsigned error = lodepng::encode(filename, image, w, h);

    //if there's an error, display it
    if(error) std::cout << "encoder error " << error << ": "<< lodepng_error_text(error) << std::endl;
}

void console_rendering(SDF* s){
    string chars[26] = {"@", "@", "&", "B", "9", "#", "$", "S", "G", "H", "M", "height", "3", "5", "2", "A", "X", "%", "s", "r", "i", ";", ":", ",", ".", " "};
    string line = "";
    string clear = "";
    int w = 128;
    int h = 128;
    for(int i = 0; i < w; i++){
        clear+="  ";
    }
    float *cam_pos = new float[3];//{-2, 0, 0};
    cam_pos[0] = -2;
    cam_pos[1] = 0;
    cam_pos[2] = 0;
    Scene scene(w, h, 0, 0, cam_pos, s);
    int k = 0;
    for(int i = 0; i < 1; ++i){
        system("clear");
        //system("cls");
        //scene.thread_rendering(threads);
        scene.render();
        for(int i = 0; i < 15; i++) {
            cout << endl;
        }
        scene.set_preset(cam_pos, 0, k*0.1);
        for(int i = 0; i < h; i++) {
            for(int j = 0; j < w; j++) {
                //cout << (int)scene.image[i][j] << " ";
                line += chars[int(round(scene.image[i][j] / 10.2))];
                line += chars[int(round(scene.image[i][j] / 10.2))];
            }
            if(line != clear) {
                cout << line << endl;
            }
            line.clear();
        }
        k++;
    }
}

int main(){

//    int k = 2;
//    int threads = pow(2, int((log2f(w * h) / 2 + 1) / 2 + 1));
//
    SDF *s[3];
    Sphere sph = Sphere(0, 0, 0, 1, 1, 1, 0, 0, 0.65);
    Cube cu = Cube(0, 0, 0, 1, 1, 1, 0, 0, 1);
    s[0] = &sph;
    s[1] = &cu;
    Subtraction obj = Subtraction(s[1], s[0]);
    s[2] = &obj;

    //console_rendering(s[2]);
    image_rendering(s[2]);
    return 0;

}
