#include <thread>

class interrupt_flag {
 public:
  void set();
  bool is_set() const;
};

thread_local interrupt_flag this_thread_interrupt_flag;

class interruptible_thread {
 public:
  template<typename FunctionType>
  interruptible_thread(FunctionType f) {
    std::promise<interrupt_flag*> p;  // 2
    internal_thread=std::thread([f,&p]{  // 3
      p.set_value(&this_thread_interrupt_flag);
      f();  // 4
    });
    flag=p.get_future().get();  // 5
  }

  void join();
  void detach();
  bool joinable() const;
  void interrupt() {
    if(flag)
    {
      flag->set();  // 6
    }
  }

  void interruption_point() {
    if(this_thread_interrupt_flag.is_set())
    {
      throw std::runtime_error("");
    }
  }

  void interruptible_wait(std::condition_variable& cv, std::unique_lock<std::mutex>& lk){
    interruption_point();
    this_thread_interrupt_flag.set_condition_variable(cv);  // 1
    cv.wait(lk);  // 2
    this_thread_interrupt_flag.clear_condition_variable();  // 3
    interruption_point();
  }

 private:
  std::thread internal_thread;
  interrupt_flag* flag;
};