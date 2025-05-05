/**
 * @file ifa_runtime_udp.h
 * @brief Defines the UdpCommunicator class for handling UDP communication within the IFA runtime engine.
 * @authors Your Authors (xsiaket00, xsimonl00)
 * @date 2025-05-05 // Date of last modification
 */

#include "ifa_runtime_udp.h"
#include <iostream>
#include <utility>

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
        // Create a resolver to convert host/port strings to an endpoint
        asio::ip::udp::resolver resolver(io_context_);
        // Resolve the destination endpoint (take the first result)
        destinationEndpoint_ = *resolver.resolve(asio::ip::udp::v4(), dest_host, std::to_string(dest_port)).begin();
        // Open the socket using IPv4
        socket_.open(asio::ip::udp::v4());
        // Bind the socket to the specified local port and any IPv4 address
        socket_.bind(asio::ip::udp::endpoint(asio::ip::udp::v4(), listen_port));
        // Mark as initialized
        initialized_ = true;
        return true;
    } catch (const std::exception& ex) {
        errorHandler_("Initialization failed: " + std::string(ex.what()));
        return false;
    }
}

void UdpCommunicator::shutdown() {
    if (initialized_) {
        asio::error_code ec; // Ignored error code for close
        socket_.close(ec);
        initialized_ = false; // Mark as no longer initialized
    }
}

void UdpCommunicator::sendMessage(const std::string& message) {
    // Initiate an asynchronous send operation
    socket_.async_send_to(
        asio::buffer(message), destinationEndpoint_,
        // Lambda function as completion handler
        [this](const asio::error_code& error, std::size_t bytes_transferred) {
            handleSend(error, bytes_transferred);
        });
}

void UdpCommunicator::startReceive() {
    // Initiate an asynchronous receive operation
    socket_.async_receive_from(
        asio::buffer(recvBuffer_), senderEndpoint_,
        // Lambda function as completion handler
        [this](const asio::error_code& error, std::size_t bytes_transferred) {
            // Call handleReceive when the operation completes
            handleReceive(error, bytes_transferred);
        });
}

void UdpCommunicator::handleReceive(const asio::error_code& error, std::size_t bytes_transferred) {
    if (!error && bytes_transferred > 0) {
        // If no error and data was received, parse it
        parseAndDelegate(recvBuffer_.data(), bytes_transferred);
    } else if (error) {
        // If an error occurred (and it wasn't just cancellation), report it
        errorHandler_("Receive error: " + error.message());
    }
    startReceive();
}

void UdpCommunicator::handleSend(const asio::error_code& error, std::size_t /*bytes_transferred*/) {
    if (error) {
        // If an error occurred during sending, report it
        errorHandler_("Send error: " + error.message());
    }
}

void UdpCommunicator::parseAndDelegate(const char* data, std::size_t length) {
    // Convert raw data to a string
    std::string msg(data, length);

    // Find the positions of the delimiters
    auto first = msg.find('|');
    auto second = msg.find('|', first + 1);

    // Check if both delimiters were found
    if (first == std::string::npos || second == std::string::npos) {
        errorHandler_("Malformed message: " + msg);
        return;
    }

    // Extract the parts based on delimiter positions
    std::string type = msg.substr(0, first);
    std::string name = msg.substr(first + 1, second - first - 1);
    std::string value = msg.substr(second + 1);

    // Call the registered receive handler with the parsed parts
    receiveHandler_(type, name, value);
}

} // namespace ifa_runtime
