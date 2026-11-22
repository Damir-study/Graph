#ifndef PM_PACKAGE_CATALOG_HPP
#define PM_PACKAGE_CATALOG_HPP

#include "../Domain/Package.hpp"
#include "../Domain/Requirement.hpp"
#include "../Domain/Version.hpp"
#include "../../../TreeStructures/include/Dictionary/Dictionary.hpp"
#include "../../../TreeStructures/include/Dictionary/BinaryTreeDictionaryStorage.hpp"
#include "../../../TreeStructures/include/BinaryTree/BalancedTree.hpp"
#include "../../../Sequence/mutable_array_sequence.h"

#include <expected>
#include <string>

namespace pm {

class PackageCatalog {
private:
    Dictionary<std::string, mutable_array_sequence<Package>> packages;
    mutable_array_sequence<std::string> package_names;
    std::size_t total_versions = 0;

public:
    PackageCatalog();

    PackageCatalog(const PackageCatalog& other) = default;
    PackageCatalog& operator=(const PackageCatalog& other) = default;
    PackageCatalog(PackageCatalog&& other) noexcept = default;
    PackageCatalog& operator=(PackageCatalog&& other) noexcept = default;

    std::expected<void, std::string> add_package(const Package& pkg);

    bool contains(const std::string& name) const;
    bool contains(const std::string& name, const Version& ver) const;

    std::expected<mutable_array_sequence<Package>, std::string> get_versions(const std::string& name) const;
    std::expected<Package, std::string> find_best_match(const Requirement& req) const;

    const mutable_array_sequence<std::string>& get_package_names() const;
    std::size_t get_package_count() const;
    std::size_t get_total_version_count() const;

    void clear();
};

} // namespace pm

#endif // PM_PACKAGE_CATALOG_HPP
