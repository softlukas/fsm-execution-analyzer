#include "ifa_runtime_udp.h"
#include <iostream>

namespace ifa_runtime {

UdpCommunicator::UdpCommunicator(asio::io_context& io_ctx, UdpReceiveHandler receiver, UdpErrorHandler error_handler)
    : io_context_(io_ctx),
      socket_(io_ctx),
      receiveHandler_(std::move(receiver)),
      errorHandler_(std::move(error_handler)) {}

UdpCommunicator::~UdpCommunicator() {
    shutdown();
}

bool UdpCommunicator::initialize(int listen_port, const std::string& dest_host, int dest_port) {
    try {
        asio::ip::udp::resolver resolver(io_context_);
        destinationEndpoint_ = *resolver.resolve(asio::ip::udp::v4(), dest_host, std::to_string(dest_port)).begin();
        socket_.open(asio::ip::udp::v4());
        socket_.bind(asio::ip::udp::endpoint(asio::ip::udp::v4(), listen_port));
        initialized_ = true;
        return true;
    } catch (const std::exception& ex) {
        errorHandler_("Initialization failed: " + std::string(ex.what()));
        return false;
    }
}

void UdpCommunicator::shutdown() {
    if (initialized_) {
        asio::error_code ec;
        socket_.close(ec);
        initialized_ = false;
    }
}

void UdpCommunicator::sendMessage(const std::string& message) {
    socket_.async_send_to(
        asio::buffer(message), destinationEndpoint_,
        [this](const asio::error_code& error, std::size_t bytes_transferred) {
            handleSend(error, bytes_transferred);
        });
}

void UdpCommunicator::startReceive() {
    socket_.async_receive_from(
        asio::buffer(recvBuffer_), senderEndpoint_,
        [this](const asio::error_code& error, std::size_t bytes_transferred) {
            handleReceive(error, bytes_transferred);
        });
}

void UdpCommunicator::handleReceive(const asio::error_code& error, std::size_t bytes_transferred) {
    if (!error && bytes_transferred > 0) {
        parseAndDelegate(recvBuffer_.data(), bytes_transferred);
    } else if (error) {
        errorHandler_("Receive error: " + error.message());
    }
    startReceive(); // Pokračuj v počúvaní
}

void UdpCommunicator::handleSend(const asio::error_code& error, std::size_t /*bytes_transferred*/) {
    if (error) {
        errorHandler_("Send error: " + error.message());
    }
}

// Jednoduchý textový formát: TYPE|NAME|VALUE\n
void UdpCommunicator::parseAndDelegate(const char* data, std::size_t length) {
    std::string msg(data, length);
    auto first = msg.find('|');
    auto second = msg.find('|', first + 1);

    if (first == std::string::npos || second == std::string::npos) {
        errorHandler_("Malformed message: " + msg);
        return;
    }

    std::string type = msg.substr(0, first);
    std::string name = msg.substr(first + 1, second - first - 1);
    std::string value = msg.substr(second + 1);
    receiveHandler_(type, name, value);
}

} // namespace ifa_runtime
