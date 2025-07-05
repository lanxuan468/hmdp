#define _CRT_SECURE_NO_WARNINGS  // ����CRT�����İ�ȫ����
#include "pet_system.h"          // ��������ϵͳ�Զ���ͷ�ļ�
#include <ctime>                 // ʱ����غ���
#include <iomanip>               // �����ʽ��
#include <limits>                // ��ֵ����
#include <vector>                // ��������
#include <algorithm>             // �㷨��find_if
#include <sstream>               // �ַ���������

// ȫ�ֱ�������
std::vector<PetBoarding> boardingRecords;  // ��¼��¼
std::vector<PetGrooming> groomingRecords;  //���ݼ�¼
std::vector<PetVaccine> vaccineRecords;    // �����¼
Admin admin = { "lin", "123321" };         // Ĭ�Ϲ���Ա��Ϣ������ļ���û�ж����Ļ���
const std::string ADMIN_FILE = "admin.txt", BOARDING_FILE = "boarding.txt",  // ���ݴ洢�ļ���
GROOMING_FILE = "grooming.txt", VACCINE_FILE = "vaccine.txt";

// ���ߺ���
void clearScreen() {
#ifdef _WIN32
    system("cls");  // Windows��������
#endif
}

void pauseScreen() {
    // ��ʾ�û��������ȴ�����
    std::cout << "\n�����������...";
    std::cin.ignore();
    std::cin.get();
}

void cinClear() {
    // �����������־������ʣ������
    std::cin.clear();
    std::cin.ignore(1024, '\n');
}

// ��ȡ��ǰ�����ַ�������ʽ��YYYY-MM-DD��
std::string getCurrentDate() {
    time_t now = time(0);
    tm* ltm = localtime(&now);//��1970sתΪtmָ�����(1900��ƫ����)
    std::stringstream ss;
    ss << (1900 + ltm->tm_year) << "-"
        << std::setw(2) << std::setfill('0') << (1 + ltm->tm_mon) << "-"
        << std::setw(2) << std::setfill('0') << ltm->tm_mday;
    return ss.str();
}

// ��ȡδ�������ַ�������ʽ��YYYY-MM-DD��
std::string getFutureDate(int days) {
    time_t now = time(0) + days * 24 * 60 * 60;
    tm* ltm = localtime(&now);
    std::stringstream ss;//����������ʽ���ģ��������ַ���Ĭ����0
    ss << (1900 + ltm->tm_year) << "-"
        << std::setw(2) << std::setfill('0') << (1 + ltm->tm_mon) << "-"
        << std::setw(2) << std::setfill('0') << ltm->tm_mday;
    return ss.str();
}

// ��ȡ������¼����һ������ID
int getNextId(const std::vector<PetBoarding>& records) {
    if (records.empty()) {
        return 1;
    }
    int maxId = 0;
    for (const auto& record : records) {
        if (record.id > maxId) {
            maxId = record.id;
        }
    }
    return maxId + 1;
}

// ��ȡ���ݼ�¼����һ������ID
int getNextId(const std::vector<PetGrooming>& records) {
    if (records.empty()) {
        return 1;
    }
    int maxId = 0;
    for (const auto& record : records) {
        if (record.id > maxId) {
            maxId = record.id;
        }
    }
    return maxId + 1;
}

// ��ȡ�����¼����һ������ID
int getNextId(const std::vector<PetVaccine>& records) {
    if (records.empty()) {
        return 1;
    }
    int maxId = 0;
    for (const auto& record : records) {
        if (record.id > maxId) {
            maxId = record.id;
        }
    }
    return maxId + 1;
}

// ����Ա��¼����
bool adminLogin() {
    std::string u, p;
    for (int i = 0; i < 3; i++) {
        clearScreen();
        std::cout << "===== �������ϵͳ��¼ =====" << std::endl;
       std::cout << "�û���: "; std::cin >> u;
        std::cout << "����: "; std::cin >> p;
        if (u == admin.username && p == admin.password) {
             std::cout << "��¼�ɹ�����ӭʹ�ó������ϵͳ��" << std::endl;
            pauseScreen();
            return true;
        }
         std::cout << "�û�����������󣬻�ʣ" << 2 - i << "�γ��Ի��ᡣ" << std::endl;
        pauseScreen();
    }
    std::cout << "��¼���Դ������࣬ϵͳ�����˳�" << std::endl;
    return false;
}

// �������Ա���ݵ��ļ�
void saveAdminData() {
    std::ofstream f(ADMIN_FILE);
    if (f) f << admin.username << "," << admin.password;
}

// ���ļ����ع���Ա����
void loadAdminData() {
    std::ifstream f(ADMIN_FILE);
    if (f && std::getline(f, admin.username, ',') && std::getline(f, admin.password));
}

// ��ȡ��֤������������
int getIntInput(const std::string& prompt, int min, int max) {
    int val;
    while (true) {
        std::cout << prompt;
        if (std::cin >> val && val >= min && val <= max) {
            cinClear();
            return val;
        }
        std::cout << "������Ч�����������롣" << std::endl;
        cinClear();
    }
}

// ���������¼���ļ�
void saveBoardingData() {
    std::ofstream f(BOARDING_FILE);
    if (!f) { std::cout << "�޷����ļ���" << std::endl; return; }
    for (const auto& r : boardingRecords)
        f << r.id << "," << r.petName << "," << r.ownerName << "," << r.ownerPhone << ","
        << r.startDate << "," << r.endDate << "," << r.status << "," << r.price << "\n";
}

// ���ļ����ؼ�����¼
void loadBoardingData() {
    std::ifstream f(BOARDING_FILE);
    if (!f) return;
    boardingRecords.clear();
    std::string line;
    while (std::getline(f, line)) {
        std::stringstream ss(line);
        PetBoarding r;
        std::getline(ss, line, ','); r.id = std::stoi(line);
        std::getline(ss, r.petName, ',');
        std::getline(ss, r.ownerName, ',');
        std::getline(ss, r.ownerPhone, ',');
        std::getline(ss, r.startDate, ',');
        std::getline(ss, r.endDate, ',');
        std::getline(ss, r.status, ',');
        std::getline(ss, line); r.price = std::stod(line);
        boardingRecords.push_back(r);
    }
}

// ��Ӽ�����¼
void addBoardingRecord() {
    clearScreen();
    std::cout << "===== ��ӳ��������¼ =====" << std::endl;
    PetBoarding r;
    r.id = getNextId(boardingRecords);
    std::cout << "��������: "; std::getline(std::cin, r.petName);
    std::cout << "��������: "; std::getline(std::cin, r.ownerName);
    std::cout << "���˵绰: "; std::getline(std::cin, r.ownerPhone);
    r.startDate = getCurrentDate();
    std::cout << "��ʼ����: " << r.startDate << " (��ǰ)" << std::endl;
    r.endDate = getFutureDate(getIntInput("��������: ", 1, 365));
    std::cout << "��������: " << r.endDate << std::endl;
    r.status = "��ס";
    std::cout << "״̬: " << r.status << std::endl;
    r.price = getIntInput("�۸�: ", 0, 10000);
    boardingRecords.push_back(r);
    saveBoardingData();
    std::cout << "��ӳɹ���" << std::endl;
    pauseScreen();
}

// �鿴���м�����¼
void viewBoardingRecords() {
    clearScreen();
    std::cout << "===== �鿴���������¼ =====" << std::endl;
    if (boardingRecords.empty()) {
        std::cout << "û�м�¼��" << std::endl;
        pauseScreen();
        return;
    }
    std::cout << std::left << std::setw(5) << "ID" << std::setw(15) << "��������"
        << std::setw(15) << "��������" << std::setw(15) << "���˵绰"
        << std::setw(12) << "��ʼ����" << std::setw(12) << "��������"
        << std::setw(10) << "״̬" << std::setw(10) << "�۸�" << std::endl;
    std::cout << std::string(94, '-') << std::endl;
    for (const auto& r : boardingRecords)
        std::cout << std::left << std::setw(5) << r.id << std::setw(15) << r.petName
        << std::setw(15) << r.ownerName << std::setw(15) << r.ownerPhone
        << std::setw(12) << r.startDate << std::setw(12) << r.endDate
        << std::setw(10) << r.status << std::setw(10) << r.price << std::endl;
    pauseScreen();
}

// ���¼�����¼
void updateBoardingRecord() {
    clearScreen();
    std::cout << "===== ���³��������¼ =====" << std::endl;
    if (boardingRecords.empty()) {
        std::cout << "û�м�¼��" << std::endl;
        pauseScreen();
        return;
    }
    int id = getIntInput("������Ҫ���µļ�¼ID: ", 0, 9999);
    auto it = boardingRecords.begin();
    for (; it != boardingRecords.end(); ++it) {
        if (it->id == id) break;
    }
    if (it == boardingRecords.end()) {
        std::cout << "ID�����ڣ�" << std::endl;
        pauseScreen();
        return;
    }
    std::cout << "\n��ǰ��¼��Ϣ��\n"
        << "1. ��������: " << it->petName << "\n"
        << "2. ��������: " << it->ownerName << "\n"
        << "3. ���˵绰: " << it->ownerPhone << "\n"
        << "4. ��ʼ����: " << it->startDate << "\n"
        << "5. ��������: " << it->endDate << "\n"
        << "6. ״̬: " << it->status << "\n"
        << "7. �۸�: " << it->price << "\n"
        << "8. ����\nѡ��Ҫ���µ���Ŀ: ";
    int choice = getIntInput("", 1, 8);
    if (choice == 8) return;
    cinClear();
    switch (choice) {
    case 1: std::cout << "������: "; std::getline(std::cin, it->petName); break;
    case 2: std::cout << "������: "; std::getline(std::cin, it->ownerName); break;
    case 3: std::cout << "�µ绰: "; std::getline(std::cin, it->ownerPhone); break;
    case 4: std::cout << "������: "; std::getline(std::cin, it->startDate); break;
    case 5: std::cout << "������: "; std::getline(std::cin, it->endDate); break;
    case 6: std::cout << "��״̬: "; std::getline(std::cin, it->status); break;
    case 7: it->price = getIntInput("�¼۸�: ", 0, 10000); break;
    }
    saveBoardingData();
    std::cout << "���³ɹ���" << std::endl;
    pauseScreen();
}

// ɾ��������¼
void deleteBoardingRecord() {
    clearScreen();
    std::cout << "===== ɾ�����������¼ =====" << std::endl;
    if (boardingRecords.empty()) {
        std::cout << "û�м�¼��" << std::endl;
        pauseScreen();
        return;
    }
    int id = getIntInput("������Ҫɾ���ļ�¼ID: ", 0, 9999);
    auto it = std::find_if(boardingRecords.begin(), boardingRecords.end(),
        [id](const PetBoarding& r) { return r.id == id; });
    if (it == boardingRecords.end()) {
        std::cout << "ID�����ڣ�" << std::endl;
        pauseScreen();
        return;
    }
    std::cout << "\nȷ��ɾ�����¼�¼��\n"
        << "ID: " << it->id << "\n��������: " << it->petName
        << "\n��������: " << it->ownerName << "\nȷ��ɾ�� (Y/N): ";
    char c; std::cin >> c;
    if (c == 'Y' || c == 'y') {
        boardingRecords.erase(it);
        saveBoardingData();
        std::cout << "ɾ���ɹ���" << std::endl;
    }
    else {
        std::cout << "ȡ��ɾ��" << std::endl;
    }
    pauseScreen();
}

// ��������˵�
void boardingManagement() {
    while (true) {
        clearScreen();
        std::cout << "===== ����������� =====" << std::endl;
        std::cout << "1. ��Ӽ�����¼\n2. �鿴������¼\n3. ���¼�����¼"
            << "\n4. ɾ��������¼\n5. �������˵�\n��ѡ��: ";
        int choice = getIntInput("", 1, 5);
        switch (choice) {
        case 1: addBoardingRecord(); break;
        case 2: viewBoardingRecords(); break;
        case 3: updateBoardingRecord(); break;
        case 4: deleteBoardingRecord(); break;
        case 5: return;
        }
    }
}

// �������ݼ�¼���ļ�
void saveGroomingData() {
    std::ofstream f(GROOMING_FILE);
    if (!f) { std::cout << "�޷����ļ���" << std::endl; return; }
    for (const auto& r : groomingRecords)
        f << r.id << "," << r.petName << "," << r.ownerName << "," << r.ownerPhone << ","
        << r.serviceDate << "," << r.serviceType << "," << r.status << "," << r.price << "\n";
}

// ���ļ��������ݼ�¼
void loadGroomingData() {
    std::ifstream f(GROOMING_FILE);
    if (!f) return;
    groomingRecords.clear();
    std::string line;
    while (std::getline(f, line)) {
        std::stringstream ss(line);
        PetGrooming r;
        std::getline(ss, line, ','); r.id = std::stoi(line);
        std::getline(ss, r.petName, ',');
        std::getline(ss, r.ownerName, ',');
        std::getline(ss, r.ownerPhone, ',');
        std::getline(ss, r.serviceDate, ',');
        std::getline(ss, r.serviceType, ',');
        std::getline(ss, r.status, ',');
        std::getline(ss, line); r.price = std::stod(line);
        groomingRecords.push_back(r);
    }
}

// ����µ����ݼ�¼
void addGroomingRecord() {
    clearScreen();
    std::cout << "===== ��ӳ������ݼ�¼ =====" << std::endl;
    PetGrooming r;
    r.id = getNextId(groomingRecords);
    cinClear();
    std::cout << "��������: "; std::getline(std::cin, r.petName);
    std::cout << "��������: "; std::getline(std::cin, r.ownerName);
    std::cout << "���˵绰: "; std::getline(std::cin, r.ownerPhone);
    r.serviceDate = getCurrentDate();
    std::cout << "��������: " << r.serviceDate << " (��ǰ)" << std::endl;
    std::cout << "�������� (ϴ��/�޼�/ȫ��): "; std::getline(std::cin, r.serviceType);
    r.status = "δ���";
    std::cout << "״̬: " << r.status << std::endl;
    r.price = getIntInput("�۸�: ", 0, 10000);
    groomingRecords.push_back(r);
    saveGroomingData();
    std::cout << "��ӳɹ���" << std::endl;
    pauseScreen();
}

// �鿴�������ݼ�¼
void viewGroomingRecords() {
    clearScreen();
    std::cout << "===== �鿴�������ݼ�¼ =====" << std::endl;
    if (groomingRecords.empty()) {
        std::cout << "û�м�¼��" << std::endl;
        pauseScreen();
        return;
    }
    std::cout << std::left << std::setw(5) << "ID" << std::setw(15) << "��������"
        << std::setw(15) << "��������" << std::setw(15) << "���˵绰"
        << std::setw(12) << "��������" << std::setw(10) << "��������"
        << std::setw(10) << "״̬" << std::setw(10) << "�۸�" << std::endl;
    std::cout << std::string(92, '-') << std::endl;
    for (const auto& r : groomingRecords)
        std::cout << std::left << std::setw(5) << r.id << std::setw(15) << r.petName
        << std::setw(15) << r.ownerName << std::setw(15) << r.ownerPhone
        << std::setw(12) << r.serviceDate << std::setw(10) << r.serviceType
        << std::setw(10) << r.status << std::setw(10) << r.price << std::endl;
    pauseScreen();
}

// �������ݼ�¼
void updateGroomingRecord() {
    clearScreen();
    std::cout << "===== ���³������ݼ�¼ =====" << std::endl;
    if (groomingRecords.empty()) {
        std::cout << "û�м�¼��" << std::endl;
        pauseScreen();
        return;
    }
    int id = getIntInput("������Ҫ���µļ�¼ID: ", 0, 9999);
    auto it = std::find_if(groomingRecords.begin(), groomingRecords.end(),
        [id](const PetGrooming& r) { return r.id == id; });
    if (it == groomingRecords.end()) {
        std::cout << "ID�����ڣ�" << std::endl;
        pauseScreen();
        return;
    }
    std::cout << "\n��ǰ��¼��Ϣ��\n"
        << "1. ��������: " << it->petName << "\n"
        << "2. ��������: " << it->ownerName << "\n"
        << "3. ���˵绰: " << it->ownerPhone << "\n"
        << "4. ��������: " << it->serviceDate << "\n"
        << "5. ��������: " << it->serviceType << "\n"
        << "6. ״̬: " << it->status << "\n"
        << "7. �۸�: " << it->price << "\n"
        << "8. ����\nѡ��Ҫ���µ���Ŀ: ";
    int choice = getIntInput("", 1, 8);
    if (choice == 8) return;
    cinClear();
    switch (choice) {
    case 1: std::cout << "������: "; std::getline(std::cin, it->petName); break;
    case 2: std::cout << "������: "; std::getline(std::cin, it->ownerName); break;
    case 3: std::cout << "�µ绰: "; std::getline(std::cin, it->ownerPhone); break;
    case 4: std::cout << "������: "; std::getline(std::cin, it->serviceDate); break;
    case 5: std::cout << "������: "; std::getline(std::cin, it->serviceType); break;
    case 6: std::cout << "��״̬: "; std::getline(std::cin, it->status); break;
    case 7: it->price = getIntInput("�¼۸�: ", 0, 10000); break;
    }
    saveGroomingData();
    std::cout << "���³ɹ���" << std::endl;
    pauseScreen();
}

// ɾ�����ݼ�¼
void deleteGroomingRecord() {
    clearScreen();
    std::cout << "===== ɾ���������ݼ�¼ =====" << std::endl;
    if (groomingRecords.empty()) {
        std::cout << "û�м�¼��" << std::endl;
        pauseScreen();
        return;
    }
    int id = getIntInput("������Ҫɾ���ļ�¼ID: ", 0, 9999);
    auto it = std::find_if(groomingRecords.begin(), groomingRecords.end(),
        [id](const PetGrooming& r) { return r.id == id; });
    if (it == groomingRecords.end()) {
        std::cout << "ID�����ڣ�" << std::endl;
        pauseScreen();
        return;
    }
    std::cout << "\nȷ��ɾ�����¼�¼��\n"
        << "ID: " << it->id << "\n��������: " << it->petName
        << "\n��������: " << it->ownerName << "\nȷ��ɾ�� (Y/N): ";
    char c; std::cin >> c;
    if (c == 'Y' || c == 'y') {
        groomingRecords.erase(it);
        saveGroomingData();
        std::cout << "ɾ���ɹ���" << std::endl;
    }
    else {
        std::cout << "ȡ��ɾ��" << std::endl;
    }
    pauseScreen();
}

// ���ݹ���˵�
void groomingManagement() {
    while (true) {
        clearScreen();
        std::cout << "===== �������ݹ��� =====" << std::endl;
        std::cout << "1. ������ݼ�¼\n2. �鿴���ݼ�¼\n3. �������ݼ�¼"
            << "\n4. ɾ�����ݼ�¼\n5. �������˵�\n��ѡ��: ";
        int choice = getIntInput("", 1, 5);
        switch (choice) {
        case 1: addGroomingRecord(); break;
        case 2: viewGroomingRecords(); break;
        case 3: updateGroomingRecord(); break;
        case 4: deleteGroomingRecord(); break;
        case 5: return;
        }
    }
}

// ���������¼���ļ�
void saveVaccineData() {
    std::ofstream f(VACCINE_FILE);
    if (!f) { std::cout << "�޷����ļ���" << std::endl; return; }
    for (const auto& r : vaccineRecords)
        f << r.id << "," << r.petName << "," << r.ownerName << "," << r.ownerPhone << ","
        << r.vaccineType << "," << r.vaccineDate << "," << r.nextDueDate << ","
        << r.status << "," << r.price << "\n";
}

// ���ļ����������¼
void loadVaccineData() {
    std::ifstream f(VACCINE_FILE);
    if (!f) return;
    vaccineRecords.clear();
    std::string line;
    while (std::getline(f, line)) {
        std::stringstream ss(line);
        PetVaccine r;
        std::getline(ss, line, ','); r.id = std::stoi(line);
        std::getline(ss, r.petName, ',');
        std::getline(ss, r.ownerName, ',');
        std::getline(ss, r.ownerPhone, ',');
        std::getline(ss, r.vaccineType, ',');
        std::getline(ss, r.vaccineDate, ',');
        std::getline(ss, r.nextDueDate, ',');
        std::getline(ss, r.status, ',');
        std::getline(ss, line); r.price = std::stod(line);
        vaccineRecords.push_back(r);
    }
}

// ����µ������¼
void addVaccineRecord() {
    clearScreen();
    std::cout << "===== ��ӳ��������¼ =====" << std::endl;
    PetVaccine r;
    r.id = getNextId(vaccineRecords);
    cinClear();
    std::cout << "��������: "; std::getline(std::cin, r.petName);
    std::cout << "��������: "; std::getline(std::cin, r.ownerName);
    std::cout << "���˵绰: "; std::getline(std::cin, r.ownerPhone);
    std::cout << "��������: "; std::getline(std::cin, r.vaccineType);
    r.vaccineDate = getCurrentDate();
    std::cout << "��������: " << r.vaccineDate << " (��ǰ)" << std::endl;
    int days = getIntInput("�´�����������: ", 1, 365);
    r.nextDueDate = getFutureDate(days);
    std::cout << "�´���������: " << r.nextDueDate << std::endl;
    r.status = "�����";
    std::cout << "״̬: " << r.status << std::endl;
    r.price = getIntInput("�۸�: ", 0, 10000);
    vaccineRecords.push_back(r);
    saveVaccineData();
    std::cout << "��ӳɹ���" << std::endl;
    pauseScreen();
}

// �鿴���������¼
void viewVaccineRecords() {
    clearScreen();
    std::cout << "===== �鿴���������¼ =====" << std::endl;
    if (vaccineRecords.empty()) {
        std::cout << "û�м�¼��" << std::endl;
        pauseScreen();
        return;
    }
    std::cout << std::left << std::setw(5) << "ID" << std::setw(15) << "��������"
        << std::setw(15) << "��������" << std::setw(15) << "���˵绰"
        << std::setw(15) << "��������" << std::setw(12) << "��������"
        << std::setw(12) << "�´�����" << std::setw(10) << "״̬"
        << std::setw(10) << "�۸�" << std::endl;
    std::cout << std::string(109, '-') << std::endl;
    for (const auto& r : vaccineRecords)
        std::cout << std::left << std::setw(5) << r.id << std::setw(15) << r.petName
        << std::setw(15) << r.ownerName << std::setw(15) << r.ownerPhone
        << std::setw(15) << r.vaccineType << std::setw(12) << r.vaccineDate
        << std::setw(12) << r.nextDueDate << std::setw(10) << r.status
        << std::setw(10) << r.price << std::endl;
    pauseScreen();
}

// ���������¼
void updateVaccineRecord() {
    clearScreen();
    std::cout << "===== ���³��������¼ =====" << std::endl;
    if (vaccineRecords.empty()) {
        std::cout << "û�м�¼��" << std::endl;
        pauseScreen();
        return;
    }
    int id = getIntInput("������Ҫ���µļ�¼ID: ", 0, 9999);
    auto it = std::find_if(vaccineRecords.begin(), vaccineRecords.end(),
        [id](const PetVaccine& r) { return r.id == id; });
    if (it == vaccineRecords.end()) {
        std::cout << "ID�����ڣ�" << std::endl;
        pauseScreen();
        return;
    }
    std::cout << "\n��ǰ��¼��Ϣ��\n"
        << "1. ��������: " << it->petName << "\n"
        << "2. ��������: " << it->ownerName << "\n"
        << "3. ���˵绰: " << it->ownerPhone << "\n"
        << "4. ��������: " << it->vaccineType << "\n"
        << "5. ��������: " << it->vaccineDate << "\n"
        << "6. �´���������: " << it->nextDueDate << "\n"
        << "7. ״̬: " << it->status << "\n"
        << "8. �۸�: " << it->price << "\n"
        << "9. ����\nѡ��Ҫ���µ���Ŀ: ";
    int choice = getIntInput("", 1, 9);
    if (choice == 9) return;
    cinClear();
    switch (choice) {
    case 1: std::cout << "������: "; std::getline(std::cin, it->petName); break;
    case 2: std::cout << "������: "; std::getline(std::cin, it->ownerName); break;
    case 3: std::cout << "�µ绰: "; std::getline(std::cin, it->ownerPhone); break;
    case 4: std::cout << "������: "; std::getline(std::cin, it->vaccineType); break;
    case 5: std::cout << "������: "; std::getline(std::cin, it->vaccineDate); break;
    case 6: std::cout << "������: "; std::getline(std::cin, it->nextDueDate); break;
    case 7: std::cout << "��״̬: "; std::getline(std::cin, it->status); break;
    case 8: it->price = getIntInput("�¼۸�: ", 0, 10000); break;
    }
    saveVaccineData();
    std::cout << "���³ɹ���" << std::endl;
    pauseScreen();
}

// ɾ�������¼
void deleteVaccineRecord() {
    clearScreen();
    std::cout << "===== ɾ�����������¼ =====" << std::endl;
    if (vaccineRecords.empty()) {
        std::cout << "û�м�¼��" << std::endl;
        pauseScreen();
        return;
    }
    int id = getIntInput("������Ҫɾ���ļ�¼ID: ", 0, 9999);
    auto it = std::find_if(vaccineRecords.begin(), vaccineRecords.end(),
        [id](const PetVaccine& r) { return r.id == id; });
    if (it == vaccineRecords.end()) {
        std::cout << "ID�����ڣ�" << std::endl;
        pauseScreen();
        return;
    }
    std::cout << "\nȷ��ɾ�����¼�¼��\n"
        << "ID: " << it->id << "\n��������: " << it->petName
        << "\n��������: " << it->ownerName << "\nȷ��ɾ�� (Y/N): ";
    char c; std::cin >> c;
    if (c == 'Y' || c == 'y') {
        vaccineRecords.erase(it);
        saveVaccineData();
        std::cout << "ɾ���ɹ���" << std::endl;
    }
    else {
        std::cout << "ȡ��ɾ��" << std::endl;
    }
    pauseScreen();
}

// �������˵�
void vaccineManagement() {
    while (true) {
        clearScreen();
        std::cout << "===== ����������� =====" << std::endl;
        std::cout << "1. ��������¼\n2. �鿴�����¼\n3. ���������¼"
            << "\n4. ɾ�������¼\n5. �������˵�\n��ѡ��: ";
        int choice = getIntInput("", 1, 5);
        switch (choice) {
        case 1: addVaccineRecord(); break;
        case 2: viewVaccineRecords(); break;
        case 3: updateVaccineRecord(); break;
        case 4: deleteVaccineRecord(); break;
        case 5: return;
        }
    }
}