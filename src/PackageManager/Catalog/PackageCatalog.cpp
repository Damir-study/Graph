#include "../../../include/PackageManager/Catalog/PackageCatalog.hpp"

namespace pm {

PackageCatalog::PackageCatalog()
    : packages(new BinaryTreeDictionaryStorage<std::string, mutable_array_sequence<Package>>(
          new BalancedTree<DictionaryPair<std::string, mutable_array_sequence<Package>>>())),
      package_names(),
      total_versions(0) {}

std::expected<void, std::string> PackageCatalog::add_package(const Package& pkg) {
    auto val_res = pkg.validate();
    if (!val_res) return val_res;

    if (!packages.contains_key(pkg.get_name())) {
        mutable_array_sequence<Package> list;
        list.append(pkg);
        packages.add(pkg.get_name(), list);
        package_names.append(pkg.get_name());
        total_versions++;
        return {};
    }

    mutable_array_sequence<Package>& list = packages.get(pkg.get_name());
    for (int i = 0; i < list.get_length(); ++i) {
        if (list[i].get_version() == pkg.get_version()) {
            return std::unexpected("Версия пакета " + pkg.to_string() + " уже зарегистрирована в каталоге");
        }
    }

    list.append(pkg);
    total_versions++;
    return {};
}

bool PackageCatalog::contains(const std::string& name) const {
    return packages.contains_key(name);
}

bool PackageCatalog::contains(const std::string& name, const Version& ver) const {
    if (!packages.contains_key(name)) return false;
    const auto& list = packages.get(name);
    for (int i = 0; i < list.get_length(); ++i) {
        if (list[i].get_version() == ver) return true;
    }
    return false;
}

std::expected<mutable_array_sequence<Package>, std::string> PackageCatalog::get_versions(const std::string& name) const {
    if (!packages.contains_key(name)) {
        return std::unexpected("Пакет '" + name + "' не найден в каталоге");
    }
    return packages.get(name);
}

std::expected<Package, std::string> PackageCatalog::find_best_match(const Requirement& req) const {
    if (!packages.contains_key(req.package_name)) {
        return std::unexpected("Пакет '" + req.package_name + "' не найден в каталоге");
    }

    const auto& list = packages.get(req.package_name);
    const Package* best = nullptr;

    for (int i = 0; i < list.get_length(); ++i) {
        if (req.satisfies(list[i].get_version())) {
            if (best == nullptr || list[i].get_version() > best->get_version()) {
                best = &list[i];
            }
        }
    }

    if (!best) {
        return std::unexpected("В каталоге нет версии пакета '" + req.package_name + "', удовлетворяющей требованию " + req.to_string());
    }

    return *best;
}

const mutable_array_sequence<std::string>& PackageCatalog::get_package_names() const {
    return package_names;
}

std::size_t PackageCatalog::get_package_count() const {
    return static_cast<std::size_t>(package_names.get_length());
}

std::size_t PackageCatalog::get_total_version_count() const {
    return total_versions;
}

void PackageCatalog::clear() {
    packages.clear();
    package_names = mutable_array_sequence<std::string>();
    total_versions = 0;
}

} // namespace pm
