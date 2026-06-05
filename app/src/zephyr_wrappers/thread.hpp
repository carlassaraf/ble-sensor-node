#pragma once

#include <zephyr/kernel.h>

class Thread {
public:
  Thread(k_thread_stack_t *stack, size_t stack_size, int priority)
    : stack(stack), stack_size(stack_size), priority(priority) {}

  virtual ~Thread() = default;

  // Delete copy operators to avoid stack corruption
  Thread(const Thread &) = delete;
  Thread &operator=(const Thread &) = delete;

  void start(void *context, k_timeout_t timeout = K_NO_WAIT) {
    k_thread_create(&thread, stack, stack_size,
                    entry, this, context, nullptr,
                    priority, 0, timeout);
  }

  void suspend() {
    k_thread_suspend(&thread);
  }

  void resume() {
    k_thread_resume(&thread);
  }

protected:
  virtual void run(void *context) = 0;

  void sleep_ms(uint32_t ms) {
    k_msleep(ms);
  }

private:
  static void entry(void *arg, void *context, void *) {
    Thread *thread = static_cast<Thread *>(arg);
    thread->run(context);
  }

  k_thread_stack_t *stack;
  size_t stack_size;
  k_thread thread{};
  int priority;
};