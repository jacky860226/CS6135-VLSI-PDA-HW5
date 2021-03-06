#pragma once
#include "RouterBase.hpp"
#include <algorithm>
#include <queue>
#include <random>
#include <unordered_set>
#include <vector>

namespace Router {
class Dijkstra : public RouterBase {
  std::vector<long long> dist;
  std::vector<std::pair<int, int>> pa;
  std::vector<bool> visit;
  std::mt19937 MT;

  static bool cmp(const Net *a, const Net *b) { return a->HPWL() < b->HPWL(); }
  int overflowCost(int eid) {
    int ans = context.remain(eid) - 1;
    return std::max(-ans, 0);
  }
  std::vector<int> run(const Net *net) {
    std::fill(dist.begin(), dist.end(), 0x3f3f3f3f3f3f3f3f);
    std::fill(pa.begin(), pa.end(), std::make_pair(-1, -1));
    std::fill(visit.begin(), visit.end(), false);
    int source = globalGrid->toIndex(net->pins[0]);
    int target = globalGrid->toIndex(net->pins[1]);
    std::priority_queue<std::pair<long long, int>,
                        std::vector<std::pair<long long, int>>,
                        std::greater<std::pair<long long, int>>>
        pq;
    dist.at(source) = 0;
    pq.emplace(0, source);
    while (pq.size()) {
      auto top = pq.top();
      int u = top.second;
      pq.pop();
      if (visit[u])
        continue;
      visit[u] = true;
      if (u == target)
        break;
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
          pq.emplace(dist[v], v);
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
  Dijkstra(RouterContext &C, size_t seed = 7122)
      : RouterBase(C), dist(globalGrid->GridH * globalGrid->GridV),
        pa(globalGrid->GridH * globalGrid->GridV),
        visit(globalGrid->GridH * globalGrid->GridV), MT(seed) {}
  void route() override {
    std::vector<const Net *> nets;
    nets.reserve(globalGrid->Nets.size());
    for (auto &net : globalGrid->Nets) {
      nets.emplace_back(net.get());
    }
    std::shuffle(nets.begin(), nets.end(), MT);
    std::sort(nets.begin(), nets.end(), cmp);
    for (auto net : nets) {
      auto &netRoute = context.getNetRoute(net);
      netRoute = std::move(run(net));
      context.addRouteDemand(netRoute, 1);
      context.addRouteNet(net, netRoute);
    }
  }
  bool ripup_reroute() override {
    std::vector<int> overflowEdges;
    int edgeSize = context.getEdgeManager().getEdgeSize();
    for (int eid = 0; eid < edgeSize; ++eid) {
      if (context.overflow(eid)) {
        overflowEdges.emplace_back(eid);
      }
    }
    if (overflowEdges.empty())
      return true;
    std::shuffle(overflowEdges.begin(), overflowEdges.end(), MT);
    for (auto eid : overflowEdges) {
      auto &overflowNets = context.getRouteNet(eid);
      std::vector<const Net *> overflowNetsV(overflowNets.begin(),
                                             overflowNets.end());
      for (auto net : overflowNetsV) {
        auto &netRoute = context.getNetRoute(net);
        context.addRouteDemand(netRoute, -1);
        context.removeRouteNet(net, netRoute);
      }
      std::sort(overflowNetsV.begin(), overflowNetsV.end(), cmp);
      for (auto net : overflowNetsV) {
        auto &netRoute = context.getNetRoute(net);
        netRoute = std::move(run(net));
        context.addRouteDemand(netRoute, 1);
        context.addRouteNet(net, netRoute);
      }
    }
    return false;
  }
};
}; // namespace Router