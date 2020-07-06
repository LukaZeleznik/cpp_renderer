#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"
#include <bits/stdc++.h> 

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
            textures_.push_back(v);
        }
    }
    std::cout << "# v# " << verts_.size() 
              << " f# "  << faces_.size() 
              << " t# " << textures_.size()
              << std::endl;
    return true;
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

Vec2f Model::get_texture(int vert_idx){
    const auto iter = vert2text_.find(vert_idx);
    if  (iter != vert2text_.end())
        return textures_[vert2text_[vert_idx]];
    std::cerr << vert_idx << std::endl;
    std::cerr << "Vertex texture mapping missmatch" << std::endl;
    return Vec2f(-1., -1.);
}

