#pragma once
#include "EdgeManager.hpp"
#include "GlobalTimer.hpp"
#include "Input.hpp"
#include <fstream>
#include <unordered_map>
#include <unordered_set>

class RouterContext {
  const GlobalGrid *globalGrid;
  EdgeManager edgeManager;
  std::vector<int> demand;
  std::vector<std::unordered_set<const Net *>> routeNets;
  std::unordered_map<const Net *, std::vector<int>> routes;

public:
  RouterContext(const GlobalGrid *globalGrid)
      : globalGrid(globalGrid), edgeManager(globalGrid),
        demand(edgeManager.getEdgeSize()),
        routeNets(edgeManager.getEdgeSize()) {
    for (const auto &ptr : globalGrid->Nets) {
      routes[ptr.get()].clear();
    }
    std::cout << GlobalTimer::getDuration<>().count() / 1e9 << '\n';
  }
  bool operator<(const RouterContext &b) const {
    int overflowA = overflow();
    int overflowB = b.overflow();
    if (overflowA != overflowB)
      return overflowA < overflowB;
    return wireLength() < b.wireLength();
  }
  const GlobalGrid *getGlobalGrid() const { return globalGrid; }
  int overflow(int eid) const { return std::max(-remain(eid), 0); }
  int remain(int eid) const {
    char dir = edgeManager.getIdEdge(eid).getDirection();
    if (dir == 'H')
      return globalGrid->CapacityH - demand[eid];
    if (dir == 'V')
      return globalGrid->CapacityV - demand[eid];
    assert(false && "error edge!!");
  }
  int getEdgeDemand(int eid) const { return demand[eid]; }
  int overflow() const {
    int ans = 0;
    for (size_t i = 0; i < demand.size(); ++i)
      ans += overflow(i);
    return ans;
  }
  int wireLength() const {
    int ans = 0;
    for (const auto &r : routes) {
      ans += r.second.size();
    }
    return ans;
  }
  void addRouteDemand(const std::vector<int> &route, int d) {
    for (const auto &eid : route) {
      demand.at(eid) += d;
    }
  }
  void addEdgeDemand(int eid, int d) { demand.at(eid) += d; }
  void addRouteNet(const Net *net, const std::vector<int> &route) {
    for (const auto &eid : route) {
      routeNets.at(eid).emplace(net);
    }
  }
  void removeRouteNet(const Net *net, const std::vector<int> &route) {
    for (const auto &eid : route) {
      routeNets.at(eid).erase(net);
    }
  }
  const std::unordered_set<const Net *> &getRouteNet(int eid) const {
    return routeNets.at(eid);
  }
  std::vector<int> &getNetRoute(const Net *net) { return routes.at(net); }
  EdgeManager &getEdgeManager() { return edgeManager; }
  void output(std::string outputPath) const {
    std::ofstream fout(outputPath);
    for (const auto &net : globalGrid->Nets) {
      fout << net->name << ' ' << net->id << '\n';
      for (const auto &eid : routes.at(net.get())) {
        fout << "(" << edgeManager.getIdEdge(eid).first.h << ", "
             << edgeManager.getIdEdge(eid).first.v << ", 1)-("
             << edgeManager.getIdEdge(eid).second.h << ", "
             << edgeManager.getIdEdge(eid).second.v << ", 1)\n";
      }
      fout << "!\n";
    }
  }
};