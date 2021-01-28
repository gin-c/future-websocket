#ifndef _LOCKER_H_D888FD563C9547759AC98BD72670467E__
#define _LOCKER_H_D888FD563C9547759AC98BD72670467E__

#include <string>
#include <list>
#include <mutex>

template <typename T> class CLocker {
  public:
    CLocker(T& t, std::mutex& mutex): m_mutex(&mutex) {
        m_mutex->lock();
        m_data = &t;
    }
    ~CLocker() {
        m_mutex->unlock();
    }
    T& operator()() {
        return *m_data;
    }

  private:
    T*          m_data;
    std::mutex* m_mutex;
};
#endif  //_LOCKER_H_D888FD563C9547759AC98BD72670467E__
