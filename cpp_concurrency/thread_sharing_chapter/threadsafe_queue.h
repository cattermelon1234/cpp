#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T>
class threadsafe_queue {
private:
  std::condition_variable data_cond;
  std::queue<T> data_queue;
  mutable std::mutex m_;

public:
  threadsafe_queue() {};
  threadsafe_queue(threadsafe_queue& other) {
    std::lock_guard lock(other.m_);
    data_queue = other.data_queue;
  }
  void push(T value) {
    {
      std::lock_guard lock(m_);
      data_queue.push(std::move(value));
    } 
    // notify after lock is released for efficiency
    data_cond.notify_one();
  }
  std::shared_ptr<T> wait_and_pop() {
    std::unique_lock lock(m_);
    data_cond.wait(lock, [this]{return !data_queue.empty();}); 
    auto ptr = std::make_shared<T>(std::move(data_queue.front()));
    data_queue.pop();
    return ptr;
  }
  void wait_and_pop(T& val) {
    std::unique_lock lock(m_);
    data_cond.wait(lock, [this]{return !data_queue.empty();}); 
    val = data_queue.front();
    data_queue.pop();
  }
  bool try_pop(T& value) {
    std::lock_guard lock(m_);
    if (data_queue.empty()) {
      return false;
    } 
    value = data_queue.front();
    data_queue.pop();
    return true;
  }
  std::shared_ptr<T> try_pop() {
    std::lock_guard lock(m_);
    if (data_queue.empty()) {
      return std::shared_ptr<T>();
    }
    auto ptr = std::make_shared<T>(std::move(data_queue.front()));
    data_queue.pop();
    return ptr;
  }
  bool empty () const {
    std::lock_guard lock(m_);
    return data_queue.empty();
  }
};
