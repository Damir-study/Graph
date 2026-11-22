#ifndef PM_REQUIREMENT_HPP
#define PM_REQUIREMENT_HPP

#include "Version.hpp"

#include <expected>
#include <string>

namespace pm {

enum class VersionOp {
    ANY,            // Любая версия (просто имя пакета)
    EQUAL,          // ==
    GREATER_EQUAL,  // >=
    LESS_EQUAL,     // <=
    GREATER,        // >
    LESS            // <
};

class Requirement {
public:
    std::string package_name;
    VersionOp op = VersionOp::ANY;
    Version version = Version(0, 0, 0);

    Requirement() = default;
    Requirement(std::string name, VersionOp op, Version ver);
    Requirement(std::string name);

    static std::expected<Requirement, std::string> parse(const std::string& str);

    bool satisfies(const Version& v) const;

    std::string to_string() const;

    bool operator==(const Requirement& other) const = default;
};

} // namespace pm

#endif // PM_REQUIREMENT_HPP
