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
const Vec3f light(0, 0, 1);


class GouraudShader : public Shader{

    private: 
        Vec3f normals{};
    public:
        Vec3f transform_point(Model &model, const Mat4f &mm, const int v_idx, const int order){
            //intensity is clipped to min 0
            normals.raw[order] = std::fmax(0, model.vertex_normal(v_idx) * light); 
            return to_screen_coords(mm, model.vert(v_idx));
        }

        void fragment(Vec3f bar, TGAColor &color) const { 
            float intensity = bar * normals;
            color = TGAColor(255 * intensity, 255 * intensity , 255 * intensity, 255);
        }
};


void render(){
	TGAImage image(width, height, TGAImage::RGB);
    Model model{};
    if (!model.read_texture("../african_head_diffuse.tga", width, height)) return;
    if (!model.read("../obj/african_head.obj")) return;

    const Mat4f mm = viewport(0, 0, width, height) * modelView(Degree(-40), Degree(40), Degree(0), 0, -.3);
    std::array<std::array<float, 800>, 800> z_buffer;

    
    for(int i=0; i < width; i++)
        z_buffer[i].fill(-std::numeric_limits<float>::max());

    
    GouraudShader shader;
    for (int i=0; i < model.nfaces(); i++){
        Vec3f screen_coords[3];
        auto ids = model.face(i);
       for (int j=0; j < 3; j++){
            screen_coords[j] = shader.transform_point(model, mm, ids[j], j);

       }    
       rasterize_triangle(screen_coords,  shader, image, z_buffer);
    }
    
	image.flip_vertically();
	image.write_tga_file("output.tga");
}

int main(int argc, char** argv) {
    
    render();
	return 0;

}
