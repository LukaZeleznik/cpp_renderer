#include "geometry.h"
#include "model.h"
#include "tgaimage.h"


class Shader{
    private:
        Vec3f normals;
    public:
    //takes point from world coords to screen coords
    virtual Vec3f transform_point(Model &model, const Mat4f &mm, const int v_idx, const int order) = 0;
    //calculates the appropriate color, decides wether the point need to be drawn at all
    virtual void fragment(Vec3f bar, TGAColor &color) const  = 0;
};

Vec3f barricentric(Vec3f *pts, int x, int y){
    
    Vec3f u = Vec3f(pts[2].x-pts[0].x, pts[1].x-pts[0].x,
                    pts[0].x-x)^Vec3f(pts[2].y-pts[0].y,
                    pts[1].y-pts[0].y, pts[0].y-y);

    if (std::abs(u.z)<1) return Vec3f(-1,1,1);
        return Vec3f(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z); 
}

const Mat4f viewport(int x, int y, int width, int height, int d=255){
    Mat4f m = Mat4f().identity();
    m(0,3) = x+width/2.f;
    m(1,3) = y+height/2.f;
    m(0,0) = width/2.f;
    m(1,1) = height/2.f;
    m(2,2) = d/2.f;
    m(2,3) = d/2.f;
    return m;
} 

const Mat4f InvProjection(Radian rx, Radian ry, Radian rz, float tx, float ty){
    Mat4f rot_x = Mat4f().identity();
    Mat4f rot_y = Mat4f().identity();
    Mat4f rot_z = Mat4f().identity(); 
    Mat4f translation{}; 

    translation(0,3) = -tx;
    translation(1,3) = -ty;

    rot_x(1,1) = std::cos(rx);
    rot_x(2,2) = std::cos(rx);
    rot_x(2,1) = -std::sin(rx);
    rot_x(1,2) = std::sin(rx);

    rot_z(0,0) = std::cos(rz);
    rot_z(1,1) = std::cos(rz);
    rot_z(1,0) = -std::sin(rz);
    rot_z(0,1) = std::sin(rz);
    
    rot_y(0,0) = std::cos(ry);
    rot_y(2,0) = std::sin(ry);
    rot_y(0,2) = -std::sin(ry);
    rot_y(2,2) = std::cos(ry);
    return (rot_z * rot_y * rot_x + translation).transpose();
}

const Mat4f Projection(Radian rx, Radian ry, Radian rz, float tx, float ty){
    //succeptible to gimbal lock
    Mat4f rot_x = Mat4f().identity();
    Mat4f rot_y = Mat4f().identity();
    Mat4f rot_z = Mat4f().identity(); 
    Mat4f translation{}; 

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
    rot_y(2,0) = -std::sin(ry);
    rot_y(2,2) = std::cos(ry);
    return rot_x * rot_y * rot_z + translation;
}

void rasterize_triangle(Vec3f *screen_coords, Shader &shader, TGAImage &image, std::array<std::array<float, 800>, 800>  &z_buffer){
    int minx = image.get_width()-1;
    int miny = image.get_height()-1;
    int maxx = 0;
    int maxy = 0;

    for (int i=0; i < 3; i++){
        minx = std::min(minx, int(screen_coords[i].x));
        maxx = std::max(maxx, int(screen_coords[i].x));
        miny = std::min(miny, int(screen_coords[i].y));
        maxy = std::max(maxy, int(screen_coords[i].y));
    }
    minx = std::max(0, minx);
    miny = std::max(0, miny);
    maxx = std::min(800, maxx);
    maxy = std::min(800, maxy);

    TGAColor color;
    for (int x=minx; x<=maxx; x++){
        for (int y=miny; y<=maxy; y++){
            Vec3f b_coords = barricentric(screen_coords, x, y);
            float z = b_coords.x * screen_coords[1].z
                    + b_coords.y * screen_coords[0].z
                    + b_coords.z * screen_coords[2].z;
            if (z < z_buffer[x][y]) continue;
            if (b_coords.x >= 0 && b_coords.y >= 0 && b_coords.z >= 0){
                z_buffer[x][y] = z;
                shader.fragment(b_coords, color);
                image.set(x,y,color);
            }
        }
    }
}
