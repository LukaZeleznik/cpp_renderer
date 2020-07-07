#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include <unordered_map>

#include "geometry.h"
#include "tgaimage.h"

class Model {
private:
	std::vector<Vec3f> verts_;
	std::vector<std::vector<int> > faces_;
    std::vector<Vec2f> texture_ids;
    std::unordered_map<int, int> vert2text_;
    TGAImage texture_;
public:
	int nverts();
	int nfaces();
	Vec3f vert(int i);
	std::vector<int> face(int idx);
    bool read(const char *filename);
    Vec2f get_texture_ids(int vert_idx);
    bool read_texture(const char *filename, int width, int height);
    TGAColor get_uv(int x, int y);
    int get_t_width();
    int get_t_height();
};

#endif //__MODEL_H__
