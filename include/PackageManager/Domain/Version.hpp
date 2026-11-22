#ifndef PM_VERSION_HPP
#define PM_VERSION_HPP

#include <compare>
#include <expected>
#include <iosfwd>
#include <string>

namespace pm {

class Version {
public:
    int major = 0;
    int minor = 0;
    int patch = 0;

    Version() = default;
    Version(int maj, int min, int pat)
        : major(maj), minor(min), patch(pat) {}

    static std::expected<Version, std::string> parse(const std::string& str);

    std::string to_string() const;

    auto operator<=>(const Version& other) const = default;
    bool operator==(const Version& other) const = default;
};

} // namespace pm

#endif // PM_VERSION_HPP
