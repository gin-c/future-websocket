#ifndef __WS_CLIENT_H_486cedde20164726aa33018bbe8e2fb8__
#define __WS_CLIENT_H_486cedde20164726aa33018bbe8e2fb8__

#include "websocketpp/client.hpp"
#include <iostream>
#include "locker.h"
#include "transaction.h"

namespace ws_side {

typedef websocketpp::client<websocketpp::config::asio_client> ws_client;
typedef websocketpp::config::asio_client::message_type::ptr   message_ptr;
class CWSClient : public CTranscation {
  public:
    CWSClient();
    ~CWSClient();
    void init(MsgCallBack messageCallback);
    bool connect(std::string uri);
    bool reconnect();
    void close();
    bool sendSync(const std::string& playload, std::string& resp);
    bool send(const std::string& playload, const std::string& msgId);

  protected:
    void onOpen(websocketpp::connection_hdl hdl);
    void onClose(websocketpp::connection_hdl hdl);
    void onWsMessage(websocketpp::connection_hdl hdl, message_ptr msg);
    void terminate();

  private:
    std::string                                            m_uri;
    ws_client                                              m_c;
    websocketpp::connection_hdl                            m_hdl;
    websocketpp::lib::shared_ptr<websocketpp::lib::thread> m_wsThread;
    bool                                                   m_done;
    bool                                                   m_connected;
};
}  // namespace ws_side
#endif  //__WS_CLIENT_H_486cedde20164726aa33018bbe8e2fb8__