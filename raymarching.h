/*  Raymarching lib. Vector engine library for rendering scenes in high resolution.
    Copyright © 2020 Dmitry Kalugin

    Raymarching lib is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    Raymarching lib is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
    USA

Email to contact:kalugin180806@gmail.com*/

#include<cmath>
#include<thread>
#include<fstream>
#include<string>
#include <vector>

using namespace std;

#define PI 3.14159265

//double smin(double a, double b, float k = 0.1){
//double h = max(0, 0.5 + 0.5 * (b - a) / k);
//return max(a, b, h) - k * h * (1 - h);
//}

int get_threads(int w, int h){
    return pow(2, int((log2l(w * h) / 2 + 1) / 2 + 1));//for pc: 2^((n + 1) / 2 + 1), n = log2l(w * h) / 2 + 1) / 2 => 2^((log2l(w * h) / 2 + 1) / 2 + 1)
}

class SDF{
public:
    double x;
    double y;
    double z;

    double scale_x;
    double scale_y;
    double scale_z;

    double a;
    double b;

    double rotation_matrix [3][3]{};

    SDF(double x_local, double y_local, double z_local, double scale_x_local, double scale_y_local, double scale_z_local, float a_local, float b_local){
        x = x_local;
        y = y_local;
        z = z_local;

        scale_x = 1/scale_x_local;
        scale_y = 1/scale_y_local;
        scale_z = 1/scale_z_local;

        a = a_local * PI;
        b = b_local * PI;

        rotation_matrix[0][0] = cos(a) * cos(b);
        rotation_matrix[0][1] = -cos(a) * sin(b);
        rotation_matrix[0][2] = sin(a);
        rotation_matrix[1][0] = sin(b);
        rotation_matrix[1][1] = cos(b);
        rotation_matrix[1][2] = 0;
        rotation_matrix[2][0] = -cos(b) * sin(a);
        rotation_matrix[2][1] = sin(a) * sin(b);
        rotation_matrix[2][2] = cos(a);
    }

    virtual inline double get_value(double x_local, double y_local, double z_loca){ return 0; }

    virtual inline double formula(double x_local, double y_local, double z_local){ return 0; }
};
class Sphere : public SDF{
public:
    double r;
    Sphere(double x_local, double y_local, double z_local, double scale_x_local, double scale_y_local, double scale_z_local, float a_local, float b_local, double r_local) :SDF(x_local, y_local, z_local, scale_x_local, scale_y_local, scale_z_local, a_local, b_local){
        r = r_local;
    }

    inline double get_value(double x_local, double y_local, double z_local) override{
        return (double)formula((double)x_local + (x*scale_x), (double)y_local + (y*scale_y), (double)z_local + (z*scale_z));
    }

    inline double formula(double x_local, double y_local, double z_local) override{
        return (double) sqrt((double) pow((double)x_local, 2.0) + pow((double)y_local, 2.0) + pow((double)z_local, 2.0)) - r;
    }
};

class Cube : public SDF{
public:
    double side;
    Cube(double x_local, double y_local, double z_local, double scale_x_local, double scale_y_local, double scale_z_local, float a_local, float b_local, double side_local) :SDF(x_local, y_local, z_local, scale_x_local, scale_y_local, scale_z_local, a_local, b_local){
        side = side_local;
    }

    inline double get_value(double x_local, double y_local, double z_local) override{
        return (double)formula((double)x_local + (x*scale_x), (double)y_local + (y*scale_y), (double)z_local + (z*scale_z));
    }

    inline double formula(double x_local, double y_local, double z_local) override{
        return (double)fmax((double)fmax((double)fabs(x_local), (double)fabs(y_local)), (double)fabs(z_local)) - side/2;
    }
};

class obj_plus_obj : public SDF{
public:
    SDF *obj[2]{};
    obj_plus_obj(SDF *obj_1, SDF *obj_2) : SDF(0, 0, 0, 0, 0, 0, 0, 0){
        obj[0] = &obj_1[0];
        obj[1] = &obj_2[0];
    }

    inline double get_value(double x_local, double y_local, double z_local) override{
        double res_1 = obj[0]->get_value(x_local, y_local, z_local);
        double res_2 = obj[1]->get_value(x_local, y_local, z_local);
        if(res_1 > res_2){
            return res_2;
        }
        else if(res_1 < res_2){
            return res_1;
        }
        else{
            return res_1;
        }
    }
    inline double formula(double x_local, double y_local, double z_local){
        return 0;
    }
};

class obj_minus_obj : public SDF{
public:
    SDF *obj[2];
    obj_minus_obj(SDF *obj_1, SDF *obj_2) : SDF(0, 0, 0, 0, 0, 0, 0, 0){
        obj[0] = &obj_1[0];
        obj[1] = &obj_2[0];
    }

    inline double get_value(double x_local, double y_local, double z_local){
        double res_1 = obj[0]->get_value(x_local, y_local, z_local);
        double res_2 = -obj[1]->get_value(x_local, y_local, z_local);

        if(res_1 < res_2){
            return res_2;
        }
        else if(res_1 > res_2){
            return res_1;
        }
        else{
            return res_1;
        }
    }
    inline double formula(double x_local, double y_local, double z_local){
        return 0;
    }
};

class scene{
private:
    int w;
    int h;
    double a;
    double b;
    double cam_dist;
    double cam_angel = 39;
    double light_pos[3] = {-5, 5, 0};
    double rotation_matrix[3][3];
    double cam_coordinates[3];
    double  max_dist;
    SDF* sdf;
    double orig_dist;
    double pos_0[3];
    double u[3];
    double v[3];
    unsigned char** s_res;
    vector<unsigned char> s_res_image;
public:
    scene(int w_local, int h_local, double a_local, double b_local, double camdist, double max_dist_local, SDF* scene){
        w = w_local;
        h = h_local;

        a = a_local;
        b = b_local;

        cam_dist = camdist;

        max_dist = max_dist_local;

        sdf = &scene[0];

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

        orig_dist = sdf->get_value(cam_coordinates[0], cam_coordinates[1], cam_coordinates[2]);

        pos_0[0] = -rotation_matrix[0][0];
        pos_0[1] = -rotation_matrix[1][0];
        pos_0[2] = -rotation_matrix[2][0];

        double pixel_size = 2 * (tan(cam_angel / 2) / h);

        u[0] = rotation_matrix[0][2] * pixel_size;
        u[1] = rotation_matrix[1][2] * pixel_size;
        u[2] = rotation_matrix[2][2] * pixel_size;

        v[0] = rotation_matrix[0][1] * pixel_size;
        v[1] = rotation_matrix[1][1] * pixel_size;
        v[2] = rotation_matrix[2][1] * pixel_size;
    }

    //vector<double> raydir (double x, double y){
    //vector<double> result = {pos_0[0] + (x * u[0]) + (y * v[0]), pos_0[1] + (x * u[1]) + (y * v[1]), pos_0[2] + (x * u[2]) + (y * v[2])};
    //return result;
    //}

    //vector<double> ray (double dist, vector<double> dir){
    //vector<double> result = {cam_coordinates[0] + (dist *dir[0]), cam_coordinates[1] + (dist *dir[1]), cam_coordinates[2] + (dist *dir[2])};
    //return result;
    //}

    inline unsigned char** main_rendering (){
        unsigned char** res;
        res = new unsigned char*[h];
        for (int i = 0; i < h; i++){
            res[i] = new unsigned char[w];
        }
        //vector<double> ray_dir;
        double ray_dir[3];
        double dist;
        //vector<double> ray_pos;
        double ray_pos[3];
        double delta;
        double light_dist;
        double light_dir[3];
        double sdf_value;
        double x;
        double y;
        double z;
        double just_dist;
        double normal[3];
        for(int i = -int(h / 2); i < int(h / 2); i++){
            for (int l = -int(w / 2); l < int(w / 2); l++){
                //ray_dir = raydir(l, i);
                ray_dir[0] = pos_0[0] + (l * u[0]) + (i * v[0]);
                ray_dir[1] = pos_0[1] + (l * u[1]) + (i * v[1]);
                ray_dir[2] = pos_0[2] + (l * u[2]) + (i * v[2]);
                dist = 0;
                //ray_pos = ray(orig_dist, ray_dir);
                ray_pos[0] = cam_coordinates[0] + (orig_dist * ray_dir[0]);
                ray_pos[1] = cam_coordinates[1] + (orig_dist * ray_dir[1]);
                ray_pos[2] = cam_coordinates[2] + (orig_dist * ray_dir[2]);

                delta = sdf->get_value(ray_pos[0], ray_pos[1], ray_pos[2]);
                for(int k =0; k < 100; k++){
                    if (delta < 0.001){
                        //ray_pos = ray(dist + delta, ray_dir);
                        ray_pos[0] = cam_coordinates[0] + ((dist + delta) * ray_dir[0]);
                        ray_pos[1] = cam_coordinates[1] + ((dist + delta) * ray_dir[1]);
                        ray_pos[2] = cam_coordinates[2] + ((dist + delta) * ray_dir[2]);

                        light_dist = sqrt(pow(ray_pos[0] - light_pos[0], 2) + pow(ray_pos[1] - light_pos[1], 2) + pow(ray_pos[2] - light_pos[2], 2));//tested

                        light_dir[0] = (ray_pos[0] - light_pos[0]) / light_dist;
                        light_dir[1] = (ray_pos[1] - light_pos[1]) / light_dist;
                        light_dir[2] = (ray_pos[2] - light_pos[2]) / light_dist;

                        sdf_value = sdf->get_value(ray_pos[0], ray_pos[1], ray_pos[2]);//tested
                        x = sdf_value - sdf->get_value(ray_pos[0] - 0.0001, ray_pos[1], ray_pos[2]);
                        y = sdf_value - sdf->get_value(ray_pos[0], ray_pos[1] - 0.0001, ray_pos[2]);
                        z = sdf_value - sdf->get_value(ray_pos[0], ray_pos[1], ray_pos[2] - 0.0001);

                        just_dist = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));//tested

                        normal[0] = x / just_dist;
                        normal[1] = y / just_dist;
                        normal[2] = z / just_dist;

                        dist = fabs(((normal[0] * light_dir[0]) + (normal[1] * light_dir[1]) + (normal[2] * light_dir[2])) / (sqrt(pow(normal[0], 2) + pow(normal[1], 2) + pow(normal[2], 2)) * sqrt(pow(light_dir[0], 2) + pow(light_dir[1], 2) + pow(light_dir[2], 2)))) * 170 + 60;//tested
                        break;
                    }
                    if (delta > max_dist){
                        dist = 255;
                        break;
                    }
                    dist+=delta;
                    //ray_pos = ray(dist, ray_dir);
                    ray_pos[0] = cam_coordinates[0] + (dist * ray_dir[0]);
                    ray_pos[1] = cam_coordinates[1] + (dist * ray_dir[1]);
                    ray_pos[2] = cam_coordinates[2] + (dist * ray_dir[2]);

                    delta = sdf->get_value(ray_pos[0], ray_pos[1], ray_pos[2]);
                }
                res[int(i + (h / 2))][int(l + (w / 2))] = dist;
            }
        }
        return res;
    }

    inline unsigned char** main_rendering_with_hardwriting (){
        unsigned char** res;
        res = new unsigned char*[h];
        for (int i = 0; i < h; i++){
            res[i] = new unsigned char[w];
        }
        //vector<double> ray_dir;
        double ray_dir[3];
        double dist;
        //vector<double> ray_pos;
        double ray_pos[3];
        double delta;
        double light_dist;
        double light_dir[3];
        double sdf_value;
        double x;
        double y;
        double z;
        double just_dist;
        double normal[3];
        std::ofstream data;
        data.open("data.txt");
        for(int i = -int(h / 2); i < int(h / 2); i++){
            for (int l = -int(w / 2); l < int(w / 2); l++){
                //ray_dir = raydir(l, i);
                ray_dir[0] = pos_0[0] + (l * u[0]) + (i * v[0]);
                ray_dir[1] = pos_0[1] + (l * u[1]) + (i * v[1]);
                ray_dir[2] = pos_0[2] + (l * u[2]) + (i * v[2]);
                dist = 0;
                //ray_pos = ray(orig_dist, ray_dir);
                ray_pos[0] = cam_coordinates[0] + (orig_dist * ray_dir[0]);
                ray_pos[1] = cam_coordinates[1] + (orig_dist * ray_dir[1]);
                ray_pos[2] = cam_coordinates[2] + (orig_dist * ray_dir[2]);

                delta = sdf->get_value(ray_pos[0], ray_pos[1], ray_pos[2]);
                for(int k =0; k < 100; k++){
                    if (delta < 0.001){
                        //ray_pos = ray(dist + delta, ray_dir);
                        ray_pos[0] = cam_coordinates[0] + ((dist + delta) * ray_dir[0]);
                        ray_pos[1] = cam_coordinates[1] + ((dist + delta) * ray_dir[1]);
                        ray_pos[2] = cam_coordinates[2] + ((dist + delta) * ray_dir[2]);

                        light_dist = sqrt(pow(ray_pos[0] - light_pos[0], 2) + pow(ray_pos[1] - light_pos[1], 2) + pow(ray_pos[2] - light_pos[2], 2));//tested

                        light_dir[0] = (ray_pos[0] - light_pos[0]) / light_dist;
                        light_dir[1] = (ray_pos[1] - light_pos[1]) / light_dist;
                        light_dir[2] = (ray_pos[2] - light_pos[2]) / light_dist;

                        sdf_value = sdf->get_value(ray_pos[0], ray_pos[1], ray_pos[2]);//tested
                        x = sdf_value - sdf->get_value(ray_pos[0] - 0.0001, ray_pos[1], ray_pos[2]);
                        y = sdf_value - sdf->get_value(ray_pos[0], ray_pos[1] - 0.0001, ray_pos[2]);
                        z = sdf_value - sdf->get_value(ray_pos[0], ray_pos[1], ray_pos[2] - 0.0001);

                        just_dist = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));//tested

                        normal[0] = x / just_dist;
                        normal[1] = y / just_dist;
                        normal[2] = z / just_dist;

                        dist = fabs(((normal[0] * light_dir[0]) + (normal[1] * light_dir[1]) + (normal[2] * light_dir[2])) / (sqrt(pow(normal[0], 2) + pow(normal[1], 2) + pow(normal[2], 2)) * sqrt(pow(light_dir[0], 2) + pow(light_dir[1], 2) + pow(light_dir[2], 2)))) * 170 + 60;//tested
                        break;
                    }
                    if (delta > max_dist){
                        dist = 255;
                        break;
                    }
                    dist+=delta;
                    //ray_pos = ray(dist, ray_dir);
                    ray_pos[0] = cam_coordinates[0] + (dist * ray_dir[0]);
                    ray_pos[1] = cam_coordinates[1] + (dist * ray_dir[1]);
                    ray_pos[2] = cam_coordinates[2] + (dist * ray_dir[2]);

                    delta = sdf->get_value(ray_pos[0], ray_pos[1], ray_pos[2]);
                }
                data << dist << " ";
            }
            data << std::endl;
        }
        data.close();
        return res;
    }

    inline void stripe_rendering_with_hardwriting (int h1, int h2, short int threads){
        //vector<double> ray_dir;
        double ray_dir[3];
        double dist;
        //vector<double> ray_pos;
        double ray_pos[3];
        double delta;
        double light_dist;
        double light_dir[3];
        double sdf_value;
        double x;
        double y;
        double z;
        double just_dist;
        double normal[3];
        ofstream data;
        string f_str = "data" + to_string(threads) + ".txt";
        data.open(f_str);
        for (int i = h1; i < h2; i++){
            for (int l = -int(w / 2); l < int(w / 2); l++) {
                //ray_dir = raydir(l, i);
                ray_dir[0] = pos_0[0] + (l * u[0]) + (i * v[0]);
                ray_dir[1] = pos_0[1] + (l * u[1]) + (i * v[1]);
                ray_dir[2] = pos_0[2] + (l * u[2]) + (i * v[2]);
                dist = 0;
                //ray_pos = ray(orig_dist, ray_dir);
                ray_pos[0] = cam_coordinates[0] + (orig_dist * ray_dir[0]);
                ray_pos[1] = cam_coordinates[1] + (orig_dist * ray_dir[1]);
                ray_pos[2] = cam_coordinates[2] + (orig_dist * ray_dir[2]);
                delta = sdf->get_value(ray_pos[0], ray_pos[1], ray_pos[2]);
                for(int k = 0; k < 100; k++){
                    if (delta < 0.001){
                        //ray_pos = ray(dist + delta, ray_dir);
                        ray_pos[0] = cam_coordinates[0] + ((dist + delta) * ray_dir[0]);
                        ray_pos[1] = cam_coordinates[1] + ((dist + delta) * ray_dir[1]);
                        ray_pos[2] = cam_coordinates[2] + ((dist + delta) * ray_dir[2]);

                        light_dist = sqrt(pow(ray_pos[0] - light_pos[0], 2) + pow(ray_pos[1] - light_pos[1], 2) + pow(ray_pos[2] - light_pos[2], 2));//tested
                        light_dir[0] = (ray_pos[0] - light_pos[0]) / light_dist;
                        light_dir[1] = (ray_pos[1] - light_pos[1]) / light_dist;
                        light_dir[2] = (ray_pos[2] - light_pos[2]) / light_dist;

                        sdf_value = sdf->get_value(ray_pos[0], ray_pos[1], ray_pos[2]);//tested
                        x = sdf_value - sdf->get_value(ray_pos[0] - 0.0001, ray_pos[1], ray_pos[2]);
                        y = sdf_value - sdf->get_value(ray_pos[0], ray_pos[1] - 0.0001, ray_pos[2]);
                        z = sdf_value - sdf->get_value(ray_pos[0], ray_pos[1], ray_pos[2] - 0.0001);

                        just_dist = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));

                        normal[0] = x / just_dist;
                        normal[1] = y / just_dist;
                        normal[2] = z / just_dist;

                        dist = fabs(((normal[0] * light_dir[0]) + (normal[1] * light_dir[1]) + (normal[2] * light_dir[2])) / (sqrt(pow(normal[0], 2) + pow(normal[1], 2) + pow(normal[2], 2)) * sqrt(pow(light_dir[0], 2) + pow(light_dir[1], 2) + pow(light_dir[2], 2)))) * 170 + 60;
                        break;
                    }
                    if (delta > max_dist){
                        dist = 255;
                        break;
                    }
                    dist+=delta;
                    //ray_pos = ray(dist, ray_dir);
                    ray_pos[0] = cam_coordinates[0] + (dist * ray_dir[0]);
                    ray_pos[1] = cam_coordinates[1] + (dist * ray_dir[1]);
                    ray_pos[2] = cam_coordinates[2] + (dist * ray_dir[2]);
                    delta = sdf->get_value(ray_pos[0], ray_pos[1], ray_pos[2]);
                }
                data << dist << " ";
            }
            data << endl;
        }
        data.close();
    }

    inline void stripe_rendering_image (int h1, int h2){
        //vector<double> ray_dir;
        double ray_dir[3];
        double dist;
        //vector<double> ray_pos;
        double ray_pos[3];
        double delta;
        double light_dist;
        double light_dir[3];
        double sdf_value;
        double x;
        double y;
        double z;
        double just_dist;
        double normal[3];
        for (int i = h1; i < h2; i++){
            for (int l = -int(w / 2); l < int(w / 2); l++) {
                //ray_dir = raydir(l, i);
                ray_dir[0] = pos_0[0] + (l * u[0]) + (i * v[0]);
                ray_dir[1] = pos_0[1] + (l * u[1]) + (i * v[1]);
                ray_dir[2] = pos_0[2] + (l * u[2]) + (i * v[2]);
                dist = 0;
                //ray_pos = ray(orig_dist, ray_dir);
                ray_pos[0] = cam_coordinates[0] + (orig_dist * ray_dir[0]);
                ray_pos[1] = cam_coordinates[1] + (orig_dist * ray_dir[1]);
                ray_pos[2] = cam_coordinates[2] + (orig_dist * ray_dir[2]);
                delta = sdf->get_value(ray_pos[0], ray_pos[1], ray_pos[2]);
                for(int k = 0; k < 100; k++){
                    if (delta < 0.001){
                        //ray_pos = ray(dist + delta, ray_dir);
                        ray_pos[0] = cam_coordinates[0] + ((dist + delta) * ray_dir[0]);
                        ray_pos[1] = cam_coordinates[1] + ((dist + delta) * ray_dir[1]);
                        ray_pos[2] = cam_coordinates[2] + ((dist + delta) * ray_dir[2]);

                        light_dist = sqrt(pow(ray_pos[0] - light_pos[0], 2) + pow(ray_pos[1] - light_pos[1], 2) + pow(ray_pos[2] - light_pos[2], 2));//tested
                        light_dir[0] = (ray_pos[0] - light_pos[0]) / light_dist;
                        light_dir[1] = (ray_pos[1] - light_pos[1]) / light_dist;
                        light_dir[2] = (ray_pos[2] - light_pos[2]) / light_dist;

                        sdf_value = sdf->get_value(ray_pos[0], ray_pos[1], ray_pos[2]);//tested
                        x = sdf_value - sdf->get_value(ray_pos[0] - 0.0001, ray_pos[1], ray_pos[2]);
                        y = sdf_value - sdf->get_value(ray_pos[0], ray_pos[1] - 0.0001, ray_pos[2]);
                        z = sdf_value - sdf->get_value(ray_pos[0], ray_pos[1], ray_pos[2] - 0.0001);

                        just_dist = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));

                        normal[0] = x / just_dist;
                        normal[1] = y / just_dist;
                        normal[2] = z / just_dist;

                        dist = fabs(((normal[0] * light_dir[0]) + (normal[1] * light_dir[1]) + (normal[2] * light_dir[2])) / (sqrt(pow(normal[0], 2) + pow(normal[1], 2) + pow(normal[2], 2)) * sqrt(pow(light_dir[0], 2) + pow(light_dir[1], 2) + pow(light_dir[2], 2)))) * 170 + 60;
                        break;
                    }
                    if (delta > max_dist){
                        dist = 255;
                        break;
                    }
                    dist+=delta;
                    //ray_pos = ray(dist, ray_dir);
                    ray_pos[0] = cam_coordinates[0] + (dist * ray_dir[0]);
                    ray_pos[1] = cam_coordinates[1] + (dist * ray_dir[1]);
                    ray_pos[2] = cam_coordinates[2] + (dist * ray_dir[2]);
                    delta = sdf->get_value(ray_pos[0], ray_pos[1], ray_pos[2]);
                }
                s_res_image[4 * w * int(i + (h / 2)) + 4 * int(l + (w / 2)) + 0] = dist;
                s_res_image[4 * w * int(i + (h / 2)) + 4 * int(l + (w / 2)) + 1] = dist;
                s_res_image[4 * w * int(i + (h / 2)) + 4 * int(l + (w / 2)) + 2] = dist;
                s_res_image[4 * w * int(i + (h / 2)) + 4 * int(l + (w / 2)) + 3] = 255;
            }
        }
    }

    inline void stripe_rendering (int h1, int h2, short int threads){
        //vector<double> ray_dir;
        double ray_dir[3];
        double dist;
        //vector<double> ray_pos;
        double ray_pos[3];
        double delta;
        double light_dist;
        double light_dir[3];
        double sdf_value;
        double x;
        double y;
        double z;
        double just_dist;
        double normal[3];
        for (int i = h1; i < h2; i++){
            for (int l = -int(w / 2); l < int(w / 2); l++) {
                //ray_dir = raydir(l, i);
                ray_dir[0] = pos_0[0] + (l * u[0]) + (i * v[0]);
                ray_dir[1] = pos_0[1] + (l * u[1]) + (i * v[1]);
                ray_dir[2] = pos_0[2] + (l * u[2]) + (i * v[2]);
                dist = 0;
                //ray_pos = ray(orig_dist, ray_dir);
                ray_pos[0] = cam_coordinates[0] + (orig_dist * ray_dir[0]);
                ray_pos[1] = cam_coordinates[1] + (orig_dist * ray_dir[1]);
                ray_pos[2] = cam_coordinates[2] + (orig_dist * ray_dir[2]);
                delta = sdf->get_value(ray_pos[0], ray_pos[1], ray_pos[2]);
                for(int k = 0; k < 100; k++){
                    if (delta < 0.001){
                        //ray_pos = ray(dist + delta, ray_dir);
                        ray_pos[0] = cam_coordinates[0] + ((dist + delta) * ray_dir[0]);
                        ray_pos[1] = cam_coordinates[1] + ((dist + delta) * ray_dir[1]);
                        ray_pos[2] = cam_coordinates[2] + ((dist + delta) * ray_dir[2]);

                        light_dist = sqrt(pow(ray_pos[0] - light_pos[0], 2) + pow(ray_pos[1] - light_pos[1], 2) + pow(ray_pos[2] - light_pos[2], 2));//tested
                        light_dir[0] = (ray_pos[0] - light_pos[0]) / light_dist;
                        light_dir[1] = (ray_pos[1] - light_pos[1]) / light_dist;
                        light_dir[2] = (ray_pos[2] - light_pos[2]) / light_dist;

                        sdf_value = sdf->get_value(ray_pos[0], ray_pos[1], ray_pos[2]);//tested
                        x = sdf_value - sdf->get_value(ray_pos[0] - 0.0001, ray_pos[1], ray_pos[2]);
                        y = sdf_value - sdf->get_value(ray_pos[0], ray_pos[1] - 0.0001, ray_pos[2]);
                        z = sdf_value - sdf->get_value(ray_pos[0], ray_pos[1], ray_pos[2] - 0.0001);

                        just_dist = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));

                        normal[0] = x / just_dist;
                        normal[1] = y / just_dist;
                        normal[2] = z / just_dist;

                        dist = fabs(((normal[0] * light_dir[0]) + (normal[1] * light_dir[1]) + (normal[2] * light_dir[2])) / (sqrt(pow(normal[0], 2) + pow(normal[1], 2) + pow(normal[2], 2)) * sqrt(pow(light_dir[0], 2) + pow(light_dir[1], 2) + pow(light_dir[2], 2)))) * 170 + 60;
                        break;
                    }
                    if (delta > max_dist){
                        dist = 255;
                        break;
                    }
                    dist+=delta;
                    //ray_pos = ray(dist, ray_dir);
                    ray_pos[0] = cam_coordinates[0] + (dist * ray_dir[0]);
                    ray_pos[1] = cam_coordinates[1] + (dist * ray_dir[1]);
                    ray_pos[2] = cam_coordinates[2] + (dist * ray_dir[2]);
                    delta = sdf->get_value(ray_pos[0], ray_pos[1], ray_pos[2]);
                }
                s_res[int(i + (h / 2))][int(l + (w / 2))] = dist;
            }
        }
    }

    inline unsigned char** thread_rendering_with_hardwriting(int n_threads){
        ofstream data;
        data.open("data.txt");
        data << w << " " << h << " " << n_threads;
        int num_threads = n_threads;
        auto* threads = new thread[num_threads];
        for(int i = -num_threads / 2; i < num_threads / 2; i++){
            threads[i + num_threads / 2] = thread(&scene::stripe_rendering_with_hardwriting, this, int(h / num_threads) * i, int(h / num_threads) * (i + 1), i + num_threads / 2);
        }
        for(int i = 0; i < num_threads; i++) {
            threads[i].join();
        }
        return s_res;
    }

    inline vector<unsigned char> thread_rendering_image(int n_threads){
        s_res_image.resize(w * h * 4);
        int num_threads = n_threads;
        auto* threads = new thread[num_threads];
        for(int i = -num_threads / 2; i < num_threads / 2; i++){
            threads[i + num_threads / 2] = thread(&scene::stripe_rendering_image, this, int(h / num_threads) * i, int(h / num_threads) * (i + 1));
        }
        for(int i = 0; i < num_threads; i++) {
            threads[i].join();
        }
        return s_res_image;
    }

    inline unsigned char** thread_rendering(int n_threads){
        s_res = new unsigned char*[h];
        for (int i = 0; i < h; i++) {
            s_res[i] = new unsigned char[w];
        }
        int num_threads = n_threads;
        auto* threads = new thread[num_threads];
        for(int i = -num_threads / 2; i < num_threads / 2; i++){
            threads[i + num_threads / 2] = thread(&scene::stripe_rendering, this, int(h / num_threads) * i, int(h / num_threads) * (i + 1), i + num_threads / 2);
        }
        for(int i = 0; i < num_threads; i++) {
            threads[i].join();
        }
        return s_res;
    }
};
