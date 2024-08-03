#include <thread>
#include <cmath>
#include "sdf.h"

class Scene{
private:
    int width;
    int height;
    float cam_dist;
    float cam_angel = 39;
    float light_pos[3] = {-5, 5, 0};
    float rotation_matrix[3][3];
    float cam_coordinates[3];
    float max_dist;
    SDF* sdf;
    float start_dist;
    float pos_0[3];
    float u[3];
    float v[3];
public:
    unsigned char** image;
public:
    Scene(int w, int h, float a, float b, float cam_dist, float max_dist, SDF* scene){
        this->width = w;
        this->height = h;

        image = new unsigned char*[h];
        for (int i = 0; i < h; i++) {
            image[i] = new unsigned char[w];
        }

        this->cam_dist = cam_dist;

        this->max_dist = max_dist;

        sdf = &scene[0];

        set_preset(a, b);
    }

     void set_preset(float a, float b){
        rotation_matrix[0][0] = cos(a) * cos(b);
        rotation_matrix[0][1] = -cos(a) * sin(b);
        rotation_matrix[0][2] = sin(a);
        rotation_matrix[1][0] = sin(b);
        rotation_matrix[1][1] = cos(b);
        rotation_matrix[1][2] = 0;
        rotation_matrix[2][0] = -cos(b) * sin(a);
        rotation_matrix[2][1] = sin(a) * sin(b);
        rotation_matrix[2][2] = cos(a);

        cam_coordinates[0] = rotation_matrix[0][0] * cam_dist;
        cam_coordinates[1] = rotation_matrix[1][0] * cam_dist;
        cam_coordinates[2] = rotation_matrix[2][0] * cam_dist;

        start_dist = sdf->get_dist(cam_coordinates[0], cam_coordinates[1], cam_coordinates[2]);

        pos_0[0] = -rotation_matrix[0][0];
        pos_0[1] = -rotation_matrix[1][0];
        pos_0[2] = -rotation_matrix[2][0];

        float pixel_size = 2 * (tan(cam_angel / 2) / height);

        u[0] = rotation_matrix[0][2] * pixel_size;
        u[1] = rotation_matrix[1][2] * pixel_size;
        u[2] = rotation_matrix[2][2] * pixel_size;

        v[0] = rotation_matrix[0][1] * pixel_size;
        v[1] = rotation_matrix[1][1] * pixel_size;
        v[2] = rotation_matrix[2][1] * pixel_size;
    }

     void classical_rendering (){
        float ray_dir[3];
        float dist;
        float ray_pos[3];
        float delta;
        float light_dist;
        float light_dir[3];
        float sdf_value;
        float x;
        float y;
        float z;
        float just_dist;
        float normal[3];
        for(int i = -int(height / 2); i < int(height / 2); i++){
            for (int l = -int(width / 2); l < int(width / 2); l++){
                ray_dir[0] = pos_0[0] + (l * u[0]) + (i * v[0]);
                ray_dir[1] = pos_0[1] + (l * u[1]) + (i * v[1]);
                ray_dir[2] = pos_0[2] + (l * u[2]) + (i * v[2]);
                dist = 0;
                ray_pos[0] = cam_coordinates[0] + (start_dist * ray_dir[0]);
                ray_pos[1] = cam_coordinates[1] + (start_dist * ray_dir[1]);
                ray_pos[2] = cam_coordinates[2] + (start_dist * ray_dir[2]);

                delta = sdf->get_dist(ray_pos[0], ray_pos[1], ray_pos[2]);
                for(int k = 0; k < 100; k++){
                    if (delta < 0.001){
                        ray_pos[0] = cam_coordinates[0] + ((dist + delta) * ray_dir[0]);
                        ray_pos[1] = cam_coordinates[1] + ((dist + delta) * ray_dir[1]);
                        ray_pos[2] = cam_coordinates[2] + ((dist + delta) * ray_dir[2]);

                        light_dist = sqrtf(powf(ray_pos[0] - light_pos[0], 2) + powf(ray_pos[1] - light_pos[1], 2) + powf(ray_pos[2] - light_pos[2], 2));

                        light_dir[0] = (ray_pos[0] - light_pos[0]) / light_dist;
                        light_dir[1] = (ray_pos[1] - light_pos[1]) / light_dist;
                        light_dir[2] = (ray_pos[2] - light_pos[2]) / light_dist;

                        sdf_value = sdf->get_dist(ray_pos[0], ray_pos[1], ray_pos[2]);
                        x = sdf_value - sdf->get_dist(ray_pos[0] - 0.00001, ray_pos[1], ray_pos[2]);
                        y = sdf_value - sdf->get_dist(ray_pos[0], ray_pos[1] - 0.00001, ray_pos[2]);
                        z = sdf_value - sdf->get_dist(ray_pos[0], ray_pos[1], ray_pos[2] - 0.00001);

                        just_dist = sqrtf(powf(x, 2) + powf(y, 2) + powf(z, 2));//tested

                        normal[0] = x / just_dist;
                        normal[1] = y / just_dist;
                        normal[2] = z / just_dist;

                        dist = fabs(((normal[0] * light_dir[0]) + (normal[1] * light_dir[1]) + (normal[2] * light_dir[2])) / (sqrtf(powf(normal[0], 2) + powf(normal[1], 2) + powf(normal[2], 2)) * sqrtf(powf(light_dir[0], 2) + powf(light_dir[1], 2) + powf(light_dir[2], 2)))) * 170 + 60;//tested
                        break;
                    }
                    if (delta > max_dist){
                        dist = 255;
                        break;
                    }
                    dist+=delta;
                    ray_pos[0] = cam_coordinates[0] + (dist * ray_dir[0]);
                    ray_pos[1] = cam_coordinates[1] + (dist * ray_dir[1]);
                    ray_pos[2] = cam_coordinates[2] + (dist * ray_dir[2]);

                    delta = sdf->get_dist(ray_pos[0], ray_pos[1], ray_pos[2]);
                }
                image[int(i + (height / 2))][int(l + (width / 2))] = dist;
            }
        }
    }

     void stripe_rendering (int h1, int h2){
        float ray_dir[3];
        float dist;
        float ray_pos[3];
        float delta;
        float light_dist;
        float light_dir[3];
        float sdf_value;
        float x;
        float y;
        float z;
        float just_dist;
        float normal[3];
        for (int i = h1; i < h2; i++){
            for (int l = -int(width / 2); l < int(width / 2); l++) {
                ray_dir[0] = pos_0[0] + (l * u[0]) + (i * v[0]);
                ray_dir[1] = pos_0[1] + (l * u[1]) + (i * v[1]);
                ray_dir[2] = pos_0[2] + (l * u[2]) + (i * v[2]);
                dist = 0;
                ray_pos[0] = cam_coordinates[0] + (start_dist * ray_dir[0]);
                ray_pos[1] = cam_coordinates[1] + (start_dist * ray_dir[1]);
                ray_pos[2] = cam_coordinates[2] + (start_dist * ray_dir[2]);
                delta = sdf->get_dist(ray_pos[0], ray_pos[1], ray_pos[2]);
                for(int k = 0; k < 100; k++){
                    if (delta < 0.001){
                        ray_pos[0] = cam_coordinates[0] + ((dist + delta) * ray_dir[0]);
                        ray_pos[1] = cam_coordinates[1] + ((dist + delta) * ray_dir[1]);
                        ray_pos[2] = cam_coordinates[2] + ((dist + delta) * ray_dir[2]);

                        light_dist = sqrtf(powf(ray_pos[0] - light_pos[0], 2) + powf(ray_pos[1] - light_pos[1], 2) + powf(ray_pos[2] - light_pos[2], 2));//tested
                        light_dir[0] = (ray_pos[0] - light_pos[0]) / light_dist;
                        light_dir[1] = (ray_pos[1] - light_pos[1]) / light_dist;
                        light_dir[2] = (ray_pos[2] - light_pos[2]) / light_dist;

                        sdf_value = sdf->get_dist(ray_pos[0], ray_pos[1], ray_pos[2]);//tested
                        x = sdf_value - sdf->get_dist(ray_pos[0] - 0.00001, ray_pos[1], ray_pos[2]);
                        y = sdf_value - sdf->get_dist(ray_pos[0], ray_pos[1] - 0.00001, ray_pos[2]);
                        z = sdf_value - sdf->get_dist(ray_pos[0], ray_pos[1], ray_pos[2] - 0.00001);

                        just_dist = sqrtf(powf(x, 2) + powf(y, 2) + powf(z, 2));

                        normal[0] = x / just_dist;
                        normal[1] = y / just_dist;
                        normal[2] = z / just_dist;
                        dist = fabsf(((normal[0] * light_dir[0]) + (normal[1] * light_dir[1]) + (normal[2] * light_dir[2])) / (sqrtf(powf(normal[0], 2) + powf(normal[1], 2) + powf(normal[2], 2)) * sqrtf(powf(light_dir[0], 2) + powf(light_dir[1], 2) + powf(light_dir[2], 2)))) * 170 + 60;
                        break;
                    }
                    if (delta > max_dist){
                        dist = 255;
                        break;
                    }
                    dist+=delta;
                    ray_pos[0] = cam_coordinates[0] + (dist * ray_dir[0]);
                    ray_pos[1] = cam_coordinates[1] + (dist * ray_dir[1]);
                    ray_pos[2] = cam_coordinates[2] + (dist * ray_dir[2]);
                    delta = sdf->get_dist(ray_pos[0], ray_pos[1], ray_pos[2]);
                }
                image[int(i + (height / 2))][int(l + (width / 2))] = dist;
            }
        }
    }

     void thread_rendering(int num_threads){
        auto* threads = new std::thread[num_threads];
        for(int i = -num_threads / 2; i < num_threads / 2; i++){
            threads[i + num_threads / 2] = std::thread(&Scene::stripe_rendering, this, int(height / num_threads) * i, int(height / num_threads) * (i + 1));
        }
        for(int i = 0; i < num_threads; i++) {
            threads[i].join();
        }
         delete[] threads;
    }
};
