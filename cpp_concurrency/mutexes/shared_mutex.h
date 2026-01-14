#include <mutex>
#include <condition_variable>
#include <stdexcept>

class ReadWriteLock {
public:
    ReadWriteLock() : active_readers(0), waiting_writers(0), writing(false) {}

    void lockRead() {
        std::unique_lock<std::mutex> lock(m_);
        cv.wait(lock, [&] { return !writing && waiting_writers == 0; });
        ++active_readers;
    }

    void unlockRead() {
        std::unique_lock<std::mutex> lock(m_);
        if (active_readers == 0) {
            throw std::logic_error("no active readers!");
        }
        --active_readers;
        if (active_readers == 0) {
            cv.notify_all();
        }
    }

    void lockWrite() {
        std::unique_lock<std::mutex> lock(m_);
        ++waiting_writers;
        cv.wait(lock, [&] { return !writing && active_readers == 0; });
        --waiting_writers;
        writing = true;
    }

    void unlockWrite() {
        std::unique_lock<std::mutex> lock(m_);
        if (!writing) {
            throw std::logic_error("unlocking when not writing active!");
        }
        writing = false;
        cv.notify_all();
    }

    ReadWriteLock(const ReadWriteLock&) = delete;
    ReadWriteLock& operator=(const ReadWriteLock&) = delete;

private:
    std::mutex m_;
    std::condition_variable cv;
    int active_readers;
    int waiting_writers;
    bool writing;
};
