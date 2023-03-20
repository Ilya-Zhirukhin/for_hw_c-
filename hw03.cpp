#include <cstring> //std::memcmp
#include <fstream> //std::ifstream
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

// Define the Blob structure
struct Blob {
  uint16_t version;
  uint32_t size;
  char data[12];
};

// Define the exception classes
class version_exception : public std::runtime_error {
public:
  version_exception() : std::runtime_error("Invalid version") {}
};

class size_exception : public std::runtime_error {
public:
  size_exception() : std::runtime_error("Invalid size") {}
};

class data_exception : public std::runtime_error {
public:
  data_exception() : std::runtime_error("Invalid data") {}
};

// Read the binary file and extract the Blob fields
void do_fs(const std::string &filename) {
  std::ifstream reader(filename, std::ios::in | std::ios::binary);
  if (reader.is_open()) {
    // read bytes from file to buffer
    std::vector<char> bytes;
    int data_sz = 18;
    bytes.resize(data_sz);
    reader.read(reinterpret_cast<char *>(bytes.data()), data_sz);

    // Extract the fields of the Blob structure
    Blob blob;
    std::memcpy(&blob.version, bytes.data(), sizeof(blob.version));
    std::memcpy(&blob.size, bytes.data() + sizeof(blob.version), sizeof(blob.size));
    std::memcpy(blob.data, bytes.data() + sizeof(blob.version) + sizeof(blob.size), sizeof(blob.data));

    // Check if the values are valid, and throw the appropriate exception if not
    if (blob.version < 1 || blob.version > 7) {
      throw version_exception();
    }
    if (blob.size != 12) {
      throw size_exception();
    }
    if (std::memcmp(blob.data, "Hello, World", 12) != 0) {
      throw data_exception();
    }

    // Print the extracted values
    std::cout << "version: " << blob.version << std::endl;
    std::cout << "size: " << blob.size << std::endl;
    std::cout << "data: " << blob.data << std::endl;
  }
}

int main() {
  try {
    do_fs("bin_data.bin");
  }
  catch (const std::exception& e) {
    std::cerr << "Exception caught: " << e.what() << std::endl;
  }
  return 0;
}


  