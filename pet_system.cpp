#define _CRT_SECURE_NO_WARNINGS  // 禁用CRT函数的安全警告
#include "pet_system.h"          // 包含宠物系统自定义头文件
#include <ctime>                 // 时间相关函数
#include <iomanip>               // 输出格式化
#include <limits>                // 数值限制
#include <vector>                // 向量容器
#include <algorithm>             // 算法如find_if
#include <sstream>               // 字符串流操作

// 全局变量定义
std::vector<PetBoarding> boardingRecords;  // 登录记录
std::vector<PetGrooming> groomingRecords;  //美容记录
std::vector<PetVaccine> vaccineRecords;    // 疫苗记录
Admin admin = { "lin", "123321" };         // 默认管理员信息（如果文件里没有东西的话）
const std::string ADMIN_FILE = "admin.txt", BOARDING_FILE = "boarding.txt",  // 数据存储文件名
GROOMING_FILE = "grooming.txt", VACCINE_FILE = "vaccine.txt";

// 工具函数
void clearScreen() {
#ifdef _WIN32
    system("cls");  // Windows清屏命令
#endif
}

void pauseScreen() {
    // 提示用户继续并等待输入
    std::cout << "\n按任意键继续...";
    std::cin.ignore();
    std::cin.get();
}

void cinClear() {
    // 清除输入错误标志并忽略剩余输入
    std::cin.clear();
    std::cin.ignore(1024, '\n');
}

// 获取当前日期字符串（格式：YYYY-MM-DD）
std::string getCurrentDate() {
    time_t now = time(0);
    tm* ltm = localtime(&now);//把1970s转为tm指针对象(1900的偏移量)
    std::stringstream ss;
    ss << (1900 + ltm->tm_year) << "-"
        << std::setw(2) << std::setfill('0') << (1 + ltm->tm_mon) << "-"
        << std::setw(2) << std::setfill('0') << ltm->tm_mday;
    return ss.str();
}

// 获取未来日期字符串（格式：YYYY-MM-DD）
std::string getFutureDate(int days) {
    time_t now = time(0) + days * 24 * 60 * 60;
    tm* ltm = localtime(&now);
    std::stringstream ss;//用来帮助格式化的，给两个字符，默认填0
    ss << (1900 + ltm->tm_year) << "-"
        << std::setw(2) << std::setfill('0') << (1 + ltm->tm_mon) << "-"
        << std::setw(2) << std::setfill('0') << ltm->tm_mday;
    return ss.str();
}

// 获取寄养记录的下一个可用ID
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

// 获取美容记录的下一个可用ID
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

// 获取疫苗记录的下一个可用ID
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

// 管理员登录功能
bool adminLogin() {
    std::string u, p;
    for (int i = 0; i < 3; i++) {
        clearScreen();
        std::cout << "===== 宠物管理系统登录 =====" << std::endl;
       std::cout << "用户名: "; std::cin >> u;
        std::cout << "密码: "; std::cin >> p;
        if (u == admin.username && p == admin.password) {
             std::cout << "登录成功，欢迎使用宠物管理系统！" << std::endl;
            pauseScreen();
            return true;
        }
         std::cout << "用户名或密码错误，还剩" << 2 - i << "次尝试机会。" << std::endl;
        pauseScreen();
    }
    std::cout << "登录尝试次数过多，系统即将退出" << std::endl;
    return false;
}

// 保存管理员数据到文件
void saveAdminData() {
    std::ofstream f(ADMIN_FILE);
    if (f) f << admin.username << "," << admin.password;
}

// 从文件加载管理员数据
void loadAdminData() {
    std::ifstream f(ADMIN_FILE);
    if (f && std::getline(f, admin.username, ',') && std::getline(f, admin.password));
}

// 获取验证过的整数输入
int getIntInput(const std::string& prompt, int min, int max) {
    int val;
    while (true) {
        std::cout << prompt;
        if (std::cin >> val && val >= min && val <= max) {
            cinClear();
            return val;
        }
        std::cout << "输入无效，请重新输入。" << std::endl;
        cinClear();
    }
}

// 保存寄养记录到文件
void saveBoardingData() {
    std::ofstream f(BOARDING_FILE);
    if (!f) { std::cout << "无法打开文件！" << std::endl; return; }
    for (const auto& r : boardingRecords)
        f << r.id << "," << r.petName << "," << r.ownerName << "," << r.ownerPhone << ","
        << r.startDate << "," << r.endDate << "," << r.status << "," << r.price << "\n";
}

// 从文件加载寄养记录
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

// 添加寄养记录
void addBoardingRecord() {
    clearScreen();
    std::cout << "===== 添加宠物寄养记录 =====" << std::endl;
    PetBoarding r;
    r.id = getNextId(boardingRecords);
    std::cout << "宠物名称: "; std::getline(std::cin, r.petName);
    std::cout << "主人名称: "; std::getline(std::cin, r.ownerName);
    std::cout << "主人电话: "; std::getline(std::cin, r.ownerPhone);
    r.startDate = getCurrentDate();
    std::cout << "开始日期: " << r.startDate << " (当前)" << std::endl;
    r.endDate = getFutureDate(getIntInput("寄养天数: ", 1, 365));
    std::cout << "结束日期: " << r.endDate << std::endl;
    r.status = "在住";
    std::cout << "状态: " << r.status << std::endl;
    r.price = getIntInput("价格: ", 0, 10000);
    boardingRecords.push_back(r);
    saveBoardingData();
    std::cout << "添加成功！" << std::endl;
    pauseScreen();
}

// 查看所有寄养记录
void viewBoardingRecords() {
    clearScreen();
    std::cout << "===== 查看宠物寄养记录 =====" << std::endl;
    if (boardingRecords.empty()) {
        std::cout << "没有记录！" << std::endl;
        pauseScreen();
        return;
    }
    std::cout << std::left << std::setw(5) << "ID" << std::setw(15) << "宠物名称"
        << std::setw(15) << "主人名称" << std::setw(15) << "主人电话"
        << std::setw(12) << "开始日期" << std::setw(12) << "结束日期"
        << std::setw(10) << "状态" << std::setw(10) << "价格" << std::endl;
    std::cout << std::string(94, '-') << std::endl;
    for (const auto& r : boardingRecords)
        std::cout << std::left << std::setw(5) << r.id << std::setw(15) << r.petName
        << std::setw(15) << r.ownerName << std::setw(15) << r.ownerPhone
        << std::setw(12) << r.startDate << std::setw(12) << r.endDate
        << std::setw(10) << r.status << std::setw(10) << r.price << std::endl;
    pauseScreen();
}

// 更新寄养记录
void updateBoardingRecord() {
    clearScreen();
    std::cout << "===== 更新宠物寄养记录 =====" << std::endl;
    if (boardingRecords.empty()) {
        std::cout << "没有记录！" << std::endl;
        pauseScreen();
        return;
    }
    int id = getIntInput("请输入要更新的记录ID: ", 0, 9999);
    auto it = boardingRecords.begin();
    for (; it != boardingRecords.end(); ++it) {
        if (it->id == id) break;
    }
    if (it == boardingRecords.end()) {
        std::cout << "ID不存在！" << std::endl;
        pauseScreen();
        return;
    }
    std::cout << "\n当前记录信息：\n"
        << "1. 宠物名称: " << it->petName << "\n"
        << "2. 主人名称: " << it->ownerName << "\n"
        << "3. 主人电话: " << it->ownerPhone << "\n"
        << "4. 开始日期: " << it->startDate << "\n"
        << "5. 结束日期: " << it->endDate << "\n"
        << "6. 状态: " << it->status << "\n"
        << "7. 价格: " << it->price << "\n"
        << "8. 返回\n选择要更新的项目: ";
    int choice = getIntInput("", 1, 8);
    if (choice == 8) return;
    cinClear();
    switch (choice) {
    case 1: std::cout << "新名称: "; std::getline(std::cin, it->petName); break;
    case 2: std::cout << "新主人: "; std::getline(std::cin, it->ownerName); break;
    case 3: std::cout << "新电话: "; std::getline(std::cin, it->ownerPhone); break;
    case 4: std::cout << "新日期: "; std::getline(std::cin, it->startDate); break;
    case 5: std::cout << "新日期: "; std::getline(std::cin, it->endDate); break;
    case 6: std::cout << "新状态: "; std::getline(std::cin, it->status); break;
    case 7: it->price = getIntInput("新价格: ", 0, 10000); break;
    }
    saveBoardingData();
    std::cout << "更新成功！" << std::endl;
    pauseScreen();
}

// 删除寄养记录
void deleteBoardingRecord() {
    clearScreen();
    std::cout << "===== 删除宠物寄养记录 =====" << std::endl;
    if (boardingRecords.empty()) {
        std::cout << "没有记录！" << std::endl;
        pauseScreen();
        return;
    }
    int id = getIntInput("请输入要删除的记录ID: ", 0, 9999);
    auto it = std::find_if(boardingRecords.begin(), boardingRecords.end(),
        [id](const PetBoarding& r) { return r.id == id; });
    if (it == boardingRecords.end()) {
        std::cout << "ID不存在！" << std::endl;
        pauseScreen();
        return;
    }
    std::cout << "\n确认删除以下记录吗？\n"
        << "ID: " << it->id << "\n宠物名称: " << it->petName
        << "\n主人名称: " << it->ownerName << "\n确认删除 (Y/N): ";
    char c; std::cin >> c;
    if (c == 'Y' || c == 'y') {
        boardingRecords.erase(it);
        saveBoardingData();
        std::cout << "删除成功！" << std::endl;
    }
    else {
        std::cout << "取消删除" << std::endl;
    }
    pauseScreen();
}

// 寄养管理菜单
void boardingManagement() {
    while (true) {
        clearScreen();
        std::cout << "===== 宠物寄养管理 =====" << std::endl;
        std::cout << "1. 添加寄养记录\n2. 查看寄养记录\n3. 更新寄养记录"
            << "\n4. 删除寄养记录\n5. 返回主菜单\n请选择: ";
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

// 保存美容记录到文件
void saveGroomingData() {
    std::ofstream f(GROOMING_FILE);
    if (!f) { std::cout << "无法打开文件！" << std::endl; return; }
    for (const auto& r : groomingRecords)
        f << r.id << "," << r.petName << "," << r.ownerName << "," << r.ownerPhone << ","
        << r.serviceDate << "," << r.serviceType << "," << r.status << "," << r.price << "\n";
}

// 从文件加载美容记录
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

// 添加新的美容记录
void addGroomingRecord() {
    clearScreen();
    std::cout << "===== 添加宠物美容记录 =====" << std::endl;
    PetGrooming r;
    r.id = getNextId(groomingRecords);
    cinClear();
    std::cout << "宠物名称: "; std::getline(std::cin, r.petName);
    std::cout << "主人名称: "; std::getline(std::cin, r.ownerName);
    std::cout << "主人电话: "; std::getline(std::cin, r.ownerPhone);
    r.serviceDate = getCurrentDate();
    std::cout << "服务日期: " << r.serviceDate << " (当前)" << std::endl;
    std::cout << "服务类型 (洗澡/修剪/全套): "; std::getline(std::cin, r.serviceType);
    r.status = "未完成";
    std::cout << "状态: " << r.status << std::endl;
    r.price = getIntInput("价格: ", 0, 10000);
    groomingRecords.push_back(r);
    saveGroomingData();
    std::cout << "添加成功！" << std::endl;
    pauseScreen();
}

// 查看所有美容记录
void viewGroomingRecords() {
    clearScreen();
    std::cout << "===== 查看宠物美容记录 =====" << std::endl;
    if (groomingRecords.empty()) {
        std::cout << "没有记录！" << std::endl;
        pauseScreen();
        return;
    }
    std::cout << std::left << std::setw(5) << "ID" << std::setw(15) << "宠物名称"
        << std::setw(15) << "主人名称" << std::setw(15) << "主人电话"
        << std::setw(12) << "服务日期" << std::setw(10) << "服务类型"
        << std::setw(10) << "状态" << std::setw(10) << "价格" << std::endl;
    std::cout << std::string(92, '-') << std::endl;
    for (const auto& r : groomingRecords)
        std::cout << std::left << std::setw(5) << r.id << std::setw(15) << r.petName
        << std::setw(15) << r.ownerName << std::setw(15) << r.ownerPhone
        << std::setw(12) << r.serviceDate << std::setw(10) << r.serviceType
        << std::setw(10) << r.status << std::setw(10) << r.price << std::endl;
    pauseScreen();
}

// 更新美容记录
void updateGroomingRecord() {
    clearScreen();
    std::cout << "===== 更新宠物美容记录 =====" << std::endl;
    if (groomingRecords.empty()) {
        std::cout << "没有记录！" << std::endl;
        pauseScreen();
        return;
    }
    int id = getIntInput("请输入要更新的记录ID: ", 0, 9999);
    auto it = std::find_if(groomingRecords.begin(), groomingRecords.end(),
        [id](const PetGrooming& r) { return r.id == id; });
    if (it == groomingRecords.end()) {
        std::cout << "ID不存在！" << std::endl;
        pauseScreen();
        return;
    }
    std::cout << "\n当前记录信息：\n"
        << "1. 宠物名称: " << it->petName << "\n"
        << "2. 主人名称: " << it->ownerName << "\n"
        << "3. 主人电话: " << it->ownerPhone << "\n"
        << "4. 服务日期: " << it->serviceDate << "\n"
        << "5. 服务类型: " << it->serviceType << "\n"
        << "6. 状态: " << it->status << "\n"
        << "7. 价格: " << it->price << "\n"
        << "8. 返回\n选择要更新的项目: ";
    int choice = getIntInput("", 1, 8);
    if (choice == 8) return;
    cinClear();
    switch (choice) {
    case 1: std::cout << "新名称: "; std::getline(std::cin, it->petName); break;
    case 2: std::cout << "新主人: "; std::getline(std::cin, it->ownerName); break;
    case 3: std::cout << "新电话: "; std::getline(std::cin, it->ownerPhone); break;
    case 4: std::cout << "新日期: "; std::getline(std::cin, it->serviceDate); break;
    case 5: std::cout << "新类型: "; std::getline(std::cin, it->serviceType); break;
    case 6: std::cout << "新状态: "; std::getline(std::cin, it->status); break;
    case 7: it->price = getIntInput("新价格: ", 0, 10000); break;
    }
    saveGroomingData();
    std::cout << "更新成功！" << std::endl;
    pauseScreen();
}

// 删除美容记录
void deleteGroomingRecord() {
    clearScreen();
    std::cout << "===== 删除宠物美容记录 =====" << std::endl;
    if (groomingRecords.empty()) {
        std::cout << "没有记录！" << std::endl;
        pauseScreen();
        return;
    }
    int id = getIntInput("请输入要删除的记录ID: ", 0, 9999);
    auto it = std::find_if(groomingRecords.begin(), groomingRecords.end(),
        [id](const PetGrooming& r) { return r.id == id; });
    if (it == groomingRecords.end()) {
        std::cout << "ID不存在！" << std::endl;
        pauseScreen();
        return;
    }
    std::cout << "\n确认删除以下记录吗？\n"
        << "ID: " << it->id << "\n宠物名称: " << it->petName
        << "\n主人名称: " << it->ownerName << "\n确认删除 (Y/N): ";
    char c; std::cin >> c;
    if (c == 'Y' || c == 'y') {
        groomingRecords.erase(it);
        saveGroomingData();
        std::cout << "删除成功！" << std::endl;
    }
    else {
        std::cout << "取消删除" << std::endl;
    }
    pauseScreen();
}

// 美容管理菜单
void groomingManagement() {
    while (true) {
        clearScreen();
        std::cout << "===== 宠物美容管理 =====" << std::endl;
        std::cout << "1. 添加美容记录\n2. 查看美容记录\n3. 更新美容记录"
            << "\n4. 删除美容记录\n5. 返回主菜单\n请选择: ";
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

// 保存疫苗记录到文件
void saveVaccineData() {
    std::ofstream f(VACCINE_FILE);
    if (!f) { std::cout << "无法打开文件！" << std::endl; return; }
    for (const auto& r : vaccineRecords)
        f << r.id << "," << r.petName << "," << r.ownerName << "," << r.ownerPhone << ","
        << r.vaccineType << "," << r.vaccineDate << "," << r.nextDueDate << ","
        << r.status << "," << r.price << "\n";
}

// 从文件加载疫苗记录
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

// 添加新的疫苗记录
void addVaccineRecord() {
    clearScreen();
    std::cout << "===== 添加宠物疫苗记录 =====" << std::endl;
    PetVaccine r;
    r.id = getNextId(vaccineRecords);
    cinClear();
    std::cout << "宠物名称: "; std::getline(std::cin, r.petName);
    std::cout << "主人名称: "; std::getline(std::cin, r.ownerName);
    std::cout << "主人电话: "; std::getline(std::cin, r.ownerPhone);
    std::cout << "疫苗类型: "; std::getline(std::cin, r.vaccineType);
    r.vaccineDate = getCurrentDate();
    std::cout << "疫苗日期: " << r.vaccineDate << " (当前)" << std::endl;
    int days = getIntInput("下次疫苗间隔天数: ", 1, 365);
    r.nextDueDate = getFutureDate(days);
    std::cout << "下次疫苗日期: " << r.nextDueDate << std::endl;
    r.status = "已完成";
    std::cout << "状态: " << r.status << std::endl;
    r.price = getIntInput("价格: ", 0, 10000);
    vaccineRecords.push_back(r);
    saveVaccineData();
    std::cout << "添加成功！" << std::endl;
    pauseScreen();
}

// 查看所有疫苗记录
void viewVaccineRecords() {
    clearScreen();
    std::cout << "===== 查看宠物疫苗记录 =====" << std::endl;
    if (vaccineRecords.empty()) {
        std::cout << "没有记录！" << std::endl;
        pauseScreen();
        return;
    }
    std::cout << std::left << std::setw(5) << "ID" << std::setw(15) << "宠物名称"
        << std::setw(15) << "主人名称" << std::setw(15) << "主人电话"
        << std::setw(15) << "疫苗类型" << std::setw(12) << "疫苗日期"
        << std::setw(12) << "下次疫苗" << std::setw(10) << "状态"
        << std::setw(10) << "价格" << std::endl;
    std::cout << std::string(109, '-') << std::endl;
    for (const auto& r : vaccineRecords)
        std::cout << std::left << std::setw(5) << r.id << std::setw(15) << r.petName
        << std::setw(15) << r.ownerName << std::setw(15) << r.ownerPhone
        << std::setw(15) << r.vaccineType << std::setw(12) << r.vaccineDate
        << std::setw(12) << r.nextDueDate << std::setw(10) << r.status
        << std::setw(10) << r.price << std::endl;
    pauseScreen();
}

// 更新疫苗记录
void updateVaccineRecord() {
    clearScreen();
    std::cout << "===== 更新宠物疫苗记录 =====" << std::endl;
    if (vaccineRecords.empty()) {
        std::cout << "没有记录！" << std::endl;
        pauseScreen();
        return;
    }
    int id = getIntInput("请输入要更新的记录ID: ", 0, 9999);
    auto it = std::find_if(vaccineRecords.begin(), vaccineRecords.end(),
        [id](const PetVaccine& r) { return r.id == id; });
    if (it == vaccineRecords.end()) {
        std::cout << "ID不存在！" << std::endl;
        pauseScreen();
        return;
    }
    std::cout << "\n当前记录信息：\n"
        << "1. 宠物名称: " << it->petName << "\n"
        << "2. 主人名称: " << it->ownerName << "\n"
        << "3. 主人电话: " << it->ownerPhone << "\n"
        << "4. 疫苗类型: " << it->vaccineType << "\n"
        << "5. 疫苗日期: " << it->vaccineDate << "\n"
        << "6. 下次疫苗日期: " << it->nextDueDate << "\n"
        << "7. 状态: " << it->status << "\n"
        << "8. 价格: " << it->price << "\n"
        << "9. 返回\n选择要更新的项目: ";
    int choice = getIntInput("", 1, 9);
    if (choice == 9) return;
    cinClear();
    switch (choice) {
    case 1: std::cout << "新名称: "; std::getline(std::cin, it->petName); break;
    case 2: std::cout << "新主人: "; std::getline(std::cin, it->ownerName); break;
    case 3: std::cout << "新电话: "; std::getline(std::cin, it->ownerPhone); break;
    case 4: std::cout << "新类型: "; std::getline(std::cin, it->vaccineType); break;
    case 5: std::cout << "新日期: "; std::getline(std::cin, it->vaccineDate); break;
    case 6: std::cout << "新日期: "; std::getline(std::cin, it->nextDueDate); break;
    case 7: std::cout << "新状态: "; std::getline(std::cin, it->status); break;
    case 8: it->price = getIntInput("新价格: ", 0, 10000); break;
    }
    saveVaccineData();
    std::cout << "更新成功！" << std::endl;
    pauseScreen();
}

// 删除疫苗记录
void deleteVaccineRecord() {
    clearScreen();
    std::cout << "===== 删除宠物疫苗记录 =====" << std::endl;
    if (vaccineRecords.empty()) {
        std::cout << "没有记录！" << std::endl;
        pauseScreen();
        return;
    }
    int id = getIntInput("请输入要删除的记录ID: ", 0, 9999);
    auto it = std::find_if(vaccineRecords.begin(), vaccineRecords.end(),
        [id](const PetVaccine& r) { return r.id == id; });
    if (it == vaccineRecords.end()) {
        std::cout << "ID不存在！" << std::endl;
        pauseScreen();
        return;
    }
    std::cout << "\n确认删除以下记录吗？\n"
        << "ID: " << it->id << "\n宠物名称: " << it->petName
        << "\n主人名称: " << it->ownerName << "\n确认删除 (Y/N): ";
    char c; std::cin >> c;
    if (c == 'Y' || c == 'y') {
        vaccineRecords.erase(it);
        saveVaccineData();
        std::cout << "删除成功！" << std::endl;
    }
    else {
        std::cout << "取消删除" << std::endl;
    }
    pauseScreen();
}

// 疫苗管理菜单
void vaccineManagement() {
    while (true) {
        clearScreen();
        std::cout << "===== 宠物疫苗管理 =====" << std::endl;
        std::cout << "1. 添加疫苗记录\n2. 查看疫苗记录\n3. 更新疫苗记录"
            << "\n4. 删除疫苗记录\n5. 返回主菜单\n请选择: ";
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