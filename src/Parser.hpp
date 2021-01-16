#pragma once
#include "Input.hpp"
#include <cstdio>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>

struct Parser {
  std::unique_ptr<GlobalGrid> parse(const std::string &modifiedFile) {
    std::string buff;
    std::ifstream fin(modifiedFile);
    std::unique_ptr<GlobalGrid> globalGrid(new GlobalGrid());
    while (std::getline(fin, buff)) {
      if (buff.empty())
        continue;
      if (buff[0] == 'g')
        sscanf(buff.c_str(), "grid %d %d", &globalGrid->GridH,
               &globalGrid->GridV);
      else if (buff[0] == 'v')
        sscanf(buff.c_str(), "vertical capacity %d", &globalGrid->CapacityV);
      else if (buff[0] == 'h')
        sscanf(buff.c_str(), "horizontal capacity %d", &globalGrid->CapacityH);
      else if (buff[0] == 'n')
        break;
      else
        std::cerr << "read error!!\n";
    }
    while (std::getline(fin, buff)) {
      if (buff.empty())
        continue;
      std::unique_ptr<Net> net(new Net());
      std::stringstream ss(buff);
      ss >> net->name >> net->id;
      net->pins.resize(2);
      for (int i = 0; i < 2; ++i) {
        std::getline(fin, buff);
        sscanf(buff.c_str(), "%d %d", &net->pins[i].h, &net->pins[i].v);
      }
      globalGrid->Nets.emplace_back(std::move(net));
    }
    return globalGrid;
  }
};