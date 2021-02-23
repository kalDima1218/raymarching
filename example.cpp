#include<iostream>
#include"lodepng.h"
#include "raymarching.h"

void console_rendering(int w, int h, int threads, SDF* s){
    string chars[25] = {"@", "&", "B", "9", "#", "$", "S", "G", "H", "M", "h", "3", "5", "2", "A", "X", "%", "s", "r", "i", ";", ":", ",", ".", " "};
    string line;
    string clear;
    for (int i = 0; i < w; i++){
        clear+="  ";
    }
    unsigned char** res;
    int k = 0;
    while(true) {
        //system("clear");
        system("cls");
        res = scene(w, h, PI / 16 * k, -PI / 16 * k, 5, 6, s).thread_rendering(threads);
        for (int i = 0; i < 15; i++) {
            cout << endl;
        }
        for (int i = 0; i < h; i++) {
            for (int l = 0; l < w; l++) {
                line += chars[int(round(res[i][l] / 10.2) - 1)];
                line += chars[int(round(res[i][l] / 10.2) - 1)];
            }
            if (line != clear) {
                cout << line << endl;
            }
            line = "";
        }
        k++;
    }
}

void image_rendering(const vector<unsigned char>& image, unsigned width, unsigned height) {
    const char* filename = "test.png";
    //Encode the image
    unsigned error = lodepng::encode(filename, image, width, height);

    //if there's an error, display it
    if(error) std::cout << "encoder error " << error << ": "<< lodepng_error_text(error) << std::endl;
}

int main(){
    //picture settings
    int k = 2;
    int w = 2048 * k /2;
    int h = 1024 * k / 2;
    int threads = pow(2, int((log2f(w * h) / 2 + 1) / 2 + 1));//for pc: 2^((n + 1) / 2 + 1), n = log2f(w * h) / 2 + 1) / 2 => 2^((log2f(w * h) / 2 + 1) / 2 + 1)

    //objects
    SDF *s[3];
    Sphere sph = Sphere(0, 0, 0, 1, 1, 1, 0, 0, 0.65);
    Cube cu = Cube(0, 0, 0, 1, 1, 1, 0, 0, 1);
    s[0] = &sph;
    s[1] = &cu;
    obj_minus_obj obj = obj_minus_obj(s[1], s[0]);
    s[2] = &obj;

    //scene/rendering
    scene r = scene(w, h, PI / 4, -PI / 4, 5, 6, s[2]);
    clock_t time;
    time = clock();

    //image_rendering(r.thread_rendering_image(threads), w, h);
    //r.thread_rendering_with_hardwriting(threads);

    time = clock() - time;
    cout << static_cast<float>(time) / CLOCKS_PER_SEC << " " << threads << endl;

    //cin >> k;

    console_rendering(w, h, threads, s[2]);

    return 0;
}
