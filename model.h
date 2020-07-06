#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include <map>

#include "geometry.h"

class Model {
private:
	std::vector<Vec3f> verts_;
	std::vector<std::vector<int> > faces_;
    std::vector<Vec2f> textures_;
    std::map<int, int> vert2text_;
public:
    Model()=default;
	int nverts();
	int nfaces();
	Vec3f vert(int i);
	std::vector<int> face(int idx);
    bool read(const char *filename);
    Vec2f get_texture(int vert_idx);
};

#endif //__MODEL_H__
