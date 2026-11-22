#ifndef PM_PACKAGE_FILE_HPP
#define PM_PACKAGE_FILE_HPP

#include <string>
#include <utility>

namespace pm {

class PackageFile {
public:
    std::string path;
    std::string content;
    bool executable = false;

    PackageFile() = default;
    PackageFile(std::string path, std::string content, bool executable = false)
        : path(std::move(path)), content(std::move(content)), executable(executable) {}

    bool operator==(const PackageFile& other) const = default;
};

} // namespace pm

#endif // PM_PACKAGE_FILE_HPP
