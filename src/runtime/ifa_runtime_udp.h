/**
 * @file ifa_runtime_udp.h
 * @brief Defines the UdpCommunicator class for handling UDP communication within the IFA runtime engine.
 * @authors Your Authors (xsiaket00, xsimonl00)
 * @date 2025-05-05 // Date of last modification
 */

#ifndef IFA_RUNTIME_UDP_H
#define IFA_RUNTIME_UDP_H

#include <asio.hpp>
#include <string>
#include <functional>
#include <memory>

namespace ifa_runtime {

/**
 * @brief Callback function type for handling received UDP messages.
 * @details The Engine provides its own method matching this signature.
 *          It receives the parsed components of the message.
 * @param type The type of the message (e.g., "INPUT", "CMD").
 * @param name The name associated with the message (e.g., input name, command name).
 * @param value The value associated with the message.
 */
using UdpReceiveHandler = std::function<void(const std::string& /* type */, const std::string& /* name */, const std::string& /* value */ )>;
/**
 * @brief Callback function type for handling UDP communication errors.
 * @details The Engine provides its own method matching this signature.
 * @param errorMessage A string describing the error that occurred.
 */
using UdpErrorHandler = std::function<void(const std::string& /* error message */)>;

/**
 * @brief Manages UDP socket operations for sending and receiving messages asynchronously.
 * @details This class encapsulates Asio UDP socket logic, providing a simpler interface
 *          for the runtime Engine. It handles initialization, sending, receiving,
 *          and basic message parsing.
 */
class UdpCommunicator {
public:
    /**
     * @brief Constructs the UdpCommunicator.
     * @param io_ctx Reference to the Asio io_context for asynchronous operations.
     * @param receiver The callback function to invoke when a message is successfully received and parsed.
     * @param error_handler The callback function to invoke when a communication error occurs.
     */
    UdpCommunicator(asio::io_context& io_ctx, UdpReceiveHandler receiver, UdpErrorHandler error_handler);
    /**
     * @brief Destructor. Cleans up resources by calling shutdown().
     */
    ~UdpCommunicator();

    /**
     * @brief Initializes the UDP socket.
     * @details Opens the socket, binds it to the specified local port, and resolves the destination endpoint.
     * @param listen_port The local UDP port number to listen on.
     * @param dest_host The hostname or IP address of the destination (e.g., the GUI).
     * @param dest_port The UDP port number of the destination.
     * @return bool True if initialization was successful, false otherwise (error handler will be called).
     */
    bool initialize(int listen_port, const std::string& dest_host, int dest_port);
    /**
     * @brief Shuts down the communicator.
     * @details Closes the UDP socket if it's open.
     */
    void shutdown();

    /**
     * @brief Sends a message asynchronously to the configured destination.
     * @details The message format is determined by the caller (Engine).
     * @param message The string message to send.
     */
    void sendMessage(const std::string& message);

    /**
     * @brief Starts an asynchronous operation to receive the next UDP datagram.
     * @details This should be called once initially (e.g., by Engine::initialize)
     *          to begin listening. The receive loop continues automatically.
     */
    void startReceive();

private:
    /**
     * @brief Reference to the Asio io_context managing asynchronous operations.
     */
    asio::io_context& io_context_;

    /**
     * @brief The UDP socket used for communication.
     */
    asio::ip::udp::socket socket_;

    /**
     * @brief The resolved endpoint (IP address and port) of the destination (GUI).
     */
    asio::ip::udp::endpoint destinationEndpoint_;

    /**
     * @brief The endpoint from which the last datagram was received.
     */
    asio::ip::udp::endpoint senderEndpoint_;

    /**
     * @brief Buffer used for receiving incoming data.
     */
    std::array<char, 1024> recvBuffer_;

    /**
     * @brief Callback function invoked when a message is received.
     */
    UdpReceiveHandler receiveHandler_;

    /**
     * @brief Callback function invoked on communication errors.
     */
    UdpErrorHandler errorHandler_;
    
    /**
     * @brief Flag indicating whether the communicator has been successfully initialized.
     */
    bool initialized_ = false;

    /**
     * @brief Internal handler called by Asio when an async_receive_from operation completes.
     * @param error The error code associated with the operation.
     * @param bytes_transferred The number of bytes received.
     */
    void handleReceive(const asio::error_code& error, std::size_t bytes_transferred);
    
    /**
     * @brief Internal handler called by Asio when an async_send_to operation completes.
     * @param error The error code associated with the operation.
     * @param bytes_transferred The number of bytes sent (unused in current implementation).
     */
    void handleSend(const asio::error_code& error, std::size_t bytes_transferred);
    
    /**
     * @brief Parses a received raw data buffer into type, name, and value components.
     * @details Implements a simple text-based protocol (e.g., TYPE|NAME|VALUE).
     *          Calls the receiveHandler_ upon successful parsing or errorHandler_ on errors.
     * @param data Pointer to the start of the received data buffer.
     * @param length The number of bytes received.
     */
    void parseAndDelegate(const char* data, std::size_t length);
};

} // namespace ifa_runtime
#endif // IFA_RUNTIME_UDP_H