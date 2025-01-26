
#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <sstream>
using namespace std;

std::vector<std::string> split(std::string &s, std::string delimiter) {
  size_t pos_start = 0, pos_end, delim_len = delimiter.length();
  std::string token;
  std::vector<std::string> res;

  while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
    token = s.substr(pos_start, pos_end - pos_start);
    pos_start = pos_end + delim_len;
    res.push_back(token);
  }

  res.push_back(s.substr(pos_start));
  return res;
}

template <typename T>
T parse(std::string &s) {
  std::stringstream sstream(s);
  T ans;
  sstream >> ans;
  return ans;
}

int main() {
  ifstream in("run.log");
  std::map<std::string, size_t> allocated_map;
  size_t active_size = 0;
  size_t peak_size = 0;
  if (in) {
    string line;
    size_t index = 0;
    std::map<size_t, void *> to_free_map;

    // Traverse memory block csv.
    while (getline(in, line)) {
      if (line.find("TEST") == string::npos) {
        continue;
      }
      // process
      if (line.find("malloc") != string::npos) {
        // cout << "malloc" << endl;
        auto &&elements = split(line, " ");
        size_t size = parse<size_t>(elements[4]);
        std::string ptr = parse<std::string>(elements[7]);

        allocated_map[ptr] = size;
        active_size += size;
        peak_size = std::max(peak_size, active_size);
        cout << "active size : " << active_size << ", peak size : " << peak_size << endl;

        // cout << "malloc : " << size << ", " << ptr << endl;
      } else if (line.find("free") != string::npos) {
        // cout << "free" << endl;
        auto &&elements = split(line, " ");
        std::string ptr = parse<std::string>(elements[2]);
        // cout << "free : " << ptr << endl;
        if (allocated_map.find(ptr) != allocated_map.end()) {
          active_size -= allocated_map[ptr];
          allocated_map.erase(ptr);
        } else {
          cout << "free failed : " << ptr << endl;
        }
        peak_size = std::max(peak_size, active_size);
      }
    }
  } else {
    cout << "no such file" << endl;
  }
  in.close();
}