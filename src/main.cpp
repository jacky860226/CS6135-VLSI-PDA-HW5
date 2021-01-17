#include "GlobalTimer.hpp"
#include "Parser.hpp"
#include "Router/Dijkstra.hpp"
#include "Router/LShape.hpp"
#include "RouterContext.hpp"
#include <iostream>
#include <omp.h>

int main(int argc, char *argv[]) {
  // ./hw5 *.modified.txt *.result
  GlobalTimer::setTimeLimit(std::chrono::seconds(60 * 8));

  if (argc <= 2) {
    std::cerr << "Argument Error!\n";
    return -1;
  }

  Parser parser;
  auto globalGrid = parser.parse(argv[1]);
  RouterContext contextBase(globalGrid.get());

  Router::LShape LShapeRouter(contextBase);
  LShapeRouter.route();
  Router::Dijkstra dijkstraRouter(contextBase);
  dijkstraRouter.ripup_reroute();
  std::cout << contextBase.overflow() << " " << contextBase.wireLength()
            << '\n';

  std::vector<RouterContext> contexts(8, contextBase);
  std::vector<RouterContext> contextBest(8, contextBase);

#pragma omp parallel for
  for (size_t i = 0; i < contexts.size(); ++i) {
    Router::Dijkstra dijkstraRouter(contexts[i], i + 7122);
    for (int t = 0; t < 200; ++t) {
      if (dijkstraRouter.ripup_reroute())
        break;
      if (contexts[i] < contextBest[i])
        contextBest[i] = contexts[i];
      if (GlobalTimer::overTime())
        break;
    }
    std::cout << ".." << contextBest[i].overflow() << '\n';
  }

  const auto &context =
      *std::min_element(contextBest.begin(), contextBest.end());

  context.output(argv[2]);

  if (GlobalTimer::overTime())
    std::cout << "overTime!!!\n";
  std::cout << "\n";
  return 0;
}