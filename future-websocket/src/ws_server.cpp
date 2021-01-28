#include "ws_server.h"
#include "spdlog/spdlog.h"
#include "logictool.h"

namespace ws_side {

CWSServer::CWSServer() : m_done(false) {}

CWSServer::~CWSServer() {
    if (m_done) {
        m_endpoint.stop();
        m_wsThread.join();
    }
}

bool CWSServer::start(std::string ip, std::string port, MsgCallBack messageCallback) {
    registerCallback(messageCallback);
    // 设置log
    m_endpoint.set_error_channels(websocketpp::log::elevel::all);
    m_endpoint.set_access_channels(websocketpp::log::alevel::all);

    m_endpoint.clear_access_channels(websocketpp::log::alevel::frame_payload);
    m_endpoint.clear_access_channels(websocketpp::log::alevel::frame_header);
    // 初始化Asio
    m_endpoint.init_asio();

    // 设置消息回调为echo_handler
    m_endpoint.set_message_handler(std::bind(&CWSServer::onWsMessage, this, std::placeholders::_1, std::placeholders::_2));
    m_endpoint.set_close_handler(std::bind(&CWSServer::onClose, this, std::placeholders::_1));
    // 监听ip,端口

    websocketpp::lib::error_code ec;
    m_endpoint.listen(ip, port, ec);
    if (ec) {
        spdlog::error("ws server listen failed, host[{}:{}]", ip, port);
        return false;
    }

    m_endpoint.start_accept();

    // 开始Asio事件循环
    m_wsThread = std::thread([=]() { m_endpoint.run(); });
    m_wsThread.detach();
    return true;
}

bool CWSServer::send(websocketpp::connection_hdl hdl, const std::string& playload, const std::string& msgId) {
    std::string body;
    body.append(msgId);
    body.append("#");
    body.append(playload);
    websocketpp::lib::error_code ec;
    m_endpoint.send(hdl, body, websocketpp::frame::opcode::text, ec);
    if (ec) {
        spdlog::error("ws sendSync failed,body{}", body);
        return false;
    }
    return true;
}

bool CWSServer::sendSync(websocketpp::connection_hdl hdl, const std::string& playload, std::string& resp) {
    std::string body, uuid;
    uuid = logictool::CLogicTool().generateUUID();
    body.append(uuid);
    body.append("#");
    body.append(playload);

    CFuture*                     future = newFuture(uuid);
    websocketpp::lib::error_code ec;
    m_endpoint.send(hdl, body, websocketpp::frame::opcode::text, ec);
    if (ec) {
        spdlog::error("ws sendSync failed,body{}", body);
        releaseFuture(uuid);
        return false;
    }
    bool b = future->GetResult(resp);
    if (!b) {
        spdlog::error("ws future get result failed");
        releaseFuture(uuid);
        return false;
    }
    releaseFuture(uuid);
    return true;
}

void CWSServer::onWsMessage(websocketpp::connection_hdl hdl, ws_server::message_ptr msg) {
    onStringMessage(hdl, msg->get_payload());
}
void CWSServer::onOpen(websocketpp::connection_hdl hdl) {}


void CWSServer::onClose(websocketpp::connection_hdl hdl) {

}

}  // namespace ws_side