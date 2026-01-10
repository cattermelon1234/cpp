#include <mutex>
#include <stack>
#include <exception>
#include <memory>

struct empty_stack: std::exception
{
  const char* what() const throw();
};

template <typename T>
class ThreadSafeStack {
public:
  ThreadSafeStack() {};
  ThreadSafeStack(const ThreadSafeStack& other) {
    std::lock_guard lock(other.m_);
    // copy data
    data = other.data;
  }
  ThreadSafeStack& operator=(const ThreadSafeStack&) = delete;
  void push(T value) {
    std::lock_guard lock(m_);
    data.push(std::move(value));
  }

  void pop(T& out) {
    std::lock_guard lock(m_);
    if (data.empty()) throw 
    out = data.top();
    data.pop();
  }

  std::shared_ptr<T> pop() {
    std::lock_guard lock(m_);
    if (empty()) throw empty_stack();  
    std::shared_ptr<T> const ptr(std::make_shared<T>(data.top()));
    data.pop();
    return ptr;
  }

  bool empty() const {
    std::lock_guard lock(m_);
    return data.empty();
  }
private:
  std::stack<T> data;
  mutable std::mutex m_;
};
