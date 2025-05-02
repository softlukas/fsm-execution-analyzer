#ifndef IFA_RUNTIME_UDP_H
#define IFA_RUNTIME_UDP_H

#include <asio.hpp>
#include <string>
#include <functional>
#include <memory>

namespace ifa_runtime {

// Callback pre príjem správ (Engine poskytne svoju metódu)
using UdpReceiveHandler = std::function<void(const std::string& /* type - napr. INPUT, CMD */, const std::string& /* name */, const std::string& /* value */ )>;
// Callback pre chyby (Engine poskytne svoju metódu)
using UdpErrorHandler = std::function<void(const std::string& /* error message */)>;

class UdpCommunicator {
public:
    // Konštruktor prijme io_context a callbacky
    UdpCommunicator(asio::io_context& io_ctx, UdpReceiveHandler receiver, UdpErrorHandler error_handler);
    ~UdpCommunicator();

    // Inicializácia socketov
    bool initialize(int listen_port, const std::string& dest_host, int dest_port);
    void shutdown();

    // Odoslanie správy (asynchrónne)
    void sendMessage(const std::string& message); // Interný formát správy si rieši Engine

    // Metóda na spustenie počúvania (volaná z Engine::initialize)
    void startReceive();

private:
    asio::io_context& io_context_;
    asio::ip::udp::socket socket_;
    asio::ip::udp::endpoint destinationEndpoint_;
    asio::ip::udp::endpoint senderEndpoint_; // Odkiaľ prišla posledná správa
    std::array<char, 1024> recvBuffer_;
    UdpReceiveHandler receiveHandler_;
    UdpErrorHandler errorHandler_;
    bool initialized_ = false;

    // Interný handler pre Asio async_receive_from
    void handleReceive(const asio::error_code& error, std::size_t bytes_transferred);
    // Interný handler pre Asio async_send_to
    void handleSend(const asio::error_code& error, std::size_t bytes_transferred);
    // Metóda na parsovanie (jednoduchý textový formát pre príklad)
    void parseAndDelegate(const char* data, std::size_t length);
};

} // namespace ifa_runtime
#endif // IFA_RUNTIME_UDP_H