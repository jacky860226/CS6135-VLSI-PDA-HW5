#pragma once
#include "Input.hpp"
#include <map>
#include <vector>

class EdgeManager {
  const GlobalGrid *globalGrid;
  std::vector<std::vector<std::pair<int, int>>> G;
  std::vector<Edge> edges;
  std::map<Edge, int> edge2id;

public:
  EdgeManager(const GlobalGrid *globalGrid)
      : globalGrid(globalGrid), G(globalGrid->GridH * globalGrid->GridV) {
    for (int h = 0; h < globalGrid->GridH; ++h) {
      for (int v = 0; v < globalGrid->GridV; ++v) {
        Point pu(h, v);
        if (v + 1 < globalGrid->GridV) {
          Point pv(h, v + 1);
          edges.emplace_back(pu, pv);
          edge2id[edges.back()] = edges.size() - 1;
        }
        if (h + 1 < globalGrid->GridH) {
          Point pv(h + 1, v);
          edges.emplace_back(pu, pv);
          edge2id[edges.back()] = edges.size() - 1;
        }
      }
    }
    const Point dir[] = {Point(0, 1), Point(0, -1), Point(1, 0), Point(-1, 0)};
    for (int h = 0; h < globalGrid->GridH; ++h) {
      for (int v = 0; v < globalGrid->GridV; ++v) {
        for (const auto &dp : dir) {
          Point pu(h, v);
          Point pv = pu + dp;
          if (!globalGrid->inRange(pv))
            continue;
          G[globalGrid->toIndex(pu)].emplace_back(globalGrid->toIndex(pv),
                                                  edge2id[Edge(pu, pv)]);
        }
      }
    }
  }
  size_t getEdgeSize() const { return edges.size(); }
  int getEdgeId(const Edge &e) const { return edge2id.at(e); }
  Edge getIdEdge(int id) const { return edges.at(id); }
  const std::vector<std::pair<int, int>> &getAdjEdges(int pid) const {
    return G.at(pid);
  }
};
