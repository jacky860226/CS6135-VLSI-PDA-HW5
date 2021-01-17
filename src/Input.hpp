#pragma once
#include <cassert>
#include <memory>
#include <string>
#include <vector>

struct Point {
  int h, v;
  Point() {}
  Point(int h, int v) : h(h), v(v) {}
  bool operator<(const Point &b) const {
    return h < b.h || (h == b.h && v < b.v);
  }
  bool operator!=(const Point &b) const { return h != b.h || v != b.v; }
  Point operator+(const Point &b) const { return Point(h + b.h, v + b.v); }
};

struct Edge : public std::pair<Point, Point> {
  Edge(const Point &a, const Point &b) : std::pair<Point, Point>(a, b) {
    if (second < first)
      std::swap(first, second);
  }
  char getDirection() const {
    if (first.v == second.v)
      return 'H';
    if (first.h == second.h)
      return 'V';
    assert(false && "dir error!!");
  }
};

struct Net {
  std::string name, id;
  std::vector<Point> pins;
  int HPWL() const {
    return std::abs(pins[0].h - pins[1].h) + std::abs(pins[0].v - pins[1].v);
  }
};

struct GlobalGrid {
  int GridH, GridV;
  int CapacityH, CapacityV;
  std::vector<std::unique_ptr<Net>> Nets;
  bool inRange(const Point &p) const {
    return 0 <= p.h && p.h < GridH && 0 <= p.v && p.v < GridV;
  }
  int toIndex(const Point &p) const { return GridH * p.v + p.h; }
  Point toPoint(int id) const { return Point(id % GridH, id / GridH); }
};