#ifndef PM_INSTALLED_REGISTRY_HPP
#define PM_INSTALLED_REGISTRY_HPP

#include "../Domain/Package.hpp"
#include "../../../TreeStructures/include/Dictionary/Dictionary.hpp"
#include "../../../TreeStructures/include/Dictionary/BinaryTreeDictionaryStorage.hpp"
#include "../../../TreeStructures/include/BinaryTree/BalancedTree.hpp"
#include "../../../Sequence/mutable_array_sequence.h"

#include <expected>
#include <string>

namespace pm {

class InstalledRegistry {
private:
    Dictionary<std::string, Package> installed_packages;
    Dictionary<std::string, std::string> file_owners; // relative_path -> package_name
    mutable_array_sequence<std::string> installed_names;

public:
    InstalledRegistry();

    InstalledRegistry(const InstalledRegistry& other) = default;
    InstalledRegistry& operator=(const InstalledRegistry& other) = default;
    InstalledRegistry(InstalledRegistry&& other) noexcept = default;
    InstalledRegistry& operator=(InstalledRegistry&& other) noexcept = default;

    bool is_installed(const std::string& name) const;
    std::expected<Package, std::string> get_installed(const std::string& name) const;

    std::expected<void, std::string> record_install(const Package& pkg);
    std::expected<void, std::string> record_remove(const std::string& name);

    bool has_file_owner(const std::string& relative_path) const;
    std::expected<std::string, std::string> get_file_owner(const std::string& relative_path) const;

    const mutable_array_sequence<std::string>& get_installed_names() const;
    std::size_t get_count() const;

    void clear();

    std::string serialize() const;
    static std::expected<InstalledRegistry, std::string> deserialize(const std::string& data);
};

} // namespace pm

#endif // PM_INSTALLED_REGISTRY_HPP
