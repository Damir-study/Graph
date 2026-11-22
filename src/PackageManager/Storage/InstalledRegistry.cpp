#include "../../../include/PackageManager/Storage/InstalledRegistry.hpp"

#include <sstream>

namespace pm {

InstalledRegistry::InstalledRegistry()
    : installed_packages(new BinaryTreeDictionaryStorage<std::string, Package>(
          new BalancedTree<DictionaryPair<std::string, Package>>())),
      file_owners(new BinaryTreeDictionaryStorage<std::string, std::string>(
          new BalancedTree<DictionaryPair<std::string, std::string>>())),
      installed_names() {}

bool InstalledRegistry::is_installed(const std::string& name) const {
    return installed_packages.contains_key(name);
}

std::expected<Package, std::string> InstalledRegistry::get_installed(const std::string& name) const {
    if (!installed_packages.contains_key(name)) {
        return std::unexpected("Пакет '" + name + "' не установлен");
    }
    return installed_packages.get(name);
}

std::expected<void, std::string> InstalledRegistry::record_install(const Package& pkg) {
    auto val_res = pkg.validate();
    if (!val_res) return val_res;

    // Проверяем, не установлен ли уже точно такой же пакет той же версии
    if (installed_packages.contains_key(pkg.get_name())) {
        if (installed_packages.get(pkg.get_name()).get_version() == pkg.get_version()) {
            return std::unexpected("Пакет " + pkg.to_string() + " уже установлен");
        }
    }

    const auto& files = pkg.get_files();
    const auto& replaces = pkg.get_replaces();

    // Проверка коллизий файлов
    for (int i = 0; i < files.get_length(); ++i) {
        const std::string& file_path = files[i].path;
        if (file_owners.contains_key(file_path)) {
            const std::string& current_owner = file_owners.get(file_path);
            bool allowed = false;

            // Разрешено, если текущий владелец — этот же пакет (обновление версии)
            if (current_owner == pkg.get_name()) {
                allowed = true;
            } else {
                // Или если новый пакет явно заменяет текущего владельца
                for (int r = 0; r < replaces.get_length(); ++r) {
                    if (replaces[r] == current_owner) {
                        allowed = true;
                        break;
                    }
                }
            }

            if (!allowed) {
                return std::unexpected("Конфликт файлов: путь '" + file_path + "' уже занят установленным пакетом '" + current_owner + "'");
            }
        }
    }

    // Если обновляется уже установленный пакет, удаляем его старые файлы из file_owners
    if (installed_packages.contains_key(pkg.get_name())) {
        const Package& old_pkg = installed_packages.get(pkg.get_name());
        const auto& old_files = old_pkg.get_files();
        for (int i = 0; i < old_files.get_length(); ++i) {
            if (file_owners.contains_key(old_files[i].path)) {
                file_owners.remove(old_files[i].path);
            }
        }
    }

    // Если заменяются другие пакеты (replaces), удаляем их файлы и записи
    for (int r = 0; r < replaces.get_length(); ++r) {
        const std::string& replaced_name = replaces[r];
        if (installed_packages.contains_key(replaced_name)) {
            const Package& rep_pkg = installed_packages.get(replaced_name);
            const auto& rep_files = rep_pkg.get_files();
            for (int i = 0; i < rep_files.get_length(); ++i) {
                if (file_owners.contains_key(rep_files[i].path)) {
                    file_owners.remove(rep_files[i].path);
                }
            }
            installed_packages.remove(replaced_name);

            // Удаляем из installed_names
            mutable_array_sequence<std::string> new_names;
            for (int i = 0; i < installed_names.get_length(); ++i) {
                if (installed_names[i] != replaced_name) {
                    new_names.append(installed_names[i]);
                }
            }
            installed_names = new_names;
        }
    }

    // Регистрируем новые файлы
    for (int i = 0; i < files.get_length(); ++i) {
        if (file_owners.contains_key(files[i].path)) {
            file_owners.get(files[i].path) = pkg.get_name();
        } else {
            file_owners.add(files[i].path, pkg.get_name());
        }
    }

    // Добавляем или обновляем запись о пакете
    if (installed_packages.contains_key(pkg.get_name())) {
        installed_packages.get(pkg.get_name()) = pkg;
    } else {
        installed_packages.add(pkg.get_name(), pkg);
        installed_names.append(pkg.get_name());
    }

    return {};
}

std::expected<void, std::string> InstalledRegistry::record_remove(const std::string& name) {
    if (!installed_packages.contains_key(name)) {
        return std::unexpected("Пакет '" + name + "' не установлен в системе");
    }

    const Package& pkg = installed_packages.get(name);
    const auto& files = pkg.get_files();
    for (int i = 0; i < files.get_length(); ++i) {
        if (file_owners.contains_key(files[i].path)) {
            file_owners.remove(files[i].path);
        }
    }

    installed_packages.remove(name);

    mutable_array_sequence<std::string> new_names;
    for (int i = 0; i < installed_names.get_length(); ++i) {
        if (installed_names[i] != name) {
            new_names.append(installed_names[i]);
        }
    }
    installed_names = new_names;

    return {};
}

bool InstalledRegistry::has_file_owner(const std::string& relative_path) const {
    return file_owners.contains_key(relative_path);
}

std::expected<std::string, std::string> InstalledRegistry::get_file_owner(const std::string& relative_path) const {
    if (!file_owners.contains_key(relative_path)) {
        return std::unexpected("Файл '" + relative_path + "' не принадлежит ни одному установленному пакету");
    }
    return file_owners.get(relative_path);
}

const mutable_array_sequence<std::string>& InstalledRegistry::get_installed_names() const {
    return installed_names;
}

std::size_t InstalledRegistry::get_count() const {
    return static_cast<std::size_t>(installed_names.get_length());
}

void InstalledRegistry::clear() {
    installed_packages.clear();
    file_owners.clear();
    installed_names = mutable_array_sequence<std::string>();
}

std::string InstalledRegistry::serialize() const {
    std::ostringstream oss;
    oss << installed_names.get_length() << "\n";
    for (int i = 0; i < installed_names.get_length(); ++i) {
        const Package& p = installed_packages.get(installed_names[i]);
        oss << p.get_name() << "\n";
        oss << p.get_version().to_string() << "\n";
        oss << p.get_description() << "\n";

        const auto& deps = p.get_dependencies();
        oss << deps.get_length() << "\n";
        for (int d = 0; d < deps.get_length(); ++d) {
            oss << deps[d].to_string() << "\n";
        }

        const auto& confs = p.get_conflicts();
        oss << confs.get_length() << "\n";
        for (int c = 0; c < confs.get_length(); ++c) {
            oss << confs[c].to_string() << "\n";
        }

        const auto& reps = p.get_replaces();
        oss << reps.get_length() << "\n";
        for (int r = 0; r < reps.get_length(); ++r) {
            oss << reps[r] << "\n";
        }

        const auto& files = p.get_files();
        oss << files.get_length() << "\n";
        for (int f = 0; f < files.get_length(); ++f) {
            oss << files[f].path << "\n";
            oss << (files[f].executable ? 1 : 0) << "\n";
            oss << files[f].content.size() << "\n";
            oss << files[f].content << "\n";
        }

        const auto& pre = p.get_pre_install();
        oss << pre.get_length() << "\n";
        for (int a = 0; a < pre.get_length(); ++a) {
            oss << pre[a] << "\n";
        }

        const auto& post = p.get_post_install();
        oss << post.get_length() << "\n";
        for (int a = 0; a < post.get_length(); ++a) {
            oss << post[a] << "\n";
        }
    }
    return oss.str();
}

std::expected<InstalledRegistry, std::string> InstalledRegistry::deserialize(const std::string& data) {
    if (data.empty()) {
        return InstalledRegistry();
    }

    std::istringstream iss(data);
    int count = 0;
    if (!(iss >> count)) {
        return std::unexpected("Некорректный заголовок состояния реестра");
    }

    std::string dummy;
    std::getline(iss, dummy); // skip newline

    InstalledRegistry registry;

    for (int i = 0; i < count; ++i) {
        std::string name;
        if (!std::getline(iss, name)) return std::unexpected("Ошибка чтения имени пакета");
        std::string ver_str;
        if (!std::getline(iss, ver_str)) return std::unexpected("Ошибка чтения версии пакета");
        auto ver_res = Version::parse(ver_str);
        if (!ver_res) return std::unexpected(ver_res.error());

        std::string desc;
        if (!std::getline(iss, desc)) return std::unexpected("Ошибка чтения описания");

        Package p(name, *ver_res, desc);

        // dependencies
        int dep_count = 0;
        if (!(iss >> dep_count)) return std::unexpected("Ошибка чтения счетчика зависимостей");
        std::getline(iss, dummy);
        for (int d = 0; d < dep_count; ++d) {
            std::string req_str;
            if (!std::getline(iss, req_str)) return std::unexpected("Ошибка чтения зависимости");
            auto req_res = Requirement::parse(req_str);
            if (!req_res) return std::unexpected(req_res.error());
            p.add_dependency(*req_res);
        }

        // conflicts
        int conf_count = 0;
        if (!(iss >> conf_count)) return std::unexpected("Ошибка чтения счетчика конфликтов");
        std::getline(iss, dummy);
        for (int c = 0; c < conf_count; ++c) {
            std::string req_str;
            if (!std::getline(iss, req_str)) return std::unexpected("Ошибка чтения конфликта");
            auto req_res = Requirement::parse(req_str);
            if (!req_res) return std::unexpected(req_res.error());
            p.add_conflict(*req_res);
        }

        // replaces
        int rep_count = 0;
        if (!(iss >> rep_count)) return std::unexpected("Ошибка чтения счетчика замен");
        std::getline(iss, dummy);
        for (int r = 0; r < rep_count; ++r) {
            std::string rep_str;
            if (!std::getline(iss, rep_str)) return std::unexpected("Ошибка чтения замены");
            p.add_replaces(rep_str);
        }

        // files
        int file_count = 0;
        if (!(iss >> file_count)) return std::unexpected("Ошибка чтения счетчика файлов");
        std::getline(iss, dummy);
        for (int f = 0; f < file_count; ++f) {
            std::string fpath;
            if (!std::getline(iss, fpath)) return std::unexpected("Ошибка чтения пути файла");
            int is_exec = 0;
            if (!(iss >> is_exec)) return std::unexpected("Ошибка чтения флага исполняемости");
            std::size_t content_size = 0;
            if (!(iss >> content_size)) return std::unexpected("Ошибка чтения размера файла");
            std::getline(iss, dummy);

            std::string fcontent;
            fcontent.resize(content_size);
            iss.read(&fcontent[0], content_size);
            std::getline(iss, dummy); // trailing newline

            p.add_file(fpath, fcontent, is_exec != 0);
        }

        // pre_install
        int pre_count = 0;
        if (!(iss >> pre_count)) return std::unexpected("Ошибка чтения счетчика pre_install");
        std::getline(iss, dummy);
        for (int a = 0; a < pre_count; ++a) {
            std::string action;
            if (!std::getline(iss, action)) return std::unexpected("Ошибка чтения pre_install действия");
            p.add_pre_install(action);
        }

        // post_install
        int post_count = 0;
        if (!(iss >> post_count)) return std::unexpected("Ошибка чтения счетчика post_install");
        std::getline(iss, dummy);
        for (int a = 0; a < post_count; ++a) {
            std::string action;
            if (!std::getline(iss, action)) return std::unexpected("Ошибка чтения post_install действия");
            p.add_post_install(action);
        }

        auto rec_res = registry.record_install(p);
        if (!rec_res) return std::unexpected(rec_res.error());
    }

    return registry;
}

} // namespace pm
