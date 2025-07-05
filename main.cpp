#include <iostream>
#include "pet_system.h"

int main() {
    // ����ϵͳ����
    loadAdminData();
    loadBoardingData();
    loadGroomingData();
    loadVaccineData();

    // ����Ա��¼��֤
    if (!adminLogin()) {
        return 1; // ��¼ʧ���˳�
    }

    // ���˵�ѭ��
    int choice = 0;
    while (true) {
        clearScreen();
        std::cout << "===== ����������ϵͳ =====" << std::endl;
        std::cout << "1. ��������\n2. ���ݹ���\n3. �������\n4. �˳�ϵͳ" << std::endl;
        std::cout << "��ѡ����� (1-4): ";

        // ������֤
        std::cin >> choice;
        if (std::cin.fail()) {//������������룬��û���ⲽ������aʱ��ʼ��ʧ�ܣ�����choice��ֵ���ٴ�����ʱҲ��û��ֵ��
            std::cin.clear();
            std::cin.ignore(1024, '\n');//������Ч����
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
            return 0; // �����˳�
        default:
            std::cout << "��Чѡ�����������롣" << std::endl;
            pauseScreen();
        }
    }

    return 0;
}