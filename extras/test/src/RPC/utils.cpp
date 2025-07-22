#include <string>
#include <sstream>
#include <iomanip>

// convert a byte array into an hex string
std::string to_hex_string(const unsigned char* data, size_t size) {
  std::ostringstream oss;
  oss << std::hex << std::setw(2) << std::setfill('0');
  for (size_t i = 0; i < size; ++i) {
    oss << static_cast<int>(data[i]);
  }
  return oss.str();
}
