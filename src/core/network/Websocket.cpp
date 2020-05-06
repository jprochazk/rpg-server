
#include "pch.h"
#include "Websocket.h"
#include "SocketManager.h"
#include "PacketHandler.h"

Websocket::Websocket(
    tcp::socket&& socket,
    SocketManager* socketManager)
    : wsBuffer_()
    , ws_(std::move(socket))
    , isAsyncWriting_(false)
    , writeBuffer_()
    , socketManager_(socketManager)
    , packetHandler_(nullptr)
{
}

Websocket::~Websocket()
{
    // Remove this session from the list of active sessions
    socketManager_->Remove(this);
}

void Websocket::SetPacketHandler(PacketHandler* handler) {
    packetHandler_ = handler;
}

void Websocket::Run()
{
    // Set suggested timeout settings for the websocket
    ws_.set_option(
        beast::websocket::stream_base::timeout::suggested(
            beast::role_type::server));

    // Set a decorator to change the Server of the handshake
    ws_.set_option(beast::websocket::stream_base::decorator(
        [](beast::websocket::response_type& res)
    {
        res.set(http::field::server,
            std::string(BOOST_BEAST_VERSION_STRING) +
            " websocket-server");
    }));
    
    // Accept all data as binary (bytes aren't checked against valid utf-8)
    ws_.binary(true);

    // Accept the websocket handshake
    ws_.async_accept(
        beast::bind_front_handler(
            &Websocket::OnAccept,
            shared_from_this()));
}

void Websocket::Close()
{
    net::post(
        ws_.get_executor(),
        beast::bind_front_handler(
            &Websocket::OnClose,
            shared_from_this()));
}

void Websocket::OnClose()
{
    net::post(
        ws_.get_executor(),
        beast::bind_front_handler(
            &Websocket::AsyncClose,
            shared_from_this()));
}

void Websocket::Fail(beast::error_code ec, char const* what)
{
    // Don't report these
    if (ec == net::error::operation_aborted || ec == beast::websocket::error::closed)
        return;

    LOG_WARN("{}: {}", what, ec.message());
}

void Websocket::OnAccept(beast::error_code ec)
{
    // Handle the error, if any
    if (ec) return Fail(ec, "accept");

    // Add this session to the list of active sessions
    socketManager_->Add(this);

    // Read a message
    ws_.async_read(
        wsBuffer_,
        beast::bind_front_handler(
            &Websocket::OnRead,
            shared_from_this()));
}

void Websocket::OnRead(beast::error_code ec, std::size_t read_bytes)
{
    // Handle the error, if any
    if (ec) return Fail(ec, "read");

    if(read_bytes < 2 || read_bytes > MAX_PACKET_SIZE) {
        // don't bother reading messages smaller than 2 bytes (uint16_t opcode) 
        // or larger than max size
        wsBuffer_.consume(wsBuffer_.size());
    } else {
        ByteBuffer buf(wsBuffer_.size());
        boost::asio::buffer_copy(
            boost::asio::buffer(buf.Data(), wsBuffer_.size()),
            wsBuffer_.data()
        );
        wsBuffer_.consume(wsBuffer_.size());

        if (packetHandler_) {
            packetHandler_->Handle(std::move(buf));
        }
    }

    // Read another message
    ws_.async_read(
        wsBuffer_,
        beast::bind_front_handler(
            &Websocket::OnRead,
            shared_from_this()));
}

void Websocket::Send(std::vector<uint8_t> ss)
{
    // post it to websocket's strand and immediately return
    net::post(
        ws_.get_executor(),
        beast::bind_front_handler(
            &Websocket::OnSend,
            shared_from_this(),
            std::move(ss)));
}

void Websocket::OnSend(std::vector<uint8_t>&& ss)
{
    // Always add to queue
    writeBuffer_.push_back(std::move(ss));

    // Are we already writing?
    if (isAsyncWriting_)
        return;

    isAsyncWriting_ = true;

    // We are not currently writing, so send this immediately
    ws_.async_write(
        net::buffer(writeBuffer_.front()),
        beast::bind_front_handler(
            &Websocket::OnWrite,
            shared_from_this()));
}

void Websocket::OnWrite(beast::error_code ec, std::size_t)
{
    // Handle the error, if any
    if (ec)
        return Fail(ec, "write");

    // Remove packet from the queue
    writeBuffer_.erase(writeBuffer_.begin());

    // Stop writing if the queue is empty
    if (writeBuffer_.empty()) {
        isAsyncWriting_ = false;
        return;
    }

    // Write another
    ws_.async_write(
        net::buffer(writeBuffer_.front()),
        beast::bind_front_handler(
            &Websocket::OnWrite,
            shared_from_this()));
}

void Websocket::AsyncClose() {
    ws_.async_close(
        beast::websocket::close_code::normal,
        beast::bind_front_handler(
            &Websocket::OnAsyncClose,
            shared_from_this()
        )
    );
}

void Websocket::OnAsyncClose(beast::error_code ec) {
    if(ec) return Fail(ec, "Close");
}
