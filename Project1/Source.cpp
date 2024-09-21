#include <iostream>
#include <sqlite3.h>
#include <vector>
#include <string>
#include <fstream>

// ������� ��� ����������� � ���� ������
sqlite3* connectToDatabase() {
    sqlite3* DB;
    if (sqlite3_open("example.db", &DB)) {
        std::cerr << "������ ��� ����������� � ���� ������." << std::endl;
        return nullptr;
    }
    return DB;
}

// ������� ��� ��������� ������������� �� ���� ������
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

// ������� ��� ��������� �������� ������������
std::vector<bool> getUserLicense(sqlite3* DB, const std::string& username) {
    std::vector<bool> permissions(6, false); // 6 ����
    sqlite3_stmt* stmt;
    const char* query = "SELECT create_user, delete_user, edit_file, read_file, create_file, delete_file "
        "FROM licenses WHERE user_id = (SELECT id FROM users WHERE name = ?);";

    if (sqlite3_prepare_v2(DB, query, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            for (int i = 0; i < 6; ++i) {
                permissions[i] = sqlite3_column_int(stmt, i) != 0; // ��������� �����
            }
        }
    }
    sqlite3_finalize(stmt);
    return permissions;
}

// ������� ��� ���������� �������� � �������
void performFileActions(const std::string& username, const std::vector<bool>& permissions) {
    int choice;
    std::string filename;

    while (true) {
        std::cout << "\n�������� ��������:\n";
        if (permissions[4]) std::cout << "1. ������� ����� ����\n"; // create_file
        if (permissions[5]) std::cout << "2. ������� ����\n"; // delete_file
        if (permissions[2]) std::cout << "3. ������������� ����\n"; // edit_file
        if (permissions[3]) std::cout << "4. ������ ����\n"; // read_file
        std::cout << "5. �����\n";
        std::cout << "��� �����: ";
        std::cin >> choice;

        switch (choice) {
        case 1:
            if (!permissions[4]) {
                std::cout << "� ��� ��� ���� ��� �������� �����." << std::endl;
                continue;
            }
            std::cout << "������� �������� ������ ����� (� ����������� .bin): ";
            std::cin >> filename;
            {
                std::ofstream ofs(filename, std::ios::binary);
                if (ofs) {
                    std::cout << "���� " << filename << " ������." << std::endl;
                }
                else {
                    std::cerr << "������ ��� �������� �����." << std::endl;
                }
            }
            break;

        case 2:
            if (!permissions[5]) {
                std::cout << "� ��� ��� ���� ��� �������� �����." << std::endl;
                continue;
            }
            std::cout << "������� �������� ����� ��� �������� (� ����������� .bin): ";
            std::cin >> filename;
            if (remove(filename.c_str()) == 0) {
                std::cout << "���� " << filename << " ������." << std::endl;
            }
            else {
                std::cerr << "������ ��� �������� �����." << std::endl;
            }
            break;

        case 3:
            if (!permissions[2]) {
                std::cout << "� ��� ��� ���� ��� �������������� �����." << std::endl;
                continue;
            }
            std::cout << "������� �������� ����� ��� �������������� (� ����������� .bin): ";
            std::cin >> filename;
            {
                std::ofstream ofs(filename, std::ios::binary | std::ios::app);
                if (ofs) {
                    std::string content;
                    std::cin.ignore(); // ���������� ���������� ������ ����� ������
                    std::cout << "������� ����� ��� ���������� � ����: ";
                    std::getline(std::cin, content); // ������ ������ � ���������
                    ofs.write(content.c_str(), content.size());
                    std::cout << "���� " << filename << " ��������." << std::endl;
                }
                else {
                    std::cerr << "������ ��� �������� �����." << std::endl;
                }
            }
            break;

        case 4:
            if (!permissions[3]) {
                std::cout << "� ��� ��� ���� ��� ������ �����." << std::endl;
                continue;
            }
            std::cout << "������� �������� ����� ��� ������ (� ����������� .bin): ";
            std::cin >> filename;
            {
                std::ifstream ifs(filename, std::ios::binary);
                if (ifs) {
                    std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
                    std::cout << "���������� ����� " << filename << ":\n" << content << std::endl;
                }
                else {
                    std::cerr << "������ ��� ������ �����." << std::endl;
                }
            }
            break;

        case 5:
            return; // ����� �� �������

        default:
            std::cout << "�������� �����. ���������� ��� ���." << std::endl;
            break;
        }
    }
}

int main() {
    setlocale(LC_ALL, "Ru");
    sqlite3* DB = connectToDatabase();
    if (!DB) {
        return 1; // ������ ����������� � ���� ������
    }

    // ��������� ������������� �� ���� ������
    std::vector<std::string> users = getUsers(DB);
    if (users.empty()) {
        std::cout << "��� ��������� �������������." << std::endl;
        sqlite3_close(DB);
        return 1;
    }

    // ����� ������ �������������
    std::cout << "��������� ������������:\n";
    for (size_t i = 0; i < users.size(); ++i) {
        std::cout << i + 1 << ". " << users[i] << std::endl;
    }

    // ����� ������������
    int userChoice;
    std::cout << "������� ����� ������������ ��� �����: ";
    std::cin >> userChoice;

    if (userChoice < 1 || userChoice > users.size()) {
        std::cerr << "�������� ����� ������������." << std::endl;
        sqlite3_close(DB);
        return 1;
    }

    std::string selectedUser = users[userChoice - 1];
    std::cout << "�� ����� ��� " << selectedUser << std::endl;

    // ��������� �������� ������������
    std::vector<bool> permissions = getUserLicense(DB, selectedUser);

    // ���������� �������� � ������� � ������ ��������
    performFileActions(selectedUser, permissions);

    sqlite3_close(DB); // �������� ���� ������
    return 0;
}
