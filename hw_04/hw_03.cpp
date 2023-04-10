//
//
//#include <iostream>
//#include <fstream>
#include <filesystem>
//#include <map>
//
//using namespace std;
namespace sfs = std::filesystem;
//
//// Функция, которая создает директорию, если ее не существует
//void create_directory(const string& path) {
//    if (!fs::exists(path)) {
//        fs::create_directory(path);
//    }
//}
//
//// Функция, которая удаляет пустые вложенные папки
//void remove_empty_directories(const string& path) {
//    if (fs::is_directory(path)) {
//        // Рекурсивно обходим все вложенные папки
//        for (const auto& entry : fs::directory_iterator(path)) {
//            remove_empty_directories(entry.path().string());
//        }
//        // Если папка пустая, то удаляем ее
//        if (fs::is_empty(path)) {
//            fs::remove(path);
//        }
//    }
//}
//
//int main(int argc, char* argv[]) {
//    // Проверяем, что количество аргументов правильное
//    if (argc != 3) {
//        cerr << "Usage: " << argv[0] << " <directory_path> <rules_file_path>" << endl;
//        return 1;
//    }
//
//    // Считываем путь до директории, которую нужно обработать
//    string directory_path = argv[1];
//    // Считываем путь до файла с правилами
//    string rules_file_path = argv[2];
//
//    // Читаем правила из файла и сохраняем их в словарь
//    map<string, string> rules;
//    ifstream rules_file(rules_file_path);
//    if (rules_file.is_open()) {
//        string line;
//        while (getline(rules_file, line)) {
//            // Разбиваем строку на две части по символу ":"
//            size_t colon_pos = line.find(":");
//            if (colon_pos != string::npos) {
//                string extension = line.substr(0, colon_pos);
//                string folder_name = line.substr(colon_pos + 1);
//                rules[extension] = folder_name;
//            }
//        }
//        rules_file.close();
//    } else {
//        cerr << "Cannot open file " << rules_file_path << endl;
//        return 1;
//    }
//
//    // Обходим директорию и перемещаем файлы в нужные папки
//    for (const auto& entry : fs::recursive_directory_iterator(directory_path)) {
//        if (entry.is_regular_file()) {
//            string file_path = entry.path().string();
//            string file_extension = entry.path().extension().string();
//            // Удаляем точку из расширения
//            file_extension.erase(0, 1);
//            if (rules.count(file_extension) > 0) {
//                string folder_name = rules[file_extension];
//                string folder_path = directory_path + "/" + folder_name;
//                create_directory(folder_path);
//                string new_file_path = folder_path + "/" + entry.path().filename().string();
//                fs::rename(file_path, new_file_path);
//            }
//        }
//    }
//
//    // Удаляем пустые вложенные папки
//    for (const auto& entry : fs::recursive_directory_iterator(directory_path)) {
//        if (entry.is_directory()) {
//            if (fs::is_empty(entry.path())) {
//                fs::remove(entry.path());
//            }
//        }
//    }
//
//    return 0;
//
//}

#include <boost/program_options.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <string>
#include <iostream>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

int main(int argc, char** argv)
{
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "produce help message")
            ("dir,d", po::value<std::string>(), "directory to sort")
            ("config,c", po::value<std::string>(), "configuration file");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 1;
    }

    if (!vm.count("dir")) {
        std::cerr << "Error: directory parameter is required.\n";
        return 1;
    }

    if (!vm.count("config")) {
        std::cerr << "Error: configuration file parameter is required.\n";
        return 1;
    }

    // Read configuration file
    std::ifstream confFile(vm["config"].as<std::string>());
    if (!confFile) {
        std::cerr << "Error: can't open configuration file.\n";
        return 1;
    }

    std::map<std::string, std::string> extToDir;
    std::string line;
    while (std::getline(confFile, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }
        std::string ext;
        std::string dir;
        std::stringstream ss(line);
        if (std::getline(ss, ext, ':') && std::getline(ss, dir)) {
            extToDir[ext] = dir;
        }
    }

    // Process files in the directory
    fs::path dirPath(vm["dir"].as<std::string>());
    if (!fs::is_directory(fs::absolute(dirPath))) {
        std::cerr << "Error: specified directory doesn't exist or is not a directory.\n";
        return 1;
    }

        for (const auto& entry : sfs::recursive_directory_iterator(dirPath.string())) {
        if (entry.is_regular_file()) {
            std::string file_path = entry.path().string();
            std::string file_extension = entry.path().extension().string();
            // Удаляем точку из расширения
            file_extension.erase(0, 1);
            if (extToDir.count(file_extension) > 0) {
                std::string folder_name = extToDir[file_extension];
                fs::path folder_path = dirPath/folder_name;
                fs::create_directory(folder_path);
                fs::path new_file_path = folder_path / entry.path().filename().string();
                fs::rename(file_path, new_file_path);
            }
        }
    }

//    for (auto & item : fs::recursive_directory_iterator(dirPath))
//        //std::cout << item << '\n';
//        if (fs::is_directory(item)) {
//            continue;
//        } else if (fs::is_regular_file(item)) {
//
//            std::string ext = item.path().extension().string();
//            if (!ext.empty() && ext[0] == '.') {
//                ext = ext.substr(1);
//            }
//
//            fs::path extDir = "Other";
//            if (extToDir.count(ext)) {
//                extDir = extToDir[ext];
//            }
//            //std::cout << extDir << '\n';
//
//            fs::path targetDir = dirPath / extDir / item.path().filename();
//            std::cout << targetDir << '\n';
//            if (!fs::exists(targetDir.parent_path())) {
//                fs::create_directory(targetDir.parent_path());
//            }


//            std::cout << dirPath/targetDir << '\n';
//            if (fs::exists(item)) {
//                std::cout << item << " Exists!\n";
//            }
//            if (!fs::exists(dirPath/targetDir)) {
//                std::cout << dirPath/targetDir << " Not Exist!\n";
//            }
//            fs::copy_file(item, targetDir, fs::copy_option::overwrite_if_exists);
//            fs::remove(item);

//            auto extDirIt = extToDir.find(ext);
//            if (extDirIt == extToDir.end()) {
//                extDirIt = extToDir.find("ext");
//                if (extDirIt == extToDir.end()) {
//                    extDirIt = extToDir.find("Other");
//                }
//            }
//            if (extDirIt != extToDir.end()) {
//                fs::path targetDir = dirPath / extDirIt->second;
//                if (!fs::exists(targetDir)) {
//                    fs::create_directory(targetDir);
//                }
//                fs::rename(item, targetDir / item.path().filename());
//            }
       // }
    //}

    return 0;
}
