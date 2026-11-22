#ifndef PM_PACKAGE_HPP
#define PM_PACKAGE_HPP

#include "Version.hpp"
#include "Requirement.hpp"
#include "PackageFile.hpp"
#include "../../../Sequence/mutable_array_sequence.h"

#include <expected>
#include <string>

namespace pm {

class Package {
private:
    std::string name;
    Version version;
    std::string description;

    mutable_array_sequence<Requirement> dependencies;
    mutable_array_sequence<Requirement> conflicts;
    mutable_array_sequence<std::string> replaces;
    mutable_array_sequence<PackageFile> files;
    mutable_array_sequence<std::string> pre_install;
    mutable_array_sequence<std::string> post_install;

public:
    Package() = default;
    Package(std::string name, Version version, std::string description = "");

    // Геттеры для доступа к приватным полям
    const std::string& get_name() const;
    const Version& get_version() const;
    const std::string& get_description() const;
    const mutable_array_sequence<Requirement>& get_dependencies() const;
    const mutable_array_sequence<Requirement>& get_conflicts() const;
    const mutable_array_sequence<std::string>& get_replaces() const;
    const mutable_array_sequence<PackageFile>& get_files() const;
    const mutable_array_sequence<std::string>& get_pre_install() const;
    const mutable_array_sequence<std::string>& get_post_install() const;

    // Fluent API для удобного построения пакета
    Package& add_dependency(const Requirement& req);
    std::expected<void, std::string> add_dependency(const std::string& req_str);
    Package& add_conflict(const Requirement& req);
    std::expected<void, std::string> add_conflict(const std::string& req_str);
    Package& add_replaces(const std::string& package_name);
    Package& add_file(const PackageFile& file);
    Package& add_file(const std::string& path, const std::string& content, bool executable = false);
    Package& add_pre_install(const std::string& command);
    Package& add_post_install(const std::string& command);

    // Валидация
    static std::expected<void, std::string> validate_package_name(const std::string& name);
    static std::expected<void, std::string> validate_file_path(const std::string& path);
    std::expected<void, std::string> validate() const;

    std::string to_string() const;
    bool operator==(const Package& other) const;
};

} // namespace pm

#endif // PM_PACKAGE_HPP
