#include "ws_client.h"
#include "logictool.h"
#include "spdlog/spdlog.h"

namespace ws_side {
CWSClient::CWSClient() : m_done(false), m_connected(false), m_uri("") {
    if (m_done) {
        terminate();
    }
}

CWSClient::~CWSClient() {}

void CWSClient::init(MsgCallBack messageCallback) {
    registerCallback(messageCallback);
    m_c.set_access_channels(websocketpp::log::alevel::all);
    m_c.clear_access_channels(websocketpp::log::alevel::frame_payload);
    m_c.clear_access_channels(websocketpp::log::alevel::frame_header);

    // 初始化 ASIO
    m_c.init_asio();

    // 注册消息回调

    m_c.set_message_handler(
        websocketpp::lib::bind(&CWSClient::onWsMessage, this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));

    m_c.set_open_handler(websocketpp::lib::bind(&CWSClient::onOpen, this, websocketpp::lib::placeholders::_1));
    m_c.set_close_handler(websocketpp::lib::bind(&CWSClient::onClose, this, websocketpp::lib::placeholders::_1));
    m_c.start_perpetual();

    m_wsThread = websocketpp::lib::make_shared<websocketpp::lib::thread>(&ws_client::run, &m_c);
    m_done     = true;
    return;
}

bool CWSClient::connect(std::string uri) {
    m_uri = uri;
    websocketpp::lib::error_code ec;
    ws_client::connection_ptr    con = m_c.get_connection(uri, ec);
    if (ec) {
        std::cout << "could not create connection because: " << ec.message() << std::endl;
        return false;
    }

    m_hdl = con->get_handle();
    m_c.connect(con);
    return true;
}

bool CWSClient::reconnect() {
    return connect(m_uri);
}

void CWSClient::onWsMessage(websocketpp::connection_hdl hdl, message_ptr msg) {
    onStringMessage(hdl, msg->get_payload());
}

void CWSClient::close() {
    if (m_connected) {
        m_c.close(m_hdl, websocketpp::close::status::normal, "");
    }
}

void CWSClient::terminate() {
    m_c.stop_perpetual();
    m_wsThread->join();
}

void CWSClient::onOpen(websocketpp::connection_hdl hdl) {
    m_connected = true;
}

void CWSClient::onClose(websocketpp::connection_hdl hdl) {
    m_connected = false;
}


bool CWSClient::sendSync(const std::string& playload, std::string& resp) {
    std::string body, uuid;
    uuid = logictool::CLogicTool().generateUUID();
    body.append(uuid);
    body.append("#");
    body.append(playload);

    CFuture*                     future = newFuture(uuid);
    websocketpp::lib::error_code ec;
    m_c.send(m_hdl, body, websocketpp::frame::opcode::text, ec);
    if (ec) {
        spdlog::error("ws sendSync failed,body{}", body);
        releaseFuture(uuid);
        return false;
    }
    bool success = future->GetResult(resp);
    if (!success) {
        spdlog::error("ws future get result failed");
        releaseFuture(uuid);
        return false;
    }
    releaseFuture(uuid);
    return true;
}

bool CWSClient::send(const std::string& playload, const std::string& msgId) {
    std::string body;
    body.append(msgId);
    body.append("#");
    body.append(playload);
    websocketpp::lib::error_code ec;
    m_c.send(m_hdl, body, websocketpp::frame::opcode::text, ec);
    if (ec) {
        spdlog::error("ws sendSync failed,body{}", body);
        return false;
    }
    return true;
}

}  // namespace ws_side