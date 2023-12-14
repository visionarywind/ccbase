#include "actor_hash.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "robin_hood.h"

#include "tools/timer.h"

using namespace std;

vector<string> ReadFile(const char* filename) {
  vector<string> lines;
  ifstream file(filename);
  string str;
  while (getline(file, str)) {
    lines.emplace_back(str);
  }
  return std::move(lines);
}

std::pair<vector<string>, robin_hood::unordered_map<std::string, std::string>> actor_map() {
  robin_hood::unordered_map<std::string, std::string> map;
  vector<string> actor_names = ReadFile("/Users/shanfeng/Workspace/ccbase/application/src/actor_name.dat");
  for (auto actor_name : actor_names) {
    map.emplace(actor_name, actor_name);
  }
  return std::make_pair(actor_names, map);
}

void ActorHashTest() {
  auto [actor_names, map] = actor_map();
  {
    TimerClock clock("hash query");
    int hits = 0;
    for (int i = 0, j = 0; i != 180000; i++, j++) {
      if (j >= actor_names.size()) {
        j = 0;
      }
      auto v = map.at(actor_names[j]);
      hits++;
    }
  }
  vector<string> vec(actor_names.begin(), actor_names.end());
  {
    TimerClock clock("index query");
    int hits = 0;
    for (int i = 0, j = 0; i != 180000; i++, j++) {
      if (j >= vec.size()) {
        j = 0;
      }
      auto v = vec[j];
      hits++;
    }
  }
}