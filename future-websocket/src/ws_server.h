#ifndef __WS_SERVER_H_486cedde20164726aa33018bbe8e2fb8__
#define __WS_SERVER_H_486cedde20164726aa33018bbe8e2fb8__

#include "websocketpp/config/asio_no_tls.hpp"
#include "websocketpp/server.hpp"
#include <iostream>
#include "locker.h"
#include "transaction.h"

namespace ws_side {

typedef websocketpp::server<websocketpp::config::asio>      ws_server;
typedef websocketpp::config::asio_client::message_type::ptr message_ptr;
class CWSServer : public CTranscation {
  public:
    CWSServer();
    ~CWSServer();
    bool start(std::string ip, std::string port, MsgCallBack messageCallback);
    bool sendSync(websocketpp::connection_hdl hdl, const std::string& playload, std::string& resp);
    bool send(websocketpp::connection_hdl hdl, const std::string& playload, const std::string& msgId);

  protected:
    void onOpen(websocketpp::connection_hdl hdl);
    void onClose(websocketpp::connection_hdl hdl);
    void onWsMessage(websocketpp::connection_hdl hdl, ws_server::message_ptr msg);

  private:
    ;
    ws_server   m_endpoint;
    std::thread m_wsThread;
    bool        m_done;
};
}  // namespace ws_side
#endif  //__WS_CLIENT_H_486cedde20164726aa33018bbe8e2fb8__