#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include <iostream>
#include <list>
#include <memory>
#include <fstream>
#include <sstream>
#include <chrono>
#include <boost/filesystem.hpp>
#include <limits>



using btcp = boost::asio::ip::tcp;

struct Connection
{
    btcp::socket socket;
    boost::asio::streambuf read_buffer;
    Connection(boost::asio::io_service &io_service) : socket(io_service), read_buffer() {}
};

class Server
{
    boost::asio::io_service ioservice;
    btcp::acceptor acceptor;
    std::list<Connection> connections;

public:
    Server(uint16_t port) : acceptor(ioservice, btcp::endpoint(btcp::v4(), port)) { start_accept(); }

    void on_read(std::list<Connection>::iterator con_handle, boost::system::error_code const &err, size_t bytes_transfered)
    {
        if (bytes_transfered > 0)
        {
            std::istream is(&con_handle->read_buffer);
            std::string line;
            std::getline(is, line);

            std::string response;
            if (line == "username")
            {
                response = "Username: John Doe\n";
            }
            else if (line == "hostname")
            {
                response = "Hostname: MyServer\n";
            }
            else if (line == "serverdatetime")
            {
                auto now = std::chrono::system_clock::now();
                auto time = std::chrono::system_clock::to_time_t(now);
                response = "Server Date and Time: " + std::string(std::ctime(&time));
            }
            else if (line.find("ls") == 0)
            {
                std::string path = line.substr(3);
                response = listDirectoryContents(path);
            }
            else if (line.find("mkfile") == 0)
            {
                std::string filename = line.substr(7);
                response = createFile(filename);
            }
            else if (line.find("mkdir") == 0)
            {
                std::string dirname = line.substr(6);
                response = createDirectory(dirname);
            }
            else if (line.find("rmfile") == 0)
            {
                std::string filename = line.substr(7);
                response = removeFile(filename);
            }
            else if (line.find("rmdir") == 0)
            {
                std::string dirname = line.substr(6);
                response = removeDirectory(dirname);
            }
            else if (line.find("readfile") == 0)
            {
                std::string params = line.substr(9);
                std::istringstream iss(params);
                std::string filename;
                size_t bytes = std::numeric_limits<size_t>::max();
                if (!(iss >> filename))
                {
                    response = "Invalid parameters for readfile command";
                }
                else if (iss >> bytes)
                {
                    response = readFile(filename, bytes);
                }
                else
                {
                    response = readFile(filename);
                }
            }
            else if (line == "quit")
            {
                response = "Goodbye!\n";
                con_handle->socket.close();
            }
            else
            {
                response = "Invalid command\n";
            }

            boost::asio::async_write(con_handle->socket, boost::asio::buffer(response),
                                     boost::bind(&Server::on_write, this, con_handle, response,
                                                 boost::asio::placeholders::error));
        }

        if (err)
        {
            std::cerr << "Error: " << err.message() << std::endl;
            connections.erase(con_handle);
        }
    }

    void do_async_read(std::list<Connection>::iterator con_handle)
    {
        auto handler = boost::bind(&Server::on_read, this, con_handle, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred);
        boost::asio::async_read_until(con_handle->socket, con_handle->read_buffer, "\n", handler);
    }

    void on_write(std::list<Connection>::iterator con_handle, std::string response, boost::system::error_code const &err)
    {
        if (!err)
        {
            std::cout << "Response sent: " << response;
        }
        else
        {
            std::cerr << "Error: " << err.message() << std::endl;
            connections.erase(con_handle);
        }
    }

    void on_accept(std::list<Connection>::iterator con_handle, boost::system::error_code const &err)
    {
        if (!err)
        {
            std::cout << "Connection from: " << con_handle->socket.remote_endpoint().address().to_string() << "\n";
            std::cout << "Sending welcome message\n";
            std::string welcomeMsg = "Welcome to the server!\n";
            boost::asio::async_write(con_handle->socket, boost::asio::buffer(welcomeMsg),
                                     boost::bind(&Server::on_write, this, con_handle, welcomeMsg,
                                                 boost::asio::placeholders::error));
            do_async_read(con_handle);
        }
        else
        {
            std::cerr << "Error: " << err.message() << std::endl;
            connections.erase(con_handle);
        }
        start_accept();
    }

    void start_accept()
    {
        auto con_handle = connections.emplace(connections.begin(), ioservice);
        auto handler = boost::bind(&Server::on_accept, this, con_handle, boost::asio::placeholders::error);
        acceptor.async_accept(con_handle->socket, handler);
    }

    std::string listDirectoryContents(const std::string &path)
{
    std::stringstream response;
    try
    {
        boost::filesystem::path directoryPath(path);
        if (boost::filesystem::exists(directoryPath) && boost::filesystem::is_directory(directoryPath))
        {
            boost::filesystem::directory_iterator endIterator;
            for (boost::filesystem::directory_iterator dirIterator(directoryPath); dirIterator != endIterator; ++dirIterator)
            {
                response << dirIterator->path().filename().string() << "\n";
            }
        }
        else
        {
            response << "Invalid directory path: " << path << "\n";
        }
    }
    catch (const boost::filesystem::filesystem_error &ex)
    {
        response << "Error: " << ex.what() << "\n";
    }
    return response.str();
}

std::string createFile(const std::string &filename)
{
    std::stringstream response;
    try
    {
        std::ofstream file(filename);
        if (file)
        {
            response << "File created: " << filename << "\n";
        }
        else
        {
            response << "Failed to create file: " << filename << "\n";
        }
    }
    catch (const std::exception &ex)
    {
        response << "Error: " << ex.what() << "\n";
    }
    return response.str();
}

std::string createDirectory(const std::string &dirname)
{
    std::stringstream response;
    try
    {
        if (boost::filesystem::create_directory(dirname))
        {
            response << "Directory created: " << dirname << "\n";
        }
        else
        {
            response << "Failed to create directory: " << dirname << "\n";
        }
    }
    catch (const boost::filesystem::filesystem_error &ex)
    {
        response << "Error: " << ex.what() << "\n";
    }
    return response.str();
}

std::string removeFile(const std::string &filename)
{
    std::stringstream response;
    try
    {
        if (boost::filesystem::remove(filename))
        {
            response << "File removed: " << filename << "\n";
        }
        else
        {
            response << "Failed to remove file: " << filename << "\n";
        }
    }
    catch (const boost::filesystem::filesystem_error &ex)
    {
        response << "Error: " << ex.what() << "\n";
    }
    return response.str();
}

std::string removeDirectory(const std::string &dirname)
{
    std::stringstream response;
    try
    {
        if (boost::filesystem::remove_all(dirname) > 0)
        {
            response << "Directory removed: " << dirname << "\n";
        }
        else
        {
            response << "Failed to remove directory: " << dirname << "\n";
        }
    }
    catch (const boost::filesystem::filesystem_error &ex)
    {
        response << "Error: " << ex.what() << "\n";
    }
    return response.str();
}

std::string readFile(const std::string &filename, size_t bytes)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file)
    {
        return "Failed to open file: " + filename + "\n";
    }

    std::stringstream response;
    response << "File contents: " << filename << "\n";

    // Read 'bytes' number of bytes from the file and append to the response
    char buffer[1024];
    size_t totalBytesRead = 0;
    while (bytes > 0)
    {
        size_t bytesRead = std::min(bytes, sizeof(buffer));
        file.read(buffer, bytesRead);

        if (file.bad())
        {
            response << "Error while reading file: " << filename << "\n";
            break;
        }

        if (file.eof())
        {
            response << buffer;
            break;
        }

        response.write(buffer, bytesRead);
        totalBytesRead += bytesRead;
        bytes -= bytesRead;
    }
}




    if (totalBytesRead == 0)
    {
        response << "Empty file: " << filename << "\n";
    }

    return response.str();
}



    void run()
    {
        ioservice.run();
    }
};

int main()
{
    Server srv(12345);
    srv.run();
    return 0;
}


