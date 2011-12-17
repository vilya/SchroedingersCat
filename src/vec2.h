#ifndef cat_vec2_h
#define cat_vec2_h

namespace cat {

  //
  // Types
  //

  struct Vec2 {
    double x, y;

    Vec2();
    Vec2(double ix, double iy);
    Vec2(const Vec2& v);
  };


  //
  // Operators
  //

  Vec2 operator + (const Vec2& a, const Vec2& b);
  Vec2 operator - (const Vec2& a, const Vec2& b);
  Vec2 operator * (const Vec2& a, const Vec2& b);
  Vec2 operator / (const Vec2& a, const Vec2& b);

  Vec2 operator * (const Vec2& v, double k);
  Vec2 operator * (double k, const Vec2& v);
  Vec2 operator / (const Vec2& v, double k);
  Vec2 operator / (double k, const Vec2& v);


  //
  // Functions
  //

  double dot(const Vec2& a, const Vec2& b);
  Vec2 reflect(const Vec2& in, const Vec2& normal);


} // namespace cat

#endif // cat_vec2_h

