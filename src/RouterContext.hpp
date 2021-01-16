#pragma once
#include "EdgeIndexer.hpp"
#include "Input.hpp"
#include <fstream>
#include <unordered_map>
#include <unordered_set>

class DemandManager : public EdgeIndexer<int> {
  const GlobalGrid *globalGrid;

public:
  DemandManager(const GlobalGrid *globalGrid)
      : EdgeIndexer<int>(globalGrid->GridH, globalGrid->GridV),
        globalGrid(globalGrid) {}
  int overflow() const {
    int ans = 0;
    for (int e : edge[0]) {
      ans += std::max(e - globalGrid->CapacityH, 0);
    }
    for (int e : edge[1]) {
      ans += std::max(e - globalGrid->CapacityV, 0);
    }
    return ans;
  }
};

class RouterContext {
  const GlobalGrid *globalGrid;
  DemandManager demand;
  EdgeIndexer<std::unordered_set<const Net *>> routeNets;
  std::unordered_map<const Net *, std::vector<Edge>> routes;

public:
  RouterContext(const GlobalGrid *globalGrid)
      : globalGrid(globalGrid), demand(globalGrid),
        routeNets(globalGrid->GridH, globalGrid->GridV) {
    for (const auto &ptr : globalGrid->Nets) {
      routes[ptr.get()].clear();
    }
  }
  const GlobalGrid *getGlobalGrid() const { return globalGrid; }
  int overflow(const Edge &e) const {
    char dir = e.getDirection();
    if (dir == 'H')
      return std::max(demand.at(e) - globalGrid->CapacityH, 0);
    if (dir == 'V')
      return std::max(demand.at(e) - globalGrid->CapacityV, 0);
  }
  int overflow() const { return demand.overflow(); }
  void addRouteDemand(const std::vector<Edge> &route, int d) {
    for (const auto &edge : route) {
      demand.at(edge) += d;
    }
  }
  void addRouteNet(const Net *net, const std::vector<Edge> &route) {
    for (const auto &edge : route) {
      routeNets.at(edge).emplace(net);
    }
  }
  void removeRouteNet(const Net *net, const std::vector<Edge> &route) {
    for (const auto &edge : route) {
      routeNets.at(edge).erase(net);
    }
  }
  std::vector<Edge> &getNetRoute(const Net *net) { return routes.at(net); }

  void output(std::string outputPath) {
    std::ofstream fout(outputPath);
    for (const auto &net : globalGrid->Nets) {
      fout << net->name << ' ' << net->id << '\n';
      for (const auto &e : routes.at(net.get())) {
        fout << "(" << e.first.h << ", " << e.first.v << ", 1)-(" << e.second.h
             << ", " << e.second.v << ", 1)\n";
      }
      fout << "!\n";
    }
  }
};