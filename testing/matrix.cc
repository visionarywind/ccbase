#include <iostream>
#include <sstream>
#include <algorithm>
using namespace std;

template <typename T>
class LinearMatrix {
 private:
  std::unique_ptr<T[]> data_;
  const size_t rows_;
  const size_t cols_;

  class RowHelper {
   private:
    T *row_data_;
    const size_t cols_;

   public:
    RowHelper(T *row_data, size_t cols) : row_data_(row_data), cols_(cols) {}

    T &operator[](size_t col) {
      if (col >= cols_) {
        throw std::out_of_range("Column index out of range");
      }
      return row_data_[col];
    }

    const T &operator[](size_t col) const {
      if (col >= cols_) {
        throw std::out_of_range("Column index out of range");
      }
      return row_data_[col];
    }
  };

  class ConstRowHelper {
   private:
    const T *row_data_;
    const size_t cols_;

   public:
    ConstRowHelper(const T *row_data, size_t cols) : row_data_(row_data), cols_(cols) {}

    const T &operator[](size_t col) const {
      if (col >= cols_) {
        throw std::out_of_range("Column index out of range");
      }
      return row_data_[col];
    }
  };

 public:
  LinearMatrix(const size_t rows, const size_t cols, const T &init_value)
      : rows_(rows), cols_(cols), data_(std::make_unique<T[]>(rows * cols)) {
    std::fill_n(data_.get(), rows_ * cols_, init_value);
  }

  LinearMatrix(const LinearMatrix &) = delete;
  LinearMatrix &operator=(const LinearMatrix &) = delete;

  LinearMatrix(LinearMatrix &&) = default;
  LinearMatrix &operator=(LinearMatrix &&) = default;

  RowHelper operator[](size_t row) {
    if (row >= rows_) {
      throw std::out_of_range("Row index out of range");
    }
    return RowHelper(&data_[row * cols_], cols_);
  }

  ConstRowHelper operator[](size_t row) const {
    if (row >= rows_) {
      throw std::out_of_range("Row index out of range");
    }
    return ConstRowHelper(&data_[row * cols_], cols_);
  }

  void SetRow(size_t row, const T data) {
    if (row >= rows_) {
      throw std::out_of_range("Row index out of range");
    }
    std::fill_n(&data_[row * cols_], cols_, data);
  }

  size_t rows() const { return rows_; }
  size_t cols() const { return cols_; }

  std::string ToString() const {
    std::stringstream ss("\n");
    for (size_t i = 0; i < rows_; i++) {
      for (size_t j = 0; j < cols_; j++) {
        ss << std::to_string(data_[i * cols_ + j]) + "\t";
      }
      ss << "\n";
    }
    return ss.str();
  }
};

int main() {
  LinearMatrix<int> m(10, 10, 222);
  m[0][0] = 1;
  m.SetRow(1, 111);
  cout << m.ToString() << endl;
  return 0;
}