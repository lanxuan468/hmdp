#pragma once
#ifndef PET_SYSTEM_H
#define PET_SYSTEM_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

// 系统数据结构
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

// 全局数据
extern std::vector<PetBoarding> boardingRecords;
extern std::vector<PetGrooming> groomingRecords;
extern std::vector<PetVaccine> vaccineRecords;
extern Admin admin;

// 文件路径定义
extern const std::string ADMIN_FILE;
extern const std::string BOARDING_FILE;
extern const std::string GROOMING_FILE;
extern const std::string VACCINE_FILE;

// 管理员功能
bool adminLogin();
void saveAdminData();
void loadAdminData();

// 寄养管理
void saveBoardingData();                    // 保存寄养记录到文件
 void loadBoardingData();                    // 从文件加载寄养记录
void boardingManagement();                  // 寄养管理主界面
void addBoardingRecord();                   // 添加新的寄养记录
void viewBoardingRecords();                 // 查看所有寄养记录
void updateBoardingRecord();                // 更新现有寄养记录
void deleteBoardingRecord();                // 删除指定寄养记录

// 美容管理
void saveGroomingData();                    
void loadGroomingData();                    
void groomingManagement();                  
void addGroomingRecord();                   
void viewGroomingRecords();                 
void updateGroomingRecord();                
void deleteGroomingRecord();                

// 疫苗管理
void saveVaccineData();                     
void loadVaccineData();                     
void vaccineManagement();                   
void addVaccineRecord();                    
void viewVaccineRecords();                  
void updateVaccineRecord();                 
void deleteVaccineRecord();                 

// 辅助函数
std::string getCurrentDate();               // 获取当前日期（格式：YYYY-MM-DD）
std::string getFutureDate(int days);        
void clearScreen();                         // 清屏函数
void pauseScreen();                         // 暂停屏幕，等待用户按键
int getNextId(const std::vector<PetBoarding>&);  // 获取寄养记录下一个ID
int getNextId(const std::vector<PetGrooming>&);   // 获取美容记录下一个ID
int getNextId(const std::vector<PetVaccine>&);     // 获取疫苗记录下一个ID

#endif // PET_SYSTEM_H