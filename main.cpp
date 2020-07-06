#include <iostream>
#include <memory>
#include <vector>
#include <bits/stdc++.h> 

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


void triangle(std::unique_ptr<Model> &model,std::vector<int> face, TGAImage &image, TGAImage &texture, std::array<std::array<float, height>, width> &z_buffer, TGAColor &color){
    float pts[3][2];
    int minx = image.get_width()-1;
    int miny = image.get_height()-1;
    int maxx = 0;
    int maxy = 0;
    int a, b;
    Vec2f t_coords[3];
    Vec3f w_coords[3];
    for (int j=0; j<3; j++){
        Vec3f v0 = model->vert(face[j]);
        t_coords[j] = model->get_texture(face[j]);
        w_coords[j] = v0;
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
    Vec3f norm = cross(w_coords[2] - w_coords[0], w_coords[1]-w_coords[0]);
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
                for (int i = 0; i<3; i++) u += coords.raw[i] * (t_coords[i].u * texture.get_width());
                for (int i = 0; i<3; i++) v += coords.raw[i] * (t_coords[i].v * texture.get_height());

                TGAColor c  = texture.get(u, v);
                image.set(x,y, TGAColor(c.r * dp, c.g*dp, c.b*dp, 255) );
            }
        }
    }
    //TODO: clamp v image size
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


void render(){
	TGAImage image(width, height, TGAImage::RGB);
    TGAImage texture = TGAImage();
    if (!texture.read_tga_file("../african_head_diffuse.tga")) return;
    texture.flip_vertically();
    //TODO: texture belongs to a model, load it there
    std::unique_ptr<Model> model(new Model());
    if (!model->read("../obj/african_head.obj")) return;

    std::array<std::array<float, height>, width> z_buffer;
    for(int i=0; i < width; i++)
        for(int j=0; j < height; j++)
            z_buffer[i][j] = -1000000.f;

    for (int i=0; i < model->nfaces(); i++){
        std::vector<int> face = model->face(i);
        if (face.size() > 3)
            std::cout << "ni trikotnik" << std::endl;
        /*
        for (int j=0; j<3; j++){
            Vec3f v0 = model->vert(face[j]);
            Vec3f v1 = model->vert(face[(j+1)%3]);
            //transform into screen coordinates
            int x0 = (v0.x+1.)*width/2.; 
            int y0 = (v0.y+1.)*height/2.; 
            int x1 = (v1.x+1.)*width/2.; 
            int y1 = (v1.y+1.)*height/2.; 
            //line(x0, y0, x1, y1, image, white);  
        }
        */
        TGAColor c = TGAColor(rand()%255, rand()%255, rand()%255, 255);
        triangle(model, face, image, texture, z_buffer, c);
    }

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
}

int main(int argc, char** argv) {

    render();

	return 0;
}
