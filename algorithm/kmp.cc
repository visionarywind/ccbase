 #include <cctype>
#include <string_view>
#include <vector>
using namespace std;

std::vector<size_t> BuildPatternTable(const std::string_view &pattern) {
  std::vector<size_t> pattern_table(pattern.size(), 0);
  for (size_t i = 1, j = 0; i < pattern.size();) {
    if (pattern[i] == pattern[j]) {
      j++;
      pattern_table[i] = j;
      i++;
    } else {
      if (j > 0) {
        j = pattern_table[j - 1];
      } else {
        i++;
      }
    }
  }
  return pattern_table;
}

bool kmp(const std::string_view &src, const std::string_view &pattern) {
  if (pattern.empty()) {
    return true;
  }
  if (src.empty()) {
    return false;
  }
  const auto &pattern_table = BuildPatternTable(pattern);
  for (size_t i = 0, j = 0; i < src.size();) {
    if (src[i] == pattern[j]) {
      j++;
      if (j == pattern.size()) {
        return true;
      }
      i++;
    } else {
      if (j > 0) {
        j = pattern_table[j - 1];
      } else {
        i++;
      }
    }
  }

  return false;
}

struct ST {
  vector<long long> sum;
  vector<int> mn;

  void update(int o, int l, int r, int idx, int val) {
    if (l == r) {
      sum[o] += val;
      mn[o] += val;
      return;
    }
    int m = (l + r) / 2;
    if (idx <= m) {
      update(o * 2, l, m, idx, val);
    } else {
      update(o * 2 + 1, m + 1, r, idx, val);
    }
    mn[o] = min(mn[o * 2], mn[o * 2 + 1]);
    sum[o] = sum[o * 2] + sum[o * 2 + 1];
  }

  long long query(int o, int l, int r, int L, int R) {
    if (L <= l and r <= R) {
      return sum[o];
    }
    long long ans = 0;
    int m = (l + r) / 2;
    if (L <= m) {
      ans += query(o * 2, l, m, L, R);
    }
    if (R > m) {
      ans += query(o * 2 + 1, m + 1, r, L, R);
    }
    return ans;
  }

  int locate(int o, int l, int r, int R, int val) {
    if (mn[o] > val) {
      return -1;
    }
    if (l == r) {
      return l;
    }
    int m = (l + r) / 2;
    if (mn[o * 2] <= val) {
      return locate(o * 2, l, m, R, val);
    }
    if (R > m) {
      return locate(o * 2 + 1, m + 1, r, R, val);
    }
    return -1;
  }
};

class BookMyShow {
 private:
  int m_;
  int n_;
  ST st_;
 public:
  BookMyShow(int n, int m) : n_(n), m_(m) {
    st_.sum.resize(4 * n, 0);
    st_.mn.resize(4 * n, 0);
  }

  vector<int> gather(int k, int maxRow) {
    int r = st_.locate(1, 0, n_ - 1, maxRow, m_ - k);
    if (r < 0) {
      return {};
    }
    int s = (int)st_.query(1, 0, n_ - 1, r , r);
    st_.update(1, 0, n_ - 1, r, k);
    return {r, s};
  }

  bool scatter(int k, int maxRow) {
    long long s = st_.query(1, 0, n_ - 1, 0, maxRow);
    if (s > (long long)m_ * (maxRow + 1) - k) {
      return false;
    }
    int idx = st_.locate(1, 0, n_ - 1, maxRow, m_ - 1);
    while (k) {
      int left = min(m_ - (int)st_.query(1, 0, n_ - 1, idx, idx), k);
      st_.update(1, 0, n_ - 1, idx, left);
      k -= left;
      idx++;
    }
    return true;
  }
};