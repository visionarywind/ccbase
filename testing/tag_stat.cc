#include <vector>
#include <set>
#include <map>
#include <cstdint>

struct Interval {
    uintptr_t start;
    uintptr_t end;

    bool operator<(const Interval& other) const {
        return start < other.start;
    }
};

struct TagStat {
    std::map<int, std::set<Interval>> tagIntervals;

    std::vector<int> Query(uintptr_t start, uintptr_t end) {
        std::vector<int> result;
        // 遍历所有tag的区间，检查是否覆盖查询区间
        for (const auto& [tag, intervals] : tagIntervals) {
            // 此处需要实现区间查询逻辑，返回包含查询区间的tag
            // 实际实现需要遍历或二分查找，这里简化处理
        }
        return result;
    }

    bool UseTag(uintptr_t start, uintptr_t end, int tag) {
        auto& intervals = tagIntervals[tag];
        Interval newInterval{start, end};

        // 查找可能合并的左边区间
        auto it = intervals.lower_bound(Interval{start, 0});
        if (it != intervals.begin()) {
            auto prev = std::prev(it);
            if (prev->end >= start - 1) {
                newInterval.start = std::min(newInterval.start, prev->start);
                newInterval.end = std::max(newInterval.end, prev->end);
                intervals.erase(prev);
            }
        }

        // 查找可能合并的右边区间
        it = intervals.lower_bound(Interval{newInterval.start, 0});
        while (it != intervals.end() && it->start <= newInterval.end + 1) {
            if (it->start > newInterval.end + 1) break;
            if (it->end >= newInterval.start - 1) {
                newInterval.end = std::max(newInterval.end, it->end);
                intervals.erase(it++);
            } else {
                ++it;
            }
        }

        // 插入新区间并返回是否原区间存在
        return intervals.insert(newInterval).second;
    }
};