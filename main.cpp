#include <iostream>
#include <memory>
#include <vector>
#include <bits/stdc++.h> 
#include <eigen3/Eigen/Dense>
#include <math.h>
#include <limits>

#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const int width = 800;
const int height = 800;


Vec3f cross(Vec3f a, Vec3f b){
    return Vec3f(a.y*b.z - a.z*b.y,
                 a.z*b.x - a.x*b.z,
                 a.x*b.y - a.y*b.x);
}


Vec3f barricentric(float pts[3][2], int x, int y){
    
    Vec3f u = cross(Vec3f(pts[2][0]-pts[0][0], pts[1][0]-pts[0][0], pts[0][0]-x), Vec3f(pts[2][1]-pts[0][1], pts[1][1]-pts[0][1], pts[0][1]-y));

    if (std::abs(u.z)<1) return Vec3f(-1,1,1);
        return Vec3f(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z); 
}


void triangle(std::unique_ptr<Model> &model,int face_idx, TGAImage &image, std::array<std::array<float, height>, width> &z_buffer, const Eigen::Matrix4f &mat){
    std::vector<int> face = model->face(face_idx);
    Eigen::Matrix<float, 4, 3> homogenous;
    Vec3f wc[3];
    for (int i=0; i<3; i++){
        wc[i] = model->vert(face[i]);
        for (int j=0; j<3; j++)
            homogenous(j, i) = model->vert(face[i]).raw[j];
        homogenous(3, i) = 1;
    }
    Eigen::Matrix<float, 4, 3> projected = mat * homogenous;

    float pts[3][2];
    int minx = image.get_width()-1;
    int miny = image.get_height()-1;
    int maxx = 0;
    int maxy = 0;
    int a, b;
    Vec2f t_coords[3];
    Vec3f w_coords[3];
    for (int j=0; j<3; j++){
        Vec3f v0 = Eigen::Vector4f(projected.col(j));
        w_coords[j] = v0;
        t_coords[j] = model->get_texture_ids(face[j]);
        //transform into screen coordinates
        a = (v0.x+1.)*width/2.; 
        minx = std::min(minx, a);
        maxx = std::max(maxx, a);
        b = (v0.y+1.)*height/2.; 
        miny = std::min(miny, b);
        maxy = std::max(maxy, b);
        pts[j][0] = a;
        pts[j][1] = b;
    }
    minx = std::max(0, minx);
    miny = std::max(0, miny);
    maxx = std::min(width, maxx);
    maxy = std::min(height, maxy);
    Vec3f norm = (wc[2] - wc[0])^(wc[1]-wc[0]);
    norm.normalize();

    float dp = norm*Vec3f(0, 0, -1);
    if (dp < 0) return;
    for (int x=minx; x< maxx; x++){
        for (int y=miny; y<maxy; y++){
            Vec3f coords = barricentric(pts, x, y);
            float z = coords.x * w_coords[1].z + coords.y * w_coords[0].z + coords.z * w_coords[2].z;
            if (z < z_buffer[x][y]) continue;
            if (coords.x >= 0 && coords.y >= 0 && coords.z >= 0){
                z_buffer[x][y] = z;
                float v = 0.;
                float u = 0.;
                for (int i = 0; i<3; i++) 
                    u += coords.raw[i] * (t_coords[i].u * model->get_t_width());
                for (int i = 0; i<3; i++)
                    v += coords.raw[i] * (t_coords[i].v * model->get_t_height());

                TGAColor c  = model->get_uv(u, v);
                image.set(x,y, TGAColor(c.r * dp, c.g*dp, c.b*dp, 255) );
            }
        }
    }
}


void line(int x0, int y0, int x1, int y1, TGAImage &image, const TGAColor &color){
    bool swaped = false;
    if (std::abs(x1-x0) < std::abs(y1-y0)){
        swaped = true; 
        std::swap(x0, y0);
        std::swap(x1, y1);
    } 

    if (x0 > x1){
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    int dx = x1-x0;
    int dy = y1-y0;
    int error2 = 0;
    int derror2 = std::abs(dy)*2;
    int y = y0;
    for (int x = x0; x < x1; x++){
        if (swaped){
            image.set(y, x, color);
        }
        else{
        image.set(x,y, color);
        }
        error2 += derror2;
        if (error2 > dx) {
            y += (y1>y0?1:-1);
            error2 -= dx*2;
        }
    }
   
}

Eigen::Matrix4f modelView(Radian rx, Radian ry, Radian rz, float tx, float ty){
    Eigen::Matrix4f rot_x = Eigen::Matrix4f::Identity();
    Eigen::Matrix4f rot_y = Eigen::Matrix4f::Identity();
    Eigen::Matrix4f rot_z = Eigen::Matrix4f::Identity();
    Eigen::Matrix4f translation = Eigen::Matrix4f::Zero();

    translation(0,3) = tx;
    translation(1,3) = ty;

    rot_x(1,1) = std::cos(rx);
    rot_x(2,2) = std::cos(rx);
    rot_x(1,2) = -std::sin(rx);
    rot_x(2,1) = std::sin(rx);

    rot_z(0,0) = std::cos(rz);
    rot_z(1,1) = std::cos(rz);
    rot_z(0,1) = -std::sin(rz);
    rot_z(1,0) = std::sin(rz);
    
    rot_y(0,0) = std::cos(ry);
    rot_y(0,2) = std::sin(ry);
    rot_y(2,0) = std::sin(ry);
    rot_y(2,2) = std::cos(ry);

    return rot_x * rot_y * rot_z + translation;
}


void render(){
	TGAImage image(width, height, TGAImage::RGB);
    std::unique_ptr<Model> model(new Model());
    if (!model->read_texture("../african_head_diffuse.tga", width, height)) return;
    if (!model->read("../obj/african_head.obj")) return;

    auto mm = modelView(Degree(0), Degree(30), Degree(0), 0, 0);
    std::array<std::array<float, height>, width> z_buffer;

    for(int i=0; i < width; i++)
        for(int j=0; j < height; j++)
            z_buffer[i][j] = -std::numeric_limits<float>::max();

    for (int i=0; i < model->nfaces(); i++)
        triangle(model, i, image, z_buffer, mm);
    

	image.flip_vertically();
	image.write_tga_file("output.tga");
}

int main(int argc, char** argv) {

    render();

	return 0;}
