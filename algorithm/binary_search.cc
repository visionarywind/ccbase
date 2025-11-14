#include <vector>
using namespace std;

int BinarySearch(int nums[], int target) {
  int low = 0;
  int high = sizeof(nums) / sizeof(int) - 1;
  while (low <= high) {
    int mid = low + ((high - low) >> 1);
    if (nums[mid] < target) {
      low = mid + 1;
    } else if (nums[mid] > target) {
      high = mid - 1;
    } else {
      return mid;
    }
  }
  return -1;
}
/**
 *
LowerBound 查找逻辑
目的: 找到第一个等于 target 的元素位置
策略: 当 nums[mid] == target 时，继续向左搜索（设置 high = mid - 1）
结果: 循环结束时，low 指向第一个 target 元素的位置
 *
 */
int LowerBound(int nums[], int target) {
  int low = 0;
  int high = sizeof(nums) / sizeof(int) - 1;
  while (low <= high) {
    int mid = low + ((high - low) >> 1);
    if (nums[mid] < target) {
      low = mid + 1;
    } else if (nums[mid] > target) {
      high = mid - 1;
    } else {
      high = mid - 1;
    }
  }
  if (low >= sizeof(nums) / sizeof(int)) {
    return -1;
  }
  return nums[low] == target ? low : -1;
}

/**
 *
UpperBound 查找逻辑
目的: 找到最后一个等于 target 的元素位置
策略: 当 nums[mid] == target 时，继续向右搜索（设置 low = mid + 1）
结果: 循环结束时，low 指向最后一个 target 元素的下一个位置
 *
 */
int UpperBound(int nums[], int target) {
  int low = 0;
  int high = sizeof(nums) / sizeof(int) - 1;
  while (low <= high) {
    int mid = low + ((high - low) >> 1);
    if (nums[mid] < target) {
      low = mid + 1;
    } else if (nums[mid] > target) {
      high = mid - 1;
    } else {
      low = mid + 1;
    }
  }
  if (low - 1 < 0) {
    return -1;
  }
  return nums[low - 1] == target ? low - 1 : -1;
}