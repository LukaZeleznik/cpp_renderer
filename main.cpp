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
#include "a_gl.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const int width = 800;
const int height = 800;
const Vec3f light(0, 1, 1);

class GouraudShader : public Shader{

    private: 
        Vec3f u{};
        Vec3f v{};
        Model *m;
        Vec3f normals{};
        Mat4f *proj_normals;
    public:
        GouraudShader(Mat4f &inv_mat){
            proj_normals = &inv_mat;
        }
        Vec3f transform_point(Model &model, const Mat4f &mm, const int v_idx, const int order){
            normals.raw[order] = std::fmax(0, rotate_normals(*proj_normals, model.vertex_normal(v_idx)) * light); 
            u.raw[order] = model.get_texture_ids(v_idx).u;
            v.raw[order] = model.get_texture_ids(v_idx).v;
            m = &model;
            //intensity is clipped to min 0
            return to_screen_coords(mm, model.vert(v_idx));
        }

        void fragment(Vec3f bar, TGAColor &color) const { 
            float intensity = bar * normals;
            float avg_u = bar * u * m->get_t_width();
            float avg_v = bar * v * m->get_t_height();
            TGAColor c = m->get_uv(avg_u, avg_v);
            color = c * intensity;
        }
};

class RGBPNShader : public Shader{

    private: 
        Vec3f u{};
        Vec3f v{};
        Model *m;
        Mat4f *proj_normals;
    public:
        RGBPNShader(Mat4f &inv_mat){
            proj_normals = &inv_mat;
        }
        Vec3f transform_point(Model &model, const Mat4f &mm, const int v_idx, const int order){
            u.raw[order] = model.get_texture_ids(v_idx).u;
            v.raw[order] = model.get_texture_ids(v_idx).v;
            m = &model;
            //intensity is clipped to min 0
            return to_screen_coords(mm, model.vert(v_idx));
        }

        void fragment(Vec3f bar, TGAColor &color) const { 
            float avg_u = bar * u * m->get_t_width();
            float avg_v = bar * v * m->get_t_height();
            Vec3f normal = to_screen_coords(*proj_normals, (m->get_point_nm(avg_u, avg_v)));
            float intensity = std::fmax(0.f, light * normal);
            TGAColor c = m->get_uv(avg_u, avg_v);
            color = c * intensity;
        }
};

class CelShader : public Shader{
    private: 
        Vec3f normals{};
        float level_p;
        unsigned int levels;
    public:
        CelShader(unsigned int no_levels){
            level_p = 1.f / no_levels;
            levels = no_levels;
        }
        Vec3f transform_point(Model &model, const Mat4f &mm, const int v_idx, const int order){
            normals.raw[order] = std::fmax(0, model.vertex_normal(v_idx) * light); 
            return to_screen_coords(mm, model.vert(v_idx));
        }
        void fragment(Vec3f bar, TGAColor &color) const { 
            float intensity = floor((bar * normals)/ level_p) * level_p;
            color = TGAColor(255 * intensity, 255 * intensity , 255 * intensity, 255);
        }
};



void render(){
	TGAImage image(width, height, TGAImage::RGB);
    Model model{};
    if (!model.read_texture("../african_head_diffuse.tga")) return;
    if (!model.read_point_nm("../african_head_nm.tga")) return;
    if (!model.read("../obj/african_head.obj")) return;
    
    //TODO: extract movements
    Mat4f proj = Projection(Degree(-40), Degree(40), Degree(0), 0, 0);
    Mat4f mm = viewport(0, 0, width, height) * proj;
    std::array<std::array<float, 800>, 800> z_buffer;

    
    for(int i=0; i < width; i++)
        z_buffer[i].fill(-std::numeric_limits<float>::max());

    
    //CelShader shader(3);
    RGBPNShader shader(proj);
    //GouraudShader shader(proj);
    for (int i=0; i < model.nfaces(); i++){
        Vec3f screen_coords[3];
        auto ids = model.face(i);
       for (int j=0; j < 3; j++){
            screen_coords[j] = shader.transform_point(model, mm, ids[j], j);
       }    
       rasterize_triangle(screen_coords,  shader, image, z_buffer);
    }
    
    //todo: zbuffer to tga image
	image.flip_vertically();
	image.write_tga_file("output.tga");
}

int main(int argc, char** argv) {
    
    render();
	return 0;

}
