#pragma once
#include "RouterBase.hpp"
#include <algorithm>
#include <queue>
#include <random>
#include <vector>

namespace Router {
class SPFA : public RouterBase {
  std::vector<long long> dist;
  std::vector<int> pa;
  std::vector<bool> inQueue;

  static bool cmp(const Net *a, const Net *b) { return a->HPWL() < b->HPWL(); }
  int overflowCost(const Point &pu, const Point &pv) {
    int ans = context.remain(Edge(pu, pv)) - 1;
    return std::max(-ans, 0);
  }
  std::vector<Edge> run(const Net *net) {
    static Point dir[] = {Point(0, 1), Point(0, -1), Point(1, 0), Point(-1, 0)};
    std::fill(dist.begin(), dist.end(), 0x3f3f3f3f3f3f3f3f);
    std::fill(pa.begin(), pa.end(), -1);
    std::fill(inQueue.begin(), inQueue.end(), false);
    int source = globalGrid->toIndex(net->pins[0]);
    int target = globalGrid->toIndex(net->pins[1]);
    std::queue<int> q;
    dist.at(source) = 0;
    q.emplace(source);
    inQueue[source] = true;
    while (q.size()) {
      int u = q.front();
      q.pop();
      inQueue[u] = false;
      Point pu = globalGrid->toPoint(u);
      for (const auto &dp : dir) {
        Point pv = pu + dp;
        if (!globalGrid->inRange(pv))
          continue;
        int v = globalGrid->toIndex(pv);
        long long cost = dist[u] + 1;
        cost += overflowCost(pu, pv) * 7122LL;
        long long edgeDemand = context.getEdgeDemand(Edge(pu, pv));
        cost += edgeDemand * edgeDemand / 2;
        if (dist[v] > cost) {
          dist[v] = cost;
          pa[v] = u;
          if (!inQueue[v]) {
            q.emplace(v);
          }
        }
      }
    }
    std::vector<Edge> ans;
    assert(target != -1);
    for (; pa[target] != -1; target = pa[target]) {
      ans.emplace_back(globalGrid->toPoint(target),
                       globalGrid->toPoint(pa[target]));
    }
    return ans;
  }

public:
  SPFA(RouterContext &C)
      : RouterBase(C), dist(globalGrid->GridH * globalGrid->GridV),
        pa(globalGrid->GridH * globalGrid->GridV),
        inQueue(globalGrid->GridH * globalGrid->GridV) {}
  void route() override {
    std::vector<const Net *> nets;
    nets.reserve(globalGrid->Nets.size());
    for (auto &net : globalGrid->Nets) {
      nets.emplace_back(net.get());
    }
    std::mt19937 MT(7122);
    std::shuffle(nets.begin(), nets.end(), MT);
    std::sort(nets.begin(), nets.end(), cmp);
    for (auto net : nets) {
      auto &netRoute = context.getNetRoute(net);
      netRoute = std::move(run(net));
      context.addRouteDemand(netRoute, 1);
      context.addRouteNet(net, netRoute);
    }
  }
};
}; // namespace Router