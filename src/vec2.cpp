#include "vec2.h"

namespace cat {

  //
  // Vec2 public methods
  //

  Vec2::Vec2() :
    x(0),
    y(0)
  {
  }


  Vec2::Vec2(double ix, double iy) :
    x(ix),
    y(iy)
  {
  }


  Vec2::Vec2(const Vec2& v) :
    x(v.x),
    y(v.y)
  {
  }


  //
  // Operators
  //

  Vec2 operator + (const Vec2& a, const Vec2& b)
  {
    return Vec2(a.x + b.x, a.y + b.y);
  }


  Vec2 operator - (const Vec2& a, const Vec2& b)
  {
    return Vec2(a.x - b.x, a.y - b.y);
  }


  Vec2 operator * (const Vec2& a, const Vec2& b)
  {
    return Vec2(a.x * b.x, a.y * b.y);
  }


  Vec2 operator / (const Vec2& a, const Vec2& b)
  {
    return Vec2(a.x / b.x, a.y / b.y);
  }


  Vec2 operator * (const Vec2& v, double k)
  {
    return Vec2(v.x * k, v.y * k);
  }


  Vec2 operator * (double k, const Vec2& v)
  {
    return Vec2(k * v.x, k * v.y);
  }


  Vec2 operator / (const Vec2& v, double k)
  {
    return Vec2(v.x / k, v.y / k);
  }


  Vec2 operator / (double k, const Vec2& v)
  {
    return Vec2(k / v.x, k / v.y);
  }


  //
  // Functions
  //

  double Dot(const Vec2& a, const Vec2& b)
  {
    return a.x * b.x + a.y * b.y;
  }


  Vec2 Reflect(const Vec2& in, const Vec2& normal)
  {
    return in - 2.0 * Dot(in, normal) * normal;
  }

} // namespace cat

