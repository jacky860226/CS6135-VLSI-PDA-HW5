#pragma once
#include "RouterBase.hpp"
#include <algorithm>
#include <queue>
#include <random>
#include <vector>

namespace Router {
class LShape : public RouterBase {
  int distanceToBorder(const Point &p) const {
    int left = p.h;
    int down = p.v;
    int right = globalGrid->GridH - 1 - p.h;
    int up = globalGrid->GridV - 1 - p.v;
    return std::min({left + down, left + up, right + down, right + up});
  }
  std::vector<int> run(const Net *net) {
    Point a(net->pins[0].h, net->pins[1].v);
    Point b(net->pins[1].h, net->pins[0].v);
    if (distanceToBorder(a) > distanceToBorder(b))
      std::swap(a, b);
    std::vector<int> ans;
    int dh = 0, dv = 0;
    for (int i = 0; i < 2; ++i) {
      Point pre = net->pins[i];
      dh = a.h - pre.h;
      dv = a.v - pre.v;
      if (dh)
        dh /= std::abs(dh);
      if (dv)
        dv /= std::abs(dv);
      Point d(dh, dv);
      for (Point cur = pre + d; pre != a;) {
        ans.emplace_back(context.getEdgeManager().getEdgeId(Edge(pre, cur)));
        pre = cur;
        cur = pre + d;
      }
    }
    return ans;
  }

public:
  LShape(RouterContext &C) : RouterBase(C) {}
  void route() override {
    for (auto &netPtr : globalGrid->Nets) {
      auto net = netPtr.get();
      auto &netRoute = context.getNetRoute(net);
      netRoute = std::move(run(net));
      context.addRouteDemand(netRoute, 1);
      context.addRouteNet(net, netRoute);
    }
  }
  bool ripup_reroute() override { return true; }
};
}; // namespace Router