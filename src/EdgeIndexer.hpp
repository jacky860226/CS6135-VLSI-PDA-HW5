#pragma once
#include "Input.hpp"
#include <algorithm>
#include <cassert>
#include <vector>

template <class T> class EdgeIndexer {
protected:
  unsigned GridH, GridV;
  std::vector<T> edge[2];

public:
  EdgeIndexer(unsigned GridH, unsigned GridV) : GridH(GridH), GridV(GridV) {
    clear();
  }
  EdgeIndexer(unsigned GridH, unsigned GridV, const T &init)
      : GridH(GridH), GridV(GridV) {
    clear();
  }
  void clear() {
    edge[0].clear();
    edge[1].clear();
    edge[0].resize((GridH - 1) * GridV);
    edge[1].resize((GridV - 1) * GridH);
  }
  std::pair<unsigned, unsigned> getIndex(const Edge &e) const {
    unsigned base = 0, rem = 0;
    char dir = e.getDirection();
    if (dir == 'H') {
      rem = std::min(e.first.h, e.second.h);
      base = e.first.v;
      return std::make_pair(0, base * (GridH - 1) + rem);
    } else if (dir == 'V') {
      rem = std::min(e.first.v, e.second.v);
      base = e.first.h;
      return std::make_pair(1, base * (GridV - 1) + rem);
    }
    assert(false && "error edge!!");
  }
  T &at(const Edge &e) {
    auto res = getIndex(e);
    return edge[res.first].at(res.second);
  }
  const T &at(const Edge &e) const {
    auto res = getIndex(e);
    return edge[res.first].at(res.second);
  }
  T &at(unsigned dir, unsigned id) { return edge[dir].at(id); }
  const T &at(unsigned dir, unsigned id) const { return edge[dir].at(id); }
};