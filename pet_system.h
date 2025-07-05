#pragma once
#ifndef PET_SYSTEM_H
#define PET_SYSTEM_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

// ϵͳ���ݽṹ
struct Admin {
    std::string username;
    std::string password;
};

struct PetBoarding {
    int id;
    std::string petName;
    std::string ownerName;
    std::string ownerPhone;
    std::string startDate;
    std::string endDate;
    std::string status;
    double price;
};

struct PetGrooming {
    int id;
    std::string petName;
    std::string ownerName;
    std::string ownerPhone;
    std::string serviceDate;
    std::string serviceType;
    std::string status;
    double price;
};

struct PetVaccine {
    int id;
    std::string petName;
    std::string ownerName;
    std::string ownerPhone;
    std::string vaccineType;
    std::string vaccineDate;
    std::string nextDueDate;
    std::string status;
    double price;
};

// ȫ������
extern std::vector<PetBoarding> boardingRecords;
extern std::vector<PetGrooming> groomingRecords;
extern std::vector<PetVaccine> vaccineRecords;
extern Admin admin;

// �ļ�·������
extern const std::string ADMIN_FILE;
extern const std::string BOARDING_FILE;
extern const std::string GROOMING_FILE;
extern const std::string VACCINE_FILE;

// ����Ա����
bool adminLogin();
void saveAdminData();
void loadAdminData();

// ��������
void saveBoardingData();                    // ���������¼���ļ�
 void loadBoardingData();                    // ���ļ����ؼ�����¼
void boardingManagement();                  // ��������������
void addBoardingRecord();                   // ����µļ�����¼
void viewBoardingRecords();                 // �鿴���м�����¼
void updateBoardingRecord();                // �������м�����¼
void deleteBoardingRecord();                // ɾ��ָ��������¼

// ���ݹ���
void saveGroomingData();                    
void loadGroomingData();                    
void groomingManagement();                  
void addGroomingRecord();                   
void viewGroomingRecords();                 
void updateGroomingRecord();                
void deleteGroomingRecord();                

// �������
void saveVaccineData();                     
void loadVaccineData();                     
void vaccineManagement();                   
void addVaccineRecord();                    
void viewVaccineRecords();                  
void updateVaccineRecord();                 
void deleteVaccineRecord();                 

// ��������
std::string getCurrentDate();               // ��ȡ��ǰ���ڣ���ʽ��YYYY-MM-DD��
std::string getFutureDate(int days);        
void clearScreen();                         // ��������
void pauseScreen();                         // ��ͣ��Ļ���ȴ��û�����
int getNextId(const std::vector<PetBoarding>&);  // ��ȡ������¼��һ��ID
int getNextId(const std::vector<PetGrooming>&);   // ��ȡ���ݼ�¼��һ��ID
int getNextId(const std::vector<PetVaccine>&);     // ��ȡ�����¼��һ��ID

#endif // PET_SYSTEM_H