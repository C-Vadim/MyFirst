#include <iostream>
#include <sqlite3.h>
#include <sys/stat.h>

bool databaseExists(const std::string& dbName) {
    struct stat buffer;
    return (stat(dbName.c_str(), &buffer) == 0); // �������� ������������� �����
}

int createDatabase() {
    sqlite3* DB;
    char* messageError;

    // �������� ������� ���� ������
    if (databaseExists("example.db")) {
        std::cout << "���� ������ ��� ����������." << std::endl;
        return SQLITE_OK; // ���� ������ ��� ����������
    }

    // �������� ���� ������ ��� ��������, ���� ��� �� ����������
    int exit = sqlite3_open("example.db", &DB);

    // SQL-������ ��� �������� ������� �������������
    std::string sql_users = "CREATE TABLE IF NOT EXISTS users("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "name TEXT NOT NULL UNIQUE);";

    // SQL-������ ��� �������� ������� �������� � ������� ������ �� �������������
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

    // ���������� �������� ������
    exit = sqlite3_exec(DB, sql_users.c_str(), NULL, 0, &messageError);
    if (exit != SQLITE_OK) {
        std::cerr << "������ ��� �������� ������� users: " << messageError << std::endl;
        sqlite3_free(messageError);
        return exit;
    }
    else {
        std::cout << "������� users ������� �������." << std::endl;
    }

    exit = sqlite3_exec(DB, sql_license.c_str(), NULL, 0, &messageError);
    if (exit != SQLITE_OK) {
        std::cerr << "������ ��� �������� ������� licenses: " << messageError << std::endl;
        sqlite3_free(messageError);
        return exit;
    }
    else {
        std::cout << "������� licenses ������� �������." << std::endl;
    }

    sqlite3_close(DB); // ������� ���� ������ ����� �������� ������
    return exit;
}

int insertUsersAndLicenses() {
    sqlite3* DB;
    char* messageError;

    // �������� ���� ������
    int exit = sqlite3_open("example.db", &DB);

    // SQL-������� ��� ���������� �������������
    std::string sql_insert_admin = "INSERT INTO users (name) VALUES('admin');";
    std::string sql_insert_user1 = "INSERT INTO users (name) VALUES('user1');";
    std::string sql_insert_user2 = "INSERT INTO users (name) VALUES('user2');";
    std::string sql_insert_user3 = "INSERT INTO users (name) VALUES('user3');";

    // ���������� ������� ���������� �������������
    exit = sqlite3_exec(DB, sql_insert_admin.c_str(), NULL, 0, &messageError);
    if (exit != SQLITE_OK) {
        std::cerr << "������ ��� ���������� ��������������: " << messageError << std::endl;
        sqlite3_free(messageError);
        return exit;
    }
    else {
        std::cout << "������������ admin ��������." << std::endl;
    }

    exit = sqlite3_exec(DB, sql_insert_user1.c_str(), NULL, 0, &messageError);
    if (exit != SQLITE_OK) {
        std::cerr << "������ ��� ���������� user1: " << messageError << std::endl;
        sqlite3_free(messageError);
        return exit;
    }
    else {
        std::cout << "������������ user1 ��������." << std::endl;
    }

    exit = sqlite3_exec(DB, sql_insert_user2.c_str(), NULL, 0, &messageError);
    if (exit != SQLITE_OK) {
        std::cerr << "������ ��� ���������� user2: " << messageError << std::endl;
        sqlite3_free(messageError);
        return exit;
    }
    else {
        std::cout << "������������ user2 ��������." << std::endl;
    }

    exit = sqlite3_exec(DB, sql_insert_user3.c_str(), NULL, 0, &messageError);
    if (exit != SQLITE_OK) {
        std::cerr << "������ ��� ���������� user3: " << messageError << std::endl;
        sqlite3_free(messageError);
        return exit;
    }
    else {
        std::cout << "������������ user3 ��������." << std::endl;
    }

    // SQL-������� ��� ���������� �������� ��� �������������
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

    // ���������� �������� ���������� ��������
    exit = sqlite3_exec(DB, sql_insert_license_admin.c_str(), NULL, 0, &messageError);
    if (exit != SQLITE_OK) {
        std::cerr << "������ ��� ���������� �������� ��� admin: " << messageError << std::endl;
        sqlite3_free(messageError);
        return exit;
    }
    else {
        std::cout << "�������� ��� admin ���������." << std::endl;
    }

    exit = sqlite3_exec(DB, sql_insert_license_user1.c_str(), NULL, 0, &messageError);
    if (exit != SQLITE_OK) {
        std::cerr << "������ ��� ���������� �������� ��� user1: " << messageError << std::endl;
        sqlite3_free(messageError);
        return exit;
    }
    else {
        std::cout << "�������� ��� user1 ���������." << std::endl;
    }

    exit = sqlite3_exec(DB, sql_insert_license_user2.c_str(), NULL, 0, &messageError);
    if (exit != SQLITE_OK) {
        std::cerr << "������ ��� ���������� �������� ��� user2: " << messageError << std::endl;
        sqlite3_free(messageError);
        return exit;
    }
    else {
        std::cout << "�������� ��� user2 ���������." << std::endl;
    }

    exit = sqlite3_exec(DB, sql_insert_license_user3.c_str(), NULL, 0, &messageError);
    if (exit != SQLITE_OK) {
        std::cerr << "������ ��� ���������� �������� ��� user3: " << messageError << std::endl;
        sqlite3_free(messageError);
        return exit;
    }
    else {
        std::cout << "�������� ��� user3 ���������." << std::endl;
    }

    sqlite3_close(DB); // ������� ���� ������ ����� ���������� ������������� � ��������
    return exit;
}

/*int main() {
    // �������� ���� ������ � ������
    int exit = createDatabase();
    if (exit != SQLITE_OK) {
        std::cerr << "������ ��� �������� ���� ������." << std::endl;
        return exit;
    }

    // ���������� ������������� � ��������
    exit = insertUsersAndLicenses();
    if (exit != SQLITE_OK) {
        std::cerr << "������ ��� ���������� ������������� � ��������." << std::endl;
        return exit;
    }

    std::cout << "���� ������ ������� ������� � ����������������!" << std::endl;

    return 0;
}
*/