

#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

    using boost::asio::ip::tcp;

    class Session : public std::enable_shared_from_this<Session> {
    public:
        Session(tcp::socket socket) : socket_(std::move(socket)) {}

        void start() {
            doRead();
        }

    private:
        void doRead() {
            auto self(shared_from_this());
            boost::asio::async_read_until(socket_, buffer_, '\n',
                                          [this, self](boost::system::error_code ec, std::size_t /*length*/) {
                                              if (!ec) {
                                                  std::istream is(&buffer_);
                                                  std::string command;
                                                  std::getline(is, command);

                                                  // Обработка команды
                                                  std::string response = processCommand(command);

                                                  boost::asio::async_write(socket_, boost::asio::buffer(response),
                                                                           [this, self](boost::system::error_code ec,
                                                                                        std::size_t /*length*/) {
                                                                               if (!ec) {
                                                                                   doRead();
                                                                               }
                                                                           });
                                              }
                                          });
        }

#include <iostream>
#include <fstream>

#include <sstream>
#include <boost/filesystem.hpp>

std::string processCommand(const std::string &command) {
    std::istringstream iss(command);
    std::string cmd;
    iss >> cmd;

    if (cmd == "username") {
        // Команда: username
        // Возвращает имя пользователя
        return "Username: " + std::string(getenv("USER")) + "\n";
    } else if (cmd == "hostname") {
        // Команда: hostname
        // Возвращает имя компьютера
        return "Hostname: " + boost::asio::ip::host_name() + "\n";
    } else if (cmd == "serverdatetime") {
        // Команда: serverdatetime
        // Возвращает время и дату на сервере
        std::time_t now = std::time(nullptr);
        std::string dateTime = std::ctime(&now);
        return "Server Date and Time: " + dateTime + "\n";
    } else if (cmd == "ls") {
        // Команда: ls <directory_path>
        // Возвращает содержимое указанной папки на сервере
        std::string directory;
        iss >> directory;

        boost::filesystem::directory_iterator end_itr;
        std::ostringstream oss;
        for (boost::filesystem::directory_iterator itr(directory); itr != end_itr; ++itr) {
            oss << itr->path().filename().string() << "\n";
        }

        return oss.str();
    } else if (cmd == "mkfile") {
        // Команда: mkfile <file_path>
        // Создает файл на сервере
        std::string filePath;
        iss >> filePath;

        std::ofstream file(filePath);
        if (file) {
            file.close();
            return "File created: " + filePath + "\n";
        } else {
            return "Failed to create file: " + filePath + "\n";
        }
    } else if (cmd == "mkdir") {
        // Команда: mkdir <directory_path>
        // Создает папку на сервере
        std::string directoryPath;
        iss >> directoryPath;

        if (boost::filesystem::create_directory(directoryPath)) {
            return "Directory created: " + directoryPath + "\n";
        } else {
            return "Failed to create directory: " + directoryPath + "\n";
        }
    } else if (cmd == "rmfile") {
        // Команда: rmfile <file_path>
        // Удаляет файл на сервере
        std::string filePath;
        iss >> filePath;

        if (boost::filesystem::remove(filePath)) {
            return "File removed: " + filePath + "\n";
        } else {
            return "Failed to remove file: " + filePath + "\n";
        }
    } else if (cmd == "rmdir") {
        // Команда: rmdir <directory_path>
        // Удаляет папку на сервере
        std::string directoryPath;
        iss >> directoryPath;

        if (boost::filesystem::remove_all(directoryPath)) {
            return "Directory removed: " + directoryPath + "\n";

            else
            {
                return "Failed to remove directory: " + directoryPath + "\n";
            }
        } else if (cmd == "readfile") {
            // Команда: readfile <file_path> [<bytes>]
            // Читает содержимое файла на сервере
            std::string filePath;
            iss >> filePath;

            std::ifstream file(filePath, std::ios::binary);
            if (file) {
                // Чтение указанного количества байт или всего файла (по умолчанию)
                std::size_t bytes = std::numeric_limits<std::size_t>::max();
                iss >> bytes;

                std::ostringstream oss;
                char buffer[4096];
                std::size_t bytesRead = 0;

                while (bytesRead < bytes && file.read(buffer, sizeof(buffer))) {
                    oss.write(buffer, file.gcount());
                    bytesRead += file.gcount();
                }

                return oss.str();
            } else {
                return "Failed to read file: " + filePath + "\n";
            }
        } else if (cmd == "quit") {
            // Команда: quit
            // Завершает сеанс
            return "Goodbye!\n";
        } else {
            // Неизвестная команда
            return "Unknown command: " + cmd + "\n";
        }
    }


    tcp::socket socket_;
    boost::asio::streambuf buffer_;
};

class Server {
public:
    Server(boost::asio::io_context &io_context, short port)
            : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
        doAccept();
    }

private:
    void doAccept() {
        acceptor_.async_accept(
                [this](boost::system::error_code ec, tcp::socket socket) {
                    if (!ec) {
                        std::make_shared<Session>(std::move(socket))->start();
                    }

                    doAccept();
                });
    }

    tcp::acceptor acceptor_;
};

int main() {
    try {
        boost::asio::io_context io_context;
        Server server(io_context, 12345);
        io_context.run();
    }
    catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}

