#include <iostream>
#include <vector>
#include <string>
#include <asio.hpp>
#include <json/json.h>
#include <thread>
#include <asio/io_context.hpp>
#include <chrono>
#include "reservation_system.h"

using asio::ip::tcp;

const int number_of_threads(4);

///////////////////////////////////////////////////////////////////////////////
// Http responses!
///////////////////////////////////////////////////////////////////////////////

/// @brief Send 200 OK response with content jsonData
/// @param socket
/// @param jsonData
void sendHttpOkResponse(asio::ip::tcp::socket &socket, const Json::Value &jsonData)
{
    std::string response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: application/json\r\n";
    Json::StreamWriterBuilder writer;
    std::string jsonStr = Json::writeString(writer, jsonData);
    response += "Content-Length: " + std::to_string(jsonStr.length()) + "\r\n";
    response += "\r\n"; // Empty line to separate headers from body
    response += jsonStr;
    // std::cout << response << std::endl;
    asio::write(socket, asio::buffer(response));
}

/// @brief Send 404 Not Found
/// @param socket
void sendHttpNotFoundResponse(asio::ip::tcp::socket &socket)
{
    std::string response = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
    asio::write(socket, asio::buffer(response));
}

/// @brief Send 405 Not allowed
/// @param socket
void sendHttpMethodNotAllowedResponse(asio::ip::tcp::socket &socket)
{
    std::string response = "HTTP/1.1 405 Method Not Allowed\r\nContent-Length: 0\r\n\r\n";
    asio::write(socket, asio::buffer(response));
}

/// @brief Send 500 Internal Error
/// @param socket
/// @param error
void sendHttpErrorResponse(asio::ip::tcp::socket &socket, const std::string &error)
{
    Json::Value jsonData;
    jsonData["error"] = error;

    Json::StreamWriterBuilder writer;
    std::string jsonStr = Json::writeString(writer, jsonData);

    std::string response = "HTTP/1.1 500 Internal Server Error\r\n";
    response += "Content-Type: application/json\r\n";
    response += "Content-Length: " + std::to_string(jsonStr.size()) + "\r\n\r\n";
    response += jsonStr;

    asio::write(socket, asio::buffer(response));
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Session class to dipatch dispatch requests asyncronously
class Session : public std::enable_shared_from_this<Session>
{
public:
    /// @brief Constructor
    /// @param socket 
    /// @param reservationSystem 
    Session(tcp::socket socket, ReservationSystem &reservationSystem)
        : socket_(std::move(socket)), reservationSystem_(reservationSystem) {}

    /// @brief Session async callback
    void start()
    {
        async_read();
    }

private:
    /// @brief This method parses the request asyncronously
    /// It will try to read the full message body if not already in the buffer.
    /// Then it will handle the proper events received in the request
    void async_read()
    {
        auto self(shared_from_this());
        asio::async_read_until(socket_, buffer_, "\r\n\r\n",
                               [this, self](asio::error_code ec, std::size_t length)
                               {
                                   if (!ec)
                                   {
                                       std::istream is(&buffer_);
                                       std::string buffer_content((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
                                       auto content_length = get_content_length(buffer_content);

                                       std::size_t headers_end_pos = buffer_content.find("\r\n\r\n");
                                       std::size_t body_length = (headers_end_pos != std::string::npos) ? buffer_content.size() - headers_end_pos - 4 : 0;

                                       auto remaining = content_length - body_length;

                                       if (remaining > 0)
                                       {
                                           asio::async_read(socket_, buffer_, asio::transfer_exactly(remaining),
                                                            [this, self, buffer_content](asio::error_code ec2, std::size_t transferred)
                                                            {
                                                                // std::cout << "More data needed: async_read -> " << ec2.message() << "\n";
                                                                // std::cout << "transferred: " << transferred << std::endl;
                                                                // std::cout << "buffer: " << buffer_.size() << std::endl;

                                                                std::istream is_more(&buffer_);
                                                                std::string more_content((std::istreambuf_iterator<char>(is_more)), std::istreambuf_iterator<char>());

                                                                handle_request(buffer_content + more_content);
                                                                buffer_.consume(buffer_.size()); // Clear the buffer
                                                                async_read();
                                                            });
                                       }
                                       else
                                       {
                                           handle_request(buffer_content);
                                           buffer_.consume(buffer_.size()); // Clear the buffer
                                           async_read();
                                       }
                                   }
                               });
    }

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief This here routes requests to our APIs
    /// @param request_data string with all request data (header + body)

    void handle_request(const std::string &request_data)
    {
        std::istringstream request_stream(request_data);
        std::string request_line;
        getline(request_stream, request_line);
        std::istringstream request_line_stream(request_line);
        std::string request_method, request_target;
        request_line_stream >> request_method >> request_target;

        // Find the end of the request headers
        size_t pos = request_data.find("\r\n\r\n");
        if (pos != std::string::npos)
        {
            std::string request_body = request_data.substr(pos + 4); // Skip "\r\n\r\n"
            std::cout << request_target << " " << request_method << " " << request_body << std::endl;
            if (request_method == "GET")
            {
                if (request_target == "/movies")
                {
                    auto response = reservationSystem_.getAllPlayingMoviesJson();
                    sendHttpOkResponse(socket_, response);
                }
                else
                {
                    sendHttpNotFoundResponse(socket_);
                }
            }
            else if (request_method == "POST")
            {                
                Json::Value requestBodyJson;
                std::istringstream iss(request_body);
                if (!request_body.empty())
                    iss >> requestBodyJson;

                if (request_target == "/find")
                {
                    // ... Handle POST logic ...
                    if (requestBodyJson.isMember("movie"))
                    {
                        std::string movieTitle = requestBodyJson["movie"].asString();
                        auto response = reservationSystem_.getTheatersShowingMovieJson(movieTitle);
                        sendHttpOkResponse(socket_, response);
                    }
                }
                else if (request_target == "/bookings")
                {
                    // ... Handle POST logic ...
                    if (requestBodyJson.isMember("movie") && requestBodyJson.isMember("theater"))
                    {
                        std::string movieTitle = requestBodyJson["movie"].asString();
                        std::string theaterTitle = requestBodyJson["theater"].asString();
                        auto response = reservationSystem_.getBookings(theaterTitle, movieTitle);
                        sendHttpOkResponse(socket_, response);
                    }
                }
                else if (request_target == "/seats")
                {
                    // ... Handle POST logic ...
                    if (requestBodyJson.isMember("movie") && requestBodyJson.isMember("theater") && requestBodyJson.isMember("seats"))
                    {
                        std::string movieTitle = requestBodyJson["movie"].asString();
                        std::string theaterTitle = requestBodyJson["theater"].asString();
                        Json::Value seatsJson = requestBodyJson["seats"];

                        if (!seatsJson.isArray())
                        {
                            std::cout << " Handle error: 'seats' field is not an array" << std::endl;
                        }

                        std::vector<int> seats;
                        for (Json::Value::ArrayIndex i = 0; i < seatsJson.size(); ++i)
                        {
                            if (seatsJson[i].isInt())
                            {
                                int seatNumber = seatsJson[i].asInt();
                                seats.push_back(seatNumber);
                            }
                            else
                            {
                                std::cout << " Handle error: 'seats' not int" << std::endl;
                            }
                        }

                        // Now 'seats' vector contains the list of seat numbers

                        auto response = reservationSystem_.bookSeats(theaterTitle, movieTitle, seats);

                        if (response)
                        {
                            sendHttpOkResponse(socket_, response);
                        }
                        else
                        {
                            sendHttpErrorResponse(socket_, "No available seats.");
                        }
                    }
                }

                sendHttpMethodNotAllowedResponse(socket_);
            }
            else
            {
                sendHttpMethodNotAllowedResponse(socket_);
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Retrieves the 'Content-Length' field from the HTPP header
    std::size_t get_content_length(const std::string &buffer_content)
    {
        std::string content_length_str = "Content-Length: ";
        std::size_t pos = buffer_content.find(content_length_str);

        if (pos != std::string::npos)
        {
            pos += content_length_str.length();
            std::size_t end_pos = buffer_content.find("\r\n", pos);
            if (end_pos != std::string::npos)
            {
                return std::stoul(buffer_content.substr(pos, end_pos - pos));
            }
        }
        return 0;
    }

    ///////////////////////////////////////////////////////////////////////////////

    tcp::socket socket_;
    asio::streambuf buffer_;
    ReservationSystem &reservationSystem_;
};

///////////////////////////////////////////////////////////////////////////////
/// @brief Server class for starting async dispatchers
class Server
{
public:
    /// @brief Constructor
    /// @param io_context 
    /// @param endpoint 
    /// @param reservationSystem 
    Server(asio::io_context &io_context, const tcp::endpoint &endpoint, ReservationSystem &reservationSystem)
        : acceptor_(io_context, endpoint), reservationSystem_(reservationSystem)
    {
        accept();
    }

private:
    void accept()
    {
        acceptor_.async_accept([this](asio::error_code ec, tcp::socket socket)
                               {
                                   // std::cout << "async_accept -> " << ec.message() << "\n";
                                   if (!ec)
                                   {
                                       std::make_shared<Session>(std::move(socket), reservationSystem_)->start();
                                   }
                                   accept(); // Accept the next connection
                               });
    }

    tcp::acceptor acceptor_;
    ReservationSystem &reservationSystem_;
};

///////////////////////////////////////////////////////////////////////////////
/// @brief Will initialize 'number_of_threads' to listen to requests.
/// @param argc
/// @param argv Need to provide at least a filename with a json theater structure
/// @return
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1; // Return an error code
    }

    const std::string filename = argv[1];

    try
    {
        asio::io_context io_context;
        asio::io_context::work work(io_context); // Keep the io_context active

        // Create a thread pool with n 'number_of_threads'
        std::vector<std::thread> thread_pool;
        for (int i = 0; i < number_of_threads; ++i)
        {
            std::cout << "Initialized " << i + 1 << " threads!" << std::endl;
            thread_pool.emplace_back([&io_context]
                                     { io_context.run(); });
        }

        ReservationSystem reservationSystem(filename);

        // Start the server
        tcp::endpoint endpoint(tcp::v4(), 8080);
        Server server(io_context, endpoint, reservationSystem);
        std::cout << "Opened server in port: 8080" << std::endl;
        std::cout << "Avaliable Movies: " << reservationSystem.getAllPlayingMoviesJson() << std::endl;

        // Wait for all threads in the thread pool to finish
        for (auto &thread : thread_pool)
        {
            thread.join();
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////////////