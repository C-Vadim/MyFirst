#include <iostream>
#include <sqlite3.h>
#include <vector>
#include <string>
#include <fstream>

// Функция для подключения к базе данных
sqlite3* connectToDatabase() {
    sqlite3* DB;
    if (sqlite3_open("example.db", &DB)) {
        std::cerr << "Ошибка при подключении к базе данных." << std::endl;
        return nullptr;
    }
    return DB;
}

// Функция для получения пользователей из базы данных
std::vector<std::string> getUsers(sqlite3* DB) {
    std::vector<std::string> users;
    sqlite3_stmt* stmt;
    const char* query = "SELECT name FROM users;";

    if (sqlite3_prepare_v2(DB, query, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            users.push_back(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
        }
    }
    sqlite3_finalize(stmt);
    return users;
}

// Функция для получения лицензии пользователя
std::vector<bool> getUserLicense(sqlite3* DB, const std::string& username) {
    std::vector<bool> permissions(6, false); // 6 прав
    sqlite3_stmt* stmt;
    const char* query = "SELECT create_user, delete_user, edit_file, read_file, create_file, delete_file "
        "FROM licenses WHERE user_id = (SELECT id FROM users WHERE name = ?);";

    if (sqlite3_prepare_v2(DB, query, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            for (int i = 0; i < 6; ++i) {
                permissions[i] = sqlite3_column_int(stmt, i) != 0; // Считываем права
            }
        }
    }
    sqlite3_finalize(stmt);
    return permissions;
}

// Функция для выполнения действий с файлами
void performFileActions(const std::string& username, const std::vector<bool>& permissions) {
    int choice;
    std::string filename;

    while (true) {
        std::cout << "\nВыберите действие:\n";
        if (permissions[4]) std::cout << "1. Создать новый файл\n"; // create_file
        if (permissions[5]) std::cout << "2. Удалить файл\n"; // delete_file
        if (permissions[2]) std::cout << "3. Редактировать файл\n"; // edit_file
        if (permissions[3]) std::cout << "4. Читать файл\n"; // read_file
        std::cout << "5. Выход\n";
        std::cout << "Ваш выбор: ";
        std::cin >> choice;

        switch (choice) {
        case 1:
            if (!permissions[4]) {
                std::cout << "У вас нет прав для создания файла." << std::endl;
                continue;
            }
            std::cout << "Введите название нового файла (с расширением .bin): ";
            std::cin >> filename;
            {
                std::ofstream ofs(filename, std::ios::binary);
                if (ofs) {
                    std::cout << "Файл " << filename << " создан." << std::endl;
                }
                else {
                    std::cerr << "Ошибка при создании файла." << std::endl;
                }
            }
            break;

        case 2:
            if (!permissions[5]) {
                std::cout << "У вас нет прав для удаления файла." << std::endl;
                continue;
            }
            std::cout << "Введите название файла для удаления (с расширением .bin): ";
            std::cin >> filename;
            if (remove(filename.c_str()) == 0) {
                std::cout << "Файл " << filename << " удален." << std::endl;
            }
            else {
                std::cerr << "Ошибка при удалении файла." << std::endl;
            }
            break;

        case 3:
            if (!permissions[2]) {
                std::cout << "У вас нет прав для редактирования файла." << std::endl;
                continue;
            }
            std::cout << "Введите название файла для редактирования (с расширением .bin): ";
            std::cin >> filename;
            {
                std::ofstream ofs(filename, std::ios::binary | std::ios::app);
                if (ofs) {
                    std::string content;
                    std::cin.ignore(); // Игнорируем оставшийся символ новой строки
                    std::cout << "Введите текст для добавления в файл: ";
                    std::getline(std::cin, content); // Читаем строку с пробелами
                    ofs.write(content.c_str(), content.size());
                    std::cout << "Файл " << filename << " обновлен." << std::endl;
                }
                else {
                    std::cerr << "Ошибка при открытии файла." << std::endl;
                }
            }
            break;

        case 4:
            if (!permissions[3]) {
                std::cout << "У вас нет прав для чтения файла." << std::endl;
                continue;
            }
            std::cout << "Введите название файла для чтения (с расширением .bin): ";
            std::cin >> filename;
            {
                std::ifstream ifs(filename, std::ios::binary);
                if (ifs) {
                    std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
                    std::cout << "Содержимое файла " << filename << ":\n" << content << std::endl;
                }
                else {
                    std::cerr << "Ошибка при чтении файла." << std::endl;
                }
            }
            break;

        case 5:
            return; // Выход из функции

        default:
            std::cout << "Неверный выбор. Попробуйте еще раз." << std::endl;
            break;
        }
    }
}

int main() {
    setlocale(LC_ALL, "Ru");
    sqlite3* DB = connectToDatabase();
    if (!DB) {
        return 1; // Ошибка подключения к базе данных
    }

    // Получение пользователей из базы данных
    std::vector<std::string> users = getUsers(DB);
    if (users.empty()) {
        std::cout << "Нет доступных пользователей." << std::endl;
        sqlite3_close(DB);
        return 1;
    }

    // Вывод списка пользователей
    std::cout << "Доступные пользователи:\n";
    for (size_t i = 0; i < users.size(); ++i) {
        std::cout << i + 1 << ". " << users[i] << std::endl;
    }

    // Выбор пользователя
    int userChoice;
    std::cout << "Введите номер пользователя для входа: ";
    std::cin >> userChoice;

    if (userChoice < 1 || userChoice > users.size()) {
        std::cerr << "Неверный выбор пользователя." << std::endl;
        sqlite3_close(DB);
        return 1;
    }

    std::string selectedUser = users[userChoice - 1];
    std::cout << "Вы вошли как " << selectedUser << std::endl;

    // Получение лицензии пользователя
    std::vector<bool> permissions = getUserLicense(DB, selectedUser);

    // Выполнение действий с файлами с учетом лицензий
    performFileActions(selectedUser, permissions);

    sqlite3_close(DB); // Закрытие базы данных
    return 0;
}
