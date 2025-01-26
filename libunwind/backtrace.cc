#include <stdio.h>
#include <libunwind.h>

#define MAX_STACK_FRAMES 10

void print_backtrace() {
  void *buffer[MAX_STACK_FRAMES];
  int frames = unw_backtrace(buffer, MAX_STACK_FRAMES);

  for (int i = 0; i < frames; i++) {
    // unw_cursor_t cursor;
    // unw_context_t context;
    // unw_word_t pc = (unw_word_t)buffer[i];
    // char fname[256];
    // unw_word_t offset;

    // // 初始化上下文和游标
    // unw_getcontext(&context);
    // unw_init_local(&cursor, &context);

    // // 设置游标到指定地址
    // unw_set_reg(&cursor, UNW_REG_IP, pc);

    // // 获取函数名
    // if (unw_get_proc_name(&cursor, fname, sizeof(fname), &offset) == 0) {
    //   printf("Frame %d: 0x%lx: (%s+0x%lx)\n", i, (long)pc, fname, (long)offset);
    // } else {
    //   printf("Frame %d: 0x%lx: (?+0x%lx)\n", i, (long)pc, (long)offset);
    // }
    // code up to here is not working
    printf("Frame %d: 0x%lx\n", i, (long)buffer[i]);
  }
}

// 测试函数 3
void func3() { print_backtrace(); }

// 测试函数 2
void func2() { func3(); }

// 测试函数 1
void func1() { func2(); }

// 主函数
int main() {
  func1();
  return 0;
}