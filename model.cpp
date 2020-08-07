#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <bits/stdc++.h> 

#include "model.h"
#include "tgaimage.h"

bool Model::read(const char *filename){
    std::ifstream in;
    in.open (filename, std::ifstream::in);
    if (in.fail()) {
        std::cout << "Reading .obj file failed" << std::endl;
        return false;
    }
    std::string line;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            Vec3f v;
            for (int i=0;i<3;i++) iss >> v.raw[i];
            verts_.push_back(v);
        }
        else if (!line.compare(0, 2, "f ")) {
            std::vector<int> f;
            int itrash, idx, text_idx;
            iss >> trash;
            while (iss >> idx >> trash >> text_idx >> trash >> itrash) {
                idx--; // in wavefront obj all indices start at 1, not zero
                text_idx--;
                f.push_back(idx);
                vert2text_[idx] = text_idx;
            }
            faces_.push_back(f);
        }
        else if (!line.compare(0, 2, "vt")){
            Vec2f v;
            iss >> trash >> trash >> v.u >> trash >> v.v;
            texture_ids.push_back(v);
        }

        else if (!line.compare(0, 2, "vn")){
            Vec3f v ;
            iss >> trash >> trash >> v.x >> v.y >> v.z;
            normals_.push_back(v);
        }
    }
    std::cout << "# v# " << verts_.size() 
              << " f# "  << faces_.size() 
              << " t# " << texture_ids.size()
              << " normals "  << normals_.size()
              << std::endl;
    return true;
}

bool Model::read_texture(const char *filename){
    texture_ = TGAImage();
    if (!texture_.read_tga_file(filename)) return false;
    texture_.flip_vertically();
    return true;
}

bool Model::read_point_nm(const char *filename){
    point_nm = TGAImage();
    if (!point_nm.read_tga_file(filename)) return false;
    point_nm.flip_vertically();
    return true;

}

Vec3f Model::vertex_normal(int idx){
    if (idx >= normals_.size())
        return Vec3f(-1., -1., -1.);
    return normals_[idx];
}

int Model::get_t_width(){
    return texture_.get_width();
}

int Model::get_t_height(){
    return texture_.get_height();
}

int Model::nverts() {
    return (int)verts_.size();
}

int Model::nfaces() {
    return (int)faces_.size();
}

std::vector<int> Model::face(int idx) {
    return faces_[idx];
}

Vec3f Model::vert(int i) {
    return verts_[i];
}

TGAColor Model::get_uv(int x, int y){
    return texture_.get(x ,y);
}

Vec3f Model::get_point_nm(int x, int y){
    TGAColor normals = point_nm.get(x, y);
    return Vec3f(normals.r, normals.g, normals.b).normalize();
}



Vec2f Model::get_texture_ids(int vert_idx){
    const auto iter = vert2text_.find(vert_idx);
    if  (iter != vert2text_.end())
        return texture_ids[vert2text_[vert_idx]];
    std::cerr << vert_idx << std::endl;
    std::cerr << "Vertex texture mapping missmatch" << std::endl;
    return Vec2f(-1., -1.);
}

