#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <exception>
#include <cmath>
#include <iostream>
#include <array>


template <class T> class vec2
{
private:
    struct coords
    {
        T x, y;
    };
    
    union 
    {
        coords c_xy;
        T raw[2];
    };
    
public:
    vec2() : c_xy{0, 0} {}
    vec2(T _x, T _y) : c_xy{_x, _y} {}
    T operator[] (int) const;
    inline T& x()  { return c_xy.x; }
    inline T& y()  { return c_xy.y; }
    inline T& u()  { return c_xy.x; }
    inline T& t()  { return c_xy.y; } 
    T& at(int);

    inline vec2<T> operator+ (vec2<T> v) { return vec2<T>(x() + v.x(), y() + v.y()); }
    inline vec2<T> operator- (vec2<T> v) { return vec2<T>(x() - v.x(), y() - v.y()); }
    inline vec2<T> operator* (T l) { x() *= l; y() *= l; return *this; }
    vec2<T>& operator= (vec2<T> v) {c_xy.x = v.x(); c_xy.y = v.y(); return *this; }

    template <class > friend std::ostream& operator<<(std::ostream&, const vec2&);

};

template <class T> class vec3
{
private:
    struct coords
    {
        T x, y, z;
    };
    
    union 
    {
        coords c_xyz;
        coords c_ut;
        T raw[3];
    };

public:

    vec3() : c_xyz{0, 0, 0} {}
    vec3(T _x, T _y, T _z) : c_xyz{_x, _y, _z} {}
    //vec3(std::array<T, 3> arr) {c_xyz.x(arr[0]); c_xyz.y(arr[1]); c_xyz.z(arr[2]); return *this;}
    T operator[] (int) const;
    inline T& x()  { return c_xyz.x; }
    inline T& y()  { return c_xyz.y; }
    inline T& z()  { return c_xyz.z; }
    T& at(int);

    //inline T& u() {return c_ut.u}
    //inline T& t() {return c_ut.t}

    inline vec3<T> operator+ (vec3<T> v) { return vec3<T>(x() + v.x(), y() + v.y(), z() + v.z()); }
    inline vec3<T> operator- (vec3<T> v) { return vec3<T>(x() - v.x(), y() - v.y()); }
    inline vec3<T> operator* (T l) { x() *= l; y() *= l; z() *= l; return this; }
    inline vec3<T>& operator= (vec3<T> const& v) {x() = v[0]; y() = v[1]; z() = v[2]; return *this; }
    //inline vec3<T>& operator= (std::array<T, 3> arr) { c_xyz.x(arr[0]); c_xyz.y(arr[1]); c_xyz.z(arr[2]); return *this; }
    inline T norm() { return (sqrt(x() * x() + y() * y() + z() * z())); }
    inline void normalize() { T n = norm(); x() = x() / n; y() = y() / n; z() = z() / n; }

    template <class > friend std::ostream& operator<<(std::ostream&, const vec3&);
};

template<class T> std::ostream& operator<<(std::ostream &o, const vec2<T> &v)
{
    std::cout << "(" << v.x() << ", " << v.y() << ")";
    return o;
}

template<class T> std::ostream& operator<<(std::ostream &o, const vec3<T> &v) 
{
    std::cout << "(" << v.x() << ", " << v.y() << ", " << v.z() << ")";
    return o;
}

template<class T> T vec3<T>::operator[] (int i) const
{
    switch (i)
    {
        case 0:
            return c_xyz.x;
            break;
        case 1:
            return c_xyz.y;
            break;
        case 2:
            return c_xyz.z;
            break;
        default:
            throw (std::out_of_range("out of range"));
    }
}

template<class T> T& vec3<T>::at(int i) 
{
    switch (i)
    {
        case 0:
            return x(); 
            break;
        case 1:
            return y();
            break;
        case 2:
            return z();
            break;
        default:
            throw (std::out_of_range("out of range"));
    }
}

template<class T> T& vec2<T>::at(int i) 
{
    switch (i)
    {
        case 0:
            return x(); 
            break;
        case 1:
            return y();
            break;
        default:
            throw (std::out_of_range("out of range"));
    }
}

template<class T> T vec2<T>::operator[] (int i) const
{
    switch (i)
    {
        case 0:
            return c_xy.x;
            break;
        case 1:
            return c_xy.y;
            break;
        default:
            throw (std::out_of_range("out of range"));
    }
}

typedef vec2<int> Vec2i;
typedef vec3<int> Vec3i;
typedef vec2<float> Vec2f;
typedef vec3<float> Vec3f;

#endif