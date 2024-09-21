#include <iostream>
#include <sqlite3.h>
#include <sys/stat.h>

bool databaseExists(const std::string& dbName) {
    struct stat buffer;
    return (stat(dbName.c_str(), &buffer) == 0); // Проверка существования файла
}

int createDatabase() {
    sqlite3* DB;
    char* messageError;

    // Проверка наличия базы данных
    if (databaseExists("example.db")) {
        std::cout << "База данных уже существует." << std::endl;
        return SQLITE_OK; // База данных уже существует
    }

    // Открытие базы данных или создание, если она не существует
    int exit = sqlite3_open("example.db", &DB);

    // SQL-запрос для создания таблицы пользователей
    std::string sql_users = "CREATE TABLE IF NOT EXISTS users("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "name TEXT NOT NULL UNIQUE);";

    // SQL-запрос для создания таблицы лицензий с внешним ключом на пользователей
    std::string sql_license = "CREATE TABLE IF NOT EXISTS licenses("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "user_id INTEGER NOT NULL, "
        "create_user BOOLEAN, "
        "delete_user BOOLEAN, "
        "edit_file BOOLEAN, "
        "read_file BOOLEAN, "
        "create_file BOOLEAN, "
        "delete_file BOOLEAN, "
        "FOREIGN KEY(user_id) REFERENCES users(id));";

    // Выполнение создания таблиц
    exit = sqlite3_exec(DB, sql_users.c_str(), NULL, 0, &messageError);
    if (exit != SQLITE_OK) {
        std::cerr << "Ошибка при создании таблицы users: " << messageError << std::endl;
        sqlite3_free(messageError);
        return exit;
    }
    else {
        std::cout << "Таблица users создана успешно." << std::endl;
    }

    exit = sqlite3_exec(DB, sql_license.c_str(), NULL, 0, &messageError);
    if (exit != SQLITE_OK) {
        std::cerr << "Ошибка при создании таблицы licenses: " << messageError << std::endl;
        sqlite3_free(messageError);
        return exit;
    }
    else {
        std::cout << "Таблица licenses создана успешно." << std::endl;
    }

    sqlite3_close(DB); // Закрыть базу данных после создания таблиц
    return exit;
}

int insertUsersAndLicenses() {
    sqlite3* DB;
    char* messageError;

    // Открытие базы данных
    int exit = sqlite3_open("example.db", &DB);

    // SQL-запросы для добавления пользователей
    std::string sql_insert_admin = "INSERT INTO users (name) VALUES('admin');";
    std::string sql_insert_user1 = "INSERT INTO users (name) VALUES('user1');";
    std::string sql_insert_user2 = "INSERT INTO users (name) VALUES('user2');";
    std::string sql_insert_user3 = "INSERT INTO users (name) VALUES('user3');";

    // Выполнение запроса добавления пользователей
    exit = sqlite3_exec(DB, sql_insert_admin.c_str(), NULL, 0, &messageError);
    if (exit != SQLITE_OK) {
        std::cerr << "Ошибка при добавлении администратора: " << messageError << std::endl;
        sqlite3_free(messageError);
        return exit;
    }
    else {
        std::cout << "Пользователь admin добавлен." << std::endl;
    }

    exit = sqlite3_exec(DB, sql_insert_user1.c_str(), NULL, 0, &messageError);
    if (exit != SQLITE_OK) {
        std::cerr << "Ошибка при добавлении user1: " << messageError << std::endl;
        sqlite3_free(messageError);
        return exit;
    }
    else {
        std::cout << "Пользователь user1 добавлен." << std::endl;
    }

    exit = sqlite3_exec(DB, sql_insert_user2.c_str(), NULL, 0, &messageError);
    if (exit != SQLITE_OK) {
        std::cerr << "Ошибка при добавлении user2: " << messageError << std::endl;
        sqlite3_free(messageError);
        return exit;
    }
    else {
        std::cout << "Пользователь user2 добавлен." << std::endl;
    }

    exit = sqlite3_exec(DB, sql_insert_user3.c_str(), NULL, 0, &messageError);
    if (exit != SQLITE_OK) {
        std::cerr << "Ошибка при добавлении user3: " << messageError << std::endl;
        sqlite3_free(messageError);
        return exit;
    }
    else {
        std::cout << "Пользователь user3 добавлен." << std::endl;
    }

    // SQL-запросы для добавления лицензий для пользователей
    std::string sql_insert_license_admin = "INSERT INTO licenses (user_id, create_user, delete_user, "
        "edit_file, read_file, create_file, delete_file) "
        "VALUES ((SELECT id FROM users WHERE name = 'admin'), true, true, true, true, true, true);";

    std::string sql_insert_license_user1 = "INSERT INTO licenses (user_id, create_user, delete_user, "
        "edit_file, read_file, create_file, delete_file) "
        "VALUES ((SELECT id FROM users WHERE name = 'user1'), true, true, true, true, false, false);";

    std::string sql_insert_license_user2 = "INSERT INTO licenses (user_id, create_user, delete_user, "
        "edit_file, read_file, create_file, delete_file) "
        "VALUES ((SELECT id FROM users WHERE name = 'user2'), false, false, true, true, false, false);";

    std::string sql_insert_license_user3 = "INSERT INTO licenses (user_id, create_user, delete_user, "
        "edit_file, read_file, create_file, delete_file) "
        "VALUES ((SELECT id FROM users WHERE name = 'user3'), false, false, false, true, false, false);";

    // Выполнение запросов добавления лицензий
    exit = sqlite3_exec(DB, sql_insert_license_admin.c_str(), NULL, 0, &messageError);
    if (exit != SQLITE_OK) {
        std::cerr << "Ошибка при добавлении лицензии для admin: " << messageError << std::endl;
        sqlite3_free(messageError);
        return exit;
    }
    else {
        std::cout << "Лицензия для admin добавлена." << std::endl;
    }

    exit = sqlite3_exec(DB, sql_insert_license_user1.c_str(), NULL, 0, &messageError);
    if (exit != SQLITE_OK) {
        std::cerr << "Ошибка при добавлении лицензии для user1: " << messageError << std::endl;
        sqlite3_free(messageError);
        return exit;
    }
    else {
        std::cout << "Лицензия для user1 добавлена." << std::endl;
    }

    exit = sqlite3_exec(DB, sql_insert_license_user2.c_str(), NULL, 0, &messageError);
    if (exit != SQLITE_OK) {
        std::cerr << "Ошибка при добавлении лицензии для user2: " << messageError << std::endl;
        sqlite3_free(messageError);
        return exit;
    }
    else {
        std::cout << "Лицензия для user2 добавлена." << std::endl;
    }

    exit = sqlite3_exec(DB, sql_insert_license_user3.c_str(), NULL, 0, &messageError);
    if (exit != SQLITE_OK) {
        std::cerr << "Ошибка при добавлении лицензии для user3: " << messageError << std::endl;
        sqlite3_free(messageError);
        return exit;
    }
    else {
        std::cout << "Лицензия для user3 добавлена." << std::endl;
    }

    sqlite3_close(DB); // Закрыть базу данных после добавления пользователей и лицензий
    return exit;
}

/*int main() {
    // Создание базы данных и таблиц
    int exit = createDatabase();
    if (exit != SQLITE_OK) {
        std::cerr << "Ошибка при создании базы данных." << std::endl;
        return exit;
    }

    // Добавление пользователей и лицензий
    exit = insertUsersAndLicenses();
    if (exit != SQLITE_OK) {
        std::cerr << "Ошибка при добавлении пользователей и лицензий." << std::endl;
        return exit;
    }

    std::cout << "База данных успешно создана и инициализирована!" << std::endl;

    return 0;
}
*/