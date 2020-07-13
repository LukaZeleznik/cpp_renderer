#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <cmath>
#include <ostream>
#include <iostream>
#include <vector>
#include <eigen3/Eigen/Dense>
#include <math.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T> struct Vec2 {
	union {
		struct {T u, v;};
		struct {T x, y;};
		T raw[2];
	};
	Vec2() : u(0), v(0) {}
	Vec2(T _u, T _v) : u(_u),v(_v) {}
	inline Vec2<T> operator +(const Vec2<T> &V) const { return Vec2<T>(u+V.u, v+V.v); }
	inline Vec2<T> operator -(const Vec2<T> &V) const { return Vec2<T>(u-V.u, v-V.v); }
	inline Vec2<T> operator *(float f)          const { return Vec2<T>(u*f, v*f); }
	template <class > friend std::ostream& operator<<(std::ostream& s, Vec2<T>& v);
};

template <class T> struct Vec3 {
	union {
		struct {T x, y, z;};
		struct { T ivert, iuv, inorm; };
		T raw[3];
	};
	Vec3() : x(0), y(0), z(0) {}
	Vec3(T _x, T _y, T _z) : x(_x),y(_y),z(_z) {}
    Vec3(const Eigen::Matrix<T,3,1> &mat) : x{mat[0]}, y{mat[1]}, z{mat[2]} {}
    Vec3(const Eigen::Matrix<T,4,1> &mat) : x{mat[0]/mat[3]}, y{mat[1]/mat[3]}, z{mat[2]/mat[3]} {} 
	inline Vec3<T> operator ^(const Vec3<T> &v) const { return Vec3<T>(y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x); }
	inline Vec3<T> operator +(const Vec3<T> &v) const { return Vec3<T>(x+v.x, y+v.y, z+v.z); }
	inline Vec3<T> operator +=(const Vec3<T> &v)  { x+=v.x; y+=v.y; z+=v.z; return *this; }
	inline Vec3<T> operator -(const Vec3<T> &v) const { return Vec3<T>(x-v.x, y-v.y, z-v.z); }
	inline Vec3<T> operator *(float f)          const { return Vec3<T>(x*f, y*f, z*f); }
	inline T       operator *(const Vec3<T> &v) const { return x*v.x + y*v.y + z*v.z; }
	float norm () const { return std::sqrt(x*x+y*y+z*z); }
	Vec3<T> & normalize(T l=1) { *this = (*this)*(l/norm()); return *this; }
	template <class > friend std::ostream& operator<<(std::ostream& s, Vec3<T>& v);
    operator Eigen::Matrix<T, 3, 1>(){return Eigen::Matrix<T,3,1>(x,y,z);}
};

using Vec2f = Vec2<float>;
using Vec2i = Vec2<int>;
using Vec3f = Vec3<float>;
using Vec3i = Vec3<int>;

template <class T> std::ostream& operator<<(std::ostream& s, Vec2<T>& v) {
	s << "(" << v.x << ", " << v.y << ")\n";
	return s;
}

template <class T> std::ostream& operator<<(std::ostream& s, Vec3<T>& v) {
	s << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
	return s;
}

template <class T, int M, int N> struct Mat{
    //for our use, matrix needs to be zero initalized
    std::array<std::array<T, N>, M> arr = {};


    T & operator()(int i, int j){
        return arr[i][j];
    }

    const T & operator()(int i, int j) const {
        return arr[i][j];
    }

    template <int O>
    Mat<T, M, O> operator *(const Mat<T, N, O> &other) const{
        Mat<T, M, O> ret{};
        for (int i=0; i < M; i++){
            for (int j=0; j < O; j++){
                for (int k=0; k<N; k++){
                ret(i, j) += arr[i][k] * other(k, j);
                }
            }
        }    
        return ret;
    } 

    Mat<T, M, N> & operator +=(const Mat<T, M, N> &other){
        for (int i=0; i<M; i++)
            for (int j=0; j<N; j++)
                arr[i][j] += other(i, j);
        return *this;
    }

    Mat<T, M, N> operator +(const Mat<T, M, N> &other) const{
        Mat<T, M, N> ret(*this);
        ret += other;
        return ret;
    }

    Mat<T, M, N>  & identity(){
        arr = {};
        for (int i=0; i<M; i++)
            arr[i][i] = 1;
        return *this;
    }

    Vec3f col(int j){
        return Vec3f(arr[0][j], arr[1][j], arr[2][j]);
    }

friend std::ostream & operator << (std::ostream & os, const Mat<T, M, N> & matrix) {
for(int i=0; i<M;++i){
   for(int j = 0; j< N;++j)
      os << matrix(i, j) << " ";
   os << std::endl;
}
};


    

};
using Mat4f = Mat<float, 4, 4>;
using Mat3f = Mat<float, 3, 3>;
using Mat2f = Mat<float, 2, 2>;

template<class T, int N>
Mat<T, 3, N> from_homog(const Mat<T, 4, N> &m){
    Mat<T, 3, N> ret{};
    for (int i=0; i<N; i++)
        for (int j=0; j < 3; j++){
            ret(j, i) = m(j,i) / m(3, i);
        }
    return ret;
}




class Degree{
    float deg;
    public:
    explicit Degree(float d){deg=d;}
    float degree() {return deg;}
    void set_degree(float d) {deg=d;};

};


class Radian{
    float rad;
    public:
    explicit Radian(float r) {rad = r;}
    Radian(Degree d) {rad = d.degree() * M_PI/180;}
    //conversion operator
    operator float() {return rad;}
    float radian() {return rad;}
};

#endif //__GEOMETRY_H__
