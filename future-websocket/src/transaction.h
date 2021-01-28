#ifndef __TRANSACTION_H_0c029c3f253a4cff837024b78b1e4fed_
#define __TRANSACTION_H_0c029c3f253a4cff837024b78b1e4fed_

#include "future.h"
namespace ws_side {
typedef std::map<std::string, CFuture*>                                                              FutureMap;
typedef std::function<void(websocketpp::connection_hdl hdl, const std::string&, const std::string&)> MsgCallBack;
typedef std::function<void(websocketpp::connection_hdl hdl)>                                         CloseCallBack;
class CTranscation {
  protected:
    CFuture* newFuture(std::string key);
    void     releaseFuture(std::string key);
    CFuture* getFuture(std::string key);
    void     setFuture(std::string key, const std::string& value, bool success);

    void onStringMessage(websocketpp::connection_hdl hdl, const std::string& msg);
    void registerCallback(MsgCallBack callBack);

  private:
    FutureMap     m_syncFuture;  // kye:msgId value:hdl
    std::mutex    m_syncFutureL;
    MsgCallBack   m_messageCallback;
};

}  // namespace ws_side
#endif  //__TRANSACTION_H_0c029c3f253a4cff837024b78b1e4fed_