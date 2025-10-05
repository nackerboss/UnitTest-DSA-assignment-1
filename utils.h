#ifndef __UTILS_H__
#define __UTILS_H__

#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
/**
 * @class Point
 * @brief Represents a point in 3D coordinate system (Oxyz)
 */
class Point
{
 private:
   double x, y, z;

 public:
   Point()
       : x(0)
       , y(0)
       , z(0)
   {
   }

   Point(double x, double y)
       : x(x)
       , y(y)
       , z(0)
   {
   }

   Point(double x, double y, double z)
       : x(x)
       , y(y)
       , z(z)
   {
   }

   Point(const Point &other)
       : x(other.x)
       , y(other.y)
       , z(other.z)
   {
   }

   double getX() const { return x; }

   double getY() const { return y; }

   double getZ() const { return z; }

   void setX(double newX) { x = newX; }

   void setY(double newY) { y = newY; }

   void setZ(double newZ) { z = newZ; }

   double distanceTo(const Point &other) const
   {
      double dx = x - other.x;
      double dy = y - other.y;
      double dz = z - other.z;
      return sqrt(dx * dx + dy * dy + dz * dz);
   }

   void translate(double dx, double dy, double dz)
   {
      x += dx;
      y += dy;
      z += dz;
   }

   Point operator+(const Point &other) const { return Point(x + other.x, y + other.y, z + other.z); }

   Point operator-(const Point &other) const { return Point(x - other.x, y - other.y, z - other.z); }

   Point operator*(double scalar) const { return Point(x * scalar, y * scalar, z * scalar); }

   bool operator==(const Point &other) const
   {
      const double EPSILON = 1e-9;
      return (fabs(x - other.x) < EPSILON) && (fabs(y - other.y) < EPSILON) && (fabs(z - other.z) < EPSILON);
   }

   friend std::ostream &operator<<(std::ostream &os, const Point &point)
   {
      os << "(" << point.x << "," << point.y << "," << point.z << ")";
      return os;
   }
};

namespace format
{
   // horrible performance , but do the tricks
   inline void format_impl(std::ostringstream &oss, const std::string &fmt, size_t pos)
   {
      oss << fmt.substr(pos);
   }

   template <typename T, typename... Args>
   void format_impl(std::ostringstream &oss, const std::string &fmt, size_t pos, T &&value, Args &&...args)
   {
      size_t brace { fmt.find("{}", pos) };
      if (brace == std::string::npos)
      {
         oss << fmt.substr(pos);
         return;
      }

      oss << fmt.substr(pos, brace - pos) << std::forward<T>(value);
      format_impl(oss, fmt, brace + 2, std::forward<Args>(args)...);
   }

   template <typename... Args> std::string format(const std::string &fmt, Args &&...args)
   {
      std::ostringstream oss;
      format_impl(oss, fmt, 0, std::forward<Args>(args)...);
      return oss.str();
   }

}; // namespace format
//

template <typename... Args> void print(const std::string &fmt, Args &&...args)
{
   std::cout << format::format(fmt, args...);
}

template <typename T> void print(const T &value)
{
   std::cout << value;
}

inline void println()
{
   std::cout << "\n";
}
template <typename T> void println(const T &value)
{
   std::cout << value << "\n";
}

template <typename... Args> void println(const std::string &fmt, Args &&...args)
{
   std::cout << format::format(fmt, args...) << "\n";
}
#endif // __UTILS_H__
