#pragma once
#include "../RouterContext.hpp"

namespace Router {
class RouterBase {
protected:
  RouterContext &context;
  const GlobalGrid *globalGrid;

public:
  RouterBase(RouterContext &C) : context(C), globalGrid(C.getGlobalGrid()) {}
  virtual void route() = 0;
};
}; // namespace Router