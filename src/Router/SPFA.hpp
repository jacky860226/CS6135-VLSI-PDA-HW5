#pragma once
#include "RouterBase.hpp"
#include <algorithm>
#include <queue>
#include <random>
#include <vector>

namespace Router {
class SPFA : public RouterBase {
  std::vector<long long> dist;
  std::vector<std::pair<int, int>> pa;
  std::vector<bool> inQueue;

  static bool cmp(const Net *a, const Net *b) { return a->HPWL() < b->HPWL(); }
  int overflowCost(int eid) {
    int ans = context.remain(eid) - 1;
    return std::max(-ans, 0);
  }
  std::vector<int> run(const Net *net) {
    std::fill(dist.begin(), dist.end(), 0x3f3f3f3f3f3f3f3f);
    std::fill(pa.begin(), pa.end(), std::make_pair(-1, -1));
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
      const auto &adj = context.getEdgeManager().getAdjEdges(u);
      for (const auto &pii : adj) {
        int v = pii.first;
        int eid = pii.second;
        long long cost = dist[u] + 1;
        cost += overflowCost(eid) * 7122LL;
        long long edgeDemand = context.getEdgeDemand(eid);
        cost += edgeDemand * edgeDemand / 2;
        if (dist[v] > cost) {
          dist[v] = cost;
          pa[v] = std::make_pair(u, eid);
          if (!inQueue[v]) {
            q.emplace(v);
            inQueue[v] = true;
          }
        }
      }
    }
    std::vector<int> ans;
    assert(pa[target].first != -1);
    for (; pa[target].first != -1; target = pa[target].first) {
      ans.emplace_back(pa[target].second);
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
  bool ripup_reroute() override { return true; }
};
}; // namespace Router