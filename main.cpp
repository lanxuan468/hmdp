#include <iostream>
#include "pet_system.h"

int main() {
    // 加载系统数据
    loadAdminData();
    loadBoardingData();
    loadGroomingData();
    loadVaccineData();

    // 管理员登录验证
    if (!adminLogin()) {
        return 1; // 登录失败退出
    }

    // 主菜单循环
    int choice = 0;
    while (true) {
        clearScreen();
        std::cout << "===== 宠物服务管理系统 =====" << std::endl;
        std::cout << "1. 寄养管理\n2. 美容管理\n3. 疫苗管理\n4. 退出系统" << std::endl;
        std::cout << "请选择操作 (1-4): ";

        // 输入验证
        std::cin >> choice;
        if (std::cin.fail()) {//处理非数字输入，若没有这步，输入a时初始化失败，导致choice无值。再次输入时也是没有值的
            std::cin.clear();
            std::cin.ignore(1024, '\n');//忽略无效输入
            continue;
        }

        switch (choice) {
        case 1: boardingManagement(); break;
        case 2: groomingManagement(); break;
        case 3: vaccineManagement(); break;
        case 4:
            saveAdminData();
            saveBoardingData();
            saveGroomingData();
            saveVaccineData();
            return 0; // 正常退出
        default:
            std::cout << "无效选择，请重新输入。" << std::endl;
            pauseScreen();
        }
    }

    return 0;
}