#ifndef __FUTURE_H_67e0e40b83654ddf8c3390b785ff6089__
#define __FUTURE_H_67e0e40b83654ddf8c3390b785ff6089__
#include "commuchannel.h"
#include "locker.h"

namespace ws_side {
class CFuture {
  public:
    bool                                m_isFished;
    std::string                         m_result;
    bool                                m_success;
    commuchannel::CChannel<std::string> m_resultChan;
    std::mutex                          m_l;

  public:
    CFuture() : m_isFished(false), m_result(""), m_success(false) {}
    bool GetResult(std::string& result) {
        CLocker<std::string> resultLocker(m_result, m_l);
        if (m_isFished) {
            result = this->m_result;
            return m_success;
        }
        bool b = m_resultChan.read(m_result, 3000);
        if (!b) {
            m_isFished = true;
            m_result   = "";
            m_success = false;
        }
        else {
            m_isFished = true;
            m_success = true;
        }
        result = m_result;
        return m_success;
    }
    void SetResult(const std::string& result, bool success) {
        if (m_isFished) {
            return;
        }
        m_success = success;
        m_result = result;
        m_resultChan.write(result);
    }
};

}  // namespace ws_side
#endif  //__FUTURE_H_67e0e40b83654ddf8c3390b785ff6089__