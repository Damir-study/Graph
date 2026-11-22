#include "../../../include/PackageManager/Domain/Package.hpp"

#include <cctype>

namespace pm {

Package::Package(std::string name, Version version, std::string description)
    : name(std::move(name)), version(version), description(std::move(description)) {}

const std::string& Package::get_name() const { return name; }
const Version& Package::get_version() const { return version; }
const std::string& Package::get_description() const { return description; }
const mutable_array_sequence<Requirement>& Package::get_dependencies() const { return dependencies; }
const mutable_array_sequence<Requirement>& Package::get_conflicts() const { return conflicts; }
const mutable_array_sequence<std::string>& Package::get_replaces() const { return replaces; }
const mutable_array_sequence<PackageFile>& Package::get_files() const { return files; }
const mutable_array_sequence<std::string>& Package::get_pre_install() const { return pre_install; }
const mutable_array_sequence<std::string>& Package::get_post_install() const { return post_install; }

Package& Package::add_dependency(const Requirement& req) {
    dependencies.append(req);
    return *this;
}

std::expected<void, std::string> Package::add_dependency(const std::string& req_str) {
    auto req = Requirement::parse(req_str);
    if (!req) {
        return std::unexpected("Некорректная строка требования зависимости: " + req_str + " (" + req.error() + ")");
    }
    dependencies.append(*req);
    return {};
}

Package& Package::add_conflict(const Requirement& req) {
    conflicts.append(req);
    return *this;
}

std::expected<void, std::string> Package::add_conflict(const std::string& req_str) {
    auto req = Requirement::parse(req_str);
    if (!req) {
        return std::unexpected("Некорректная строка требования конфликта: " + req_str + " (" + req.error() + ")");
    }
    conflicts.append(*req);
    return {};
}

Package& Package::add_replaces(const std::string& package_name) {
    replaces.append(package_name);
    return *this;
}

Package& Package::add_file(const PackageFile& file) {
    files.append(file);
    return *this;
}

Package& Package::add_file(const std::string& path, const std::string& content, bool executable) {
    return add_file(PackageFile(path, content, executable));
}

Package& Package::add_pre_install(const std::string& command) {
    pre_install.append(command);
    return *this;
}

Package& Package::add_post_install(const std::string& command) {
    post_install.append(command);
    return *this;
}

std::expected<void, std::string> Package::validate_package_name(const std::string& name) {
    if (name.empty()) {
        return std::unexpected("Имя пакета не может быть пустым");
    }

    if (!std::isalnum(static_cast<unsigned char>(name[0]))) {
        return std::unexpected("Имя пакета должно начинаться с буквы или цифры: " + name);
    }

    for (char ch : name) {
        if (!std::isalnum(static_cast<unsigned char>(ch)) && ch != '-' && ch != '_' && ch != '.') {
            return std::unexpected(std::string("Недопустимый символ в имени пакета: '") + ch + "' в \"" + name + "\"");
        }
    }

    return {};
}

std::expected<void, std::string> Package::validate_file_path(const std::string& path) {
    if (path.empty()) {
        return std::unexpected("Путь к файлу не может быть пустым");
    }

    if (path[0] == '/') {
        return std::unexpected("Абсолютные пути запрещены: " + path);
    }

    if (path == "." || path == "..") {
        return std::unexpected("Некорректный путь файла: " + path);
    }

    if (path.starts_with("../") || path.ends_with("/..") || path.find("/../") != std::string::npos) {
        return std::unexpected("Выход за пределы песочницы через '..' запрещён: " + path);
    }

    return {};
}

std::expected<void, std::string> Package::validate() const {
    auto name_res = validate_package_name(name);
    if (!name_res) return name_res;

    for (int i = 0; i < files.get_length(); ++i) {
        auto path_res = validate_file_path(files[i].path);
        if (!path_res) return path_res;

        for (int j = i + 1; j < files.get_length(); ++j) {
            if (files[i].path == files[j].path) {
                return std::unexpected("Дублирующийся путь файла внутри пакета: " + files[i].path);
            }
        }
    }

    return {};
}

std::string Package::to_string() const {
    return name + "@" + version.to_string();
}

bool Package::operator==(const Package& other) const {
    return name == other.name && version == other.version;
}

} // namespace pm
