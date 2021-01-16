#include "GlobalTimer.hpp"
#include "Parser.hpp"
#include "Router/SPFA.hpp"
#include "RouterContext.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  // ./hw5 *.modified.txt *.result
  GlobalTimer::setTimeLimit(std::chrono::seconds(60 * 9));

  if (argc <= 2) {
    std::cerr << "Argument Error!\n";
    return -1;
  }

  Parser parser;
  auto globalGrid = parser.parse(argv[1]);
  RouterContext context(globalGrid.get());
  Router::SPFA spfaRouter(context);
  spfaRouter.route();
  std::cout << context.overflow() << '\n';

  context.output(argv[2]);

  if (GlobalTimer::overTime())
    std::cout << "overTime!!!\n";
  std::cout << "\n";
  return 0;
}