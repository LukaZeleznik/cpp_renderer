#ifndef A_GL_H
#define A_GL_H

#include "geometry.h"
#include "model.h"
#include "tgaimage.h"




class Shader{
    private:
        Vec3f normals;
    public:
    virtual Vec3f transform_point(Model &model, const Mat4f &mm, const int v_idx, const int order) = 0;
    virtual void fragment(Vec3f bar, TGAColor &color) const = 0;
};

void move_camera();
void rasterize_triangle(Vec3f *screen_coords, 
                        Shader &shader, 
                        TGAImage &image,
                        std::array<std::array<float, 800>, 800>  &zbuffer);

const Mat4f viewport(int x, int y, int width, int height, int d=255);
const Mat4f modelView(Radian rx, Radian ry, Radian rz, float tx, float ty);

#endif
