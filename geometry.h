#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <exception>
#include <iostream>


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
        coords c_ut;
        T raw[2];
    };
    
public:
    vec2() : c_xy{0, 0} {}
    vec2(T _x, T _y) : c_xy{_x, _y} {}
    inline T operator[] (int i) {if (i < 0 || i >= 2) throw (std::out_of_range("out of range")); return raw[i];}
    inline T& x()  { return c_xy.x; }
    inline T& y()  { return c_xy.y; }
    inline T& u()  { return c_ut.u; }
    inline T& t()  { return c_ut.t; } 

    inline vec2<T> operator+ (vec2<T> v) { return vec2<T>(x() + v.x(), y() + v.y()); }
    inline vec2<T> operator- (vec2<T> v) { return vec2<T>(x() - v.x(), y() - v.y()); }
    inline vec2<T> operator* (T l) { x() *= l; y() *= l; return *this; }

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
    T operator[] (int) const;
    inline T& x()  { return c_xyz.x; }
    inline T& y()  { return c_xyz.y; }
    inline T& z()  { return c_xyz.z; }

    //inline T& u() {return c_ut.u}
    //inline T& t() {return c_ut.t}

    inline vec3<T> operator+ (vec3<T> v) { return vec3<T>(x() + v.x(), y() + v.y(), z() + v.z()); }
    inline vec3<T> operator- (vec3<T> v) { return vec3<T>(x() - v.x(), y() - v.y()); }
    inline vec3<T> operator* (T l) { x() *= l; y() *= l; z() *= l; return this; }
    inline vec3<T>& operator= (vec3<T> const& v) {x() = v[0]; y() = v[1]; z() = v[2]; return *this; }
    inline T norm() { return (x() * x() + y() * y() + z() * z() / sqrt(x() * x() + y() * y() + z() * z())); }
    inline void normalize() { T n = norm(); x() = x() / norm(); y() = y() / norm; z() = z() / norm(); }

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
    if (i < 0 || i >= 3) 
        throw (std::out_of_range("out of range"));
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
    }
}

typedef vec2<int> Vec2i;
typedef vec3<int> Vec3i;
typedef vec2<float> Vec2f;
typedef vec3<float> Vec3f;

#endif