#include "../../../include/PackageManager/Domain/Version.hpp"

#include <cctype>
#include <sstream>

namespace pm {

std::expected<Version, std::string> Version::parse(const std::string& str) {
    if (str.empty()) {
        return std::unexpected("Строка версии пуста");
    }

    int numbers[3] = {0, 0, 0};
    int part_index = 0;
    int current_val = 0;
    bool has_digit_in_part = false;

    for (std::size_t i = 0; i < str.size(); ++i) {
        char ch = str[i];
        if (std::isdigit(static_cast<unsigned char>(ch))) {
            has_digit_in_part = true;
            current_val = current_val * 10 + (ch - '0');
        } else if (ch == '.') {
            if (!has_digit_in_part) {
                return std::unexpected("Некорректный формат версии: пустой сегмент перед точкой");
            }
            if (part_index >= 2) {
                return std::unexpected("Версия не может содержать более 3 сегментов (major.minor.patch)");
            }
            numbers[part_index++] = current_val;
            current_val = 0;
            has_digit_in_part = false;
        } else {
            return std::unexpected(std::string("Недопустимый символ в версии: '") + ch + "'");
        }
    }

    if (!has_digit_in_part) {
        return std::unexpected("Некорректный формат версии: отсутствует число после точки");
    }

    numbers[part_index] = current_val;

    return Version(numbers[0], numbers[1], numbers[2]);
}

std::string Version::to_string() const {
    std::ostringstream oss;
    oss << major << "." << minor << "." << patch;
    return oss.str();
}

} // namespace pm
