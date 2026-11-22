#include "../../../include/PackageManager/Domain/Requirement.hpp"

#include <cctype>
#include <sstream>

namespace pm {

namespace {

std::string trim(const std::string& str) {
    std::size_t start = 0;
    while (start < str.size() && std::isspace(static_cast<unsigned char>(str[start]))) {
        start++;
    }
    std::size_t end = str.size();
    while (end > start && std::isspace(static_cast<unsigned char>(str[end - 1]))) {
        end--;
    }
    return str.substr(start, end - start);
}

} // namespace

Requirement::Requirement(std::string name, VersionOp op, Version ver)
    : package_name(std::move(name)), op(op), version(ver) {}

Requirement::Requirement(std::string name)
    : package_name(std::move(name)), op(VersionOp::ANY), version(0, 0, 0) {}

std::expected<Requirement, std::string> Requirement::parse(const std::string& raw_str) {
    std::string str = trim(raw_str);
    if (str.empty()) {
        return std::unexpected("Строка требования пуста");
    }

    // Ищем операторы: >=, <=, ==, >, <
    std::size_t op_pos = std::string::npos;
    VersionOp op = VersionOp::ANY;
    std::size_t op_len = 0;

    for (std::size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '>' && i + 1 < str.size() && str[i + 1] == '=') {
            op_pos = i;
            op = VersionOp::GREATER_EQUAL;
            op_len = 2;
            break;
        }
        if (str[i] == '<' && i + 1 < str.size() && str[i + 1] == '=') {
            op_pos = i;
            op = VersionOp::LESS_EQUAL;
            op_len = 2;
            break;
        }
        if (str[i] == '=' && i + 1 < str.size() && str[i + 1] == '=') {
            op_pos = i;
            op = VersionOp::EQUAL;
            op_len = 2;
            break;
        }
        if (str[i] == '>') {
            op_pos = i;
            op = VersionOp::GREATER;
            op_len = 1;
            break;
        }
        if (str[i] == '<') {
            op_pos = i;
            op = VersionOp::LESS;
            op_len = 1;
            break;
        }
    }

    if (op_pos == std::string::npos) {
        // Требование без оператора — просто имя пакета (любая версия)
        return Requirement(str);
    }

    std::string name = trim(str.substr(0, op_pos));
    if (name.empty()) {
        return std::unexpected("Отсутствует имя пакета в требовании");
    }

    std::string ver_str = trim(str.substr(op_pos + op_len));
    if (ver_str.empty()) {
        return std::unexpected("Отсутствует версия после оператора сравнения");
    }

    auto ver_res = Version::parse(ver_str);
    if (!ver_res) {
        return std::unexpected(ver_res.error());
    }

    return Requirement(name, op, *ver_res);
}

bool Requirement::satisfies(const Version& v) const {
    switch (op) {
        case VersionOp::ANY:
            return true;
        case VersionOp::EQUAL:
            return v == version;
        case VersionOp::GREATER_EQUAL:
            return v >= version;
        case VersionOp::LESS_EQUAL:
            return v <= version;
        case VersionOp::GREATER:
            return v > version;
        case VersionOp::LESS:
            return v < version;
    }
    return false;
}

std::string Requirement::to_string() const {
    if (op == VersionOp::ANY) {
        return package_name;
    }
    std::string op_str;
    switch (op) {
        case VersionOp::EQUAL: op_str = "=="; break;
        case VersionOp::GREATER_EQUAL: op_str = ">="; break;
        case VersionOp::LESS_EQUAL: op_str = "<="; break;
        case VersionOp::GREATER: op_str = ">"; break;
        case VersionOp::LESS: op_str = "<"; break;
        default: break;
    }
    return package_name + " " + op_str + " " + version.to_string();
}

} // namespace pm
