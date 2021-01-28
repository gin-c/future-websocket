#include "transaction.h"
#include "spdlog/spdlog.h"

namespace ws_side {

CFuture* CTranscation::newFuture(std::string key) {
    CFuture*           future = new (CFuture);
    CLocker<FutureMap> locker(m_syncFuture, m_syncFutureL);
    locker()[key] = future;
    return future;
}

void CTranscation::releaseFuture(std::string key) {
    CLocker<FutureMap> locker(m_syncFuture, m_syncFutureL);
    auto               iter = locker().find(key);
    if (iter == locker().end()) {
        return;
    }
    delete iter->second;
    locker().erase(iter);
}

CFuture* CTranscation::getFuture(std::string key) {
    CLocker<FutureMap> locker(m_syncFuture, m_syncFutureL);
    auto               iter = locker().find(key);
    if (iter == locker().end()) {
        return nullptr;
    }
    return iter->second;
}

void CTranscation::setFuture(std::string key, const std::string& value, bool success) {
    CLocker<FutureMap> locker(m_syncFuture, m_syncFutureL);
    locker()[key]->SetResult(value, success);
    return;
}

void CTranscation::onStringMessage(websocketpp::connection_hdl hdl, const std::string& msg) {
    int pos = msg.find("#");
    if (pos == std::string::npos) {
        spdlog::error("ws recv not proto,{}", msg);
        return;
    }
    std::string uuid = msg.substr(0, pos);
    // ´æÔÚÊÂÎñ
    CFuture* future = getFuture(uuid);
    if (future != nullptr) {
        future->SetResult(msg.substr(pos + 1), true);
        return;
    }

    if (m_messageCallback != nullptr) {
        m_messageCallback(hdl, msg.substr(pos + 1), uuid);
    }
}
void CTranscation::registerCallback(MsgCallBack callBack) {
    m_messageCallback = callBack;
}

}