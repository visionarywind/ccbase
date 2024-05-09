#include <iostream>
#include <sstream>
using namespace std;

int generate() {
  cout << "generate is called" << endl;
  return 1;
}

class LogAdapter {
 public:
  template <typename T>
  LogAdapter &operator<<(const T &val) noexcept {
    sstream_ << val;
    return *this;
  }

  string str() const { return sstream_.str(); }

  stringstream sstream_;
};

class LogWriter {
 public:
  ~LogWriter() = default;

  /// \param[in] stream The input log stream.
  void operator<(const LogAdapter &stream) const noexcept { std::cout << stream.str() << std::endl; }
};

#define LOG true ? void(0) : LogWriter() < LogAdapter()

// below is test
/*
int main() {
  stringstream ss;
  cout << "start" << endl;
  LOG << generate();
  return 0;
}
*/