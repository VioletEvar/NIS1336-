#include <iostream>
#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>
#include <ctime>
#include <fstream>
#include <cctype>
#include <sstream>
#include <mutex>
#include <thread>
#include <iomanip>
#include "account.h"


//�����������ĺ���
void help(){
        std::cout <<"-----------------------------------------------------------------------" << std::endl;
        std::cout << "���ǰ�������" << std::endl;
        std::cout << "�����û������������ʹ�ý���(��δ�����˻����Զ�����)" << std::endl;
        std::cout << "ʹ�ý����У�����showtask��ʾ�����" << std::endl;
        std::cout << "ʹ�ý����У�����addtask�������" <<std::endl;
        std::cout << "ʹ�ý����У�����deltaskɾ������" <<std::endl;
        std::cout << "ʹ�ý����У�����quit�˳�����" <<std::endl;
        std::cout << "ʹ�ý����У�����h��ʾ��������" << std::endl;
        std::cout <<"-----------------------------------------------------------------------" << std::endl;
}

//�ж��û��ļ��Ƿ��Ѵ����ĺ���
bool isCreated(std::string filename){
        std::ifstream inputfile(filename);
        if (!inputfile) {
            return false;
        }
        return true;
}

//�����û������ļ��ĺ���
int createFile(std::string filename){
        if(isCreated(filename)){
                return 0;//����0���������û��ļ�
        }
        std::ofstream userfile(filename);
        if (!userfile.is_open()) {
            std::cout << "�޷������ļ� " << filename << " ����д�룡" << std::endl;
            return -1;//����-1������ʧ��
        }
        userfile.close();
        std::cout << "�ļ������ɹ�!" << std::endl;
        return 1;//����1�������ɹ�
}

//�����û��ļ��ĺ���
void AccountManager::update(){
        //����û��ļ�
        //���´����û��ļ�
        std::string newfilename = username + "_task.txt";//���û��������ļ���ȷ����׺׼ȷʶ��
        std::ofstream newfile(newfilename);

        //������д���һ��
        newfile << passwordHash << std::endl;
        //�����ID��д��ڶ���
        newfile << max_id  << std::endl;
        //�������д���µ��û��ļ�
        //��Χ-based for ѭ���������� tasks �����е�ÿ��Ԫ�أ�������ǰԪ�ظ�ֵ������ task��
        //const ���η���ʾ task ��һ���������ã���ȷ������� data �е�Ԫ�ؽ����޸ġ�
        for(const Task& task : tasks){
                newfile << task.reminderTime << " " << task.name << " " << task.startTime << " " << task.category << " " << task.priority << " " << task.id << std::endl;
        }
        newfile.close();
        std::cout << "�ļ����³ɹ�!" << std::endl;
}

bool AccountManager::sortTaskFunction(Task a, Task b) {
    return a.startTime < b.startTime;//�����ȿ�ʼ������
}

//���ͬ��ͬ��ʼʱ�������Ƿ����
bool AccountManager::taskExist(Task a){
        for (const Task& task : tasks){
                if(task.name == a.name && task.startTime == a.startTime){
                        return true;
                }
        }
        return false;
}

//���û����񰴿�ʼʱ������
void AccountManager::sortTasksByStartTime() {
    std::sort(tasks.begin(), tasks.end(), sortTaskFunction);//�����������ʼʱ���Ⱥ�����
}

// �����ϣ�㷨���ı��Сд�����ֱ��ֲ���
std::string AccountManager::calculatePasswordHash(const std::string& password) const {
    std::string hashedPassword = "";
    int special_value = 0;//���������д��ĸ��Сд��ĸ�����ָ����ڹ�ϣֵ��β�����û�����ֵ
    //���������ַ������ı���ĸ��Сд�����������ϣ
    for (char c : password) {
        if (std::isalpha(c)) {
            if (std::isupper(c)) {
                hashedPassword += std::tolower(c);
                special_value += 2;
            }
            else {
                hashedPassword += std::toupper(c);
                special_value += 7;
            }
        }
        else {//��������ʱ����
            hashedPassword += c;
            special_value +=11;
        }
    }
    hashedPassword += std::to_string(special_value);
    return hashedPassword;

}

//�û���ʼ�������������û�����������û������ļ�
    //����Ƿ��Ѵ����˻�
    //���У���ֱ�Ӵ��Ѿ��������˺��ļ�
    //���ޣ����½�һ���˺��ļ�
AccountManager::AccountManager(const std::string& input_username, const std::string& input_password) :username(input_username){
    std::string filename = input_username + "_task.txt";//Ϊ�����û��ļ�����ӹ̶���׺"_task.txt"
    max_id = 0;//��ʼ������������Ϊ0
    int filestate = createFile(filename);//�������ͱ�������createFile��������ֵ
    std::string input_passwordHash = calculatePasswordHash(input_password);//������������Ĺ�ϣֵ
//�����û��ļ�����������г�ʼ������
    switch(filestate){
case 0:{//�����û��ļ�
        std::ifstream is(filename);//���û��ļ�
        std::string saved_passwordHash;
        std::getline(is, saved_passwordHash);//��ȡ�������û��ļ���һ�е������ϣ
        //�ж����������Ƿ���ȷ
        if(input_passwordHash == saved_passwordHash){//����������ȷ
                passwordHash = saved_passwordHash;//��ʼ�����˺������ϣ
                //���ļ�������¼���˺ŵ�tasks�����
                std::string line;
                std::getline(is,line);
                max_id = std::stoi(line);//�����ļ������ID��
                while (std::getline(is, line)) {
                Task task;
                std::istringstream iss(line);//��ÿ���ļ����ص�istringstream��iss��
                iss >> task.reminderTime >> task.name >> task.startTime >> task.category >> task.priority >> task.id ;//����issΪ����������ݳ�Ա��ֵ
                tasks.push_back(task);//��������������
                }
                is.close();//�ر��ļ�
        }
        break;
}
case -1:{//�û��ļ�����ʧ��
        std::cout << "�����ļ���������ʧ��!" << std::endl;
        break;
}
case 1:{//���û��������û��ļ��ɹ�
        std::cout << "�ɹ������ļ���������" << std::endl;
        passwordHash = input_passwordHash;//��ʼ�������ϣֵ
        max_id = 0;//��ʼ������������Ϊ0
        //������д���û��ļ���һ��
        std::ofstream os(filename);//��д��ģʽ���û��ļ�
        if(!os.is_open()){
                std::cout << "�޷����ļ�!" << std::endl;
        }
        os << passwordHash <<std::endl;
        //���������idд���û��ļ��ڶ���
        os << max_id <<std::endl;
        os.close();//�ر��û��ļ�

        break;
}
default:{//�����쳣���
        std::cout << "δ֪�����˺Ŵ������ɹ�!"<< std::endl;
        break;
}

    }
        return;
}

//��֤���������Ƿ���ȷ�ĺ���
bool AccountManager::authenticate(const std::string& password) const {
    // �������������й�ϣ���㣬���뱣��Ĺ�ϣֵ���бȽ�
    std::string inputHash = calculatePasswordHash(password);
    return inputHash == passwordHash;
}

//�������ĺ���
void AccountManager::addTask(const Task& task) {
    //��������Ƿ��Ѵ���
    if(taskExist(task)){
            std::cout << "�����Ѵ���!" << std::endl;
            max_id--;//��ֹmax_id����+2����
            return;
    }
    //�����񲻴��ڣ�������������
    else{
            tasks.push_back(task);
            update();//�����û��ļ�
    }
}

//ɾ������ĺ���
bool AccountManager::deleteTask(const int taskId) {
        //�����������ұ��ΪtaskId���������ҵ���ɾ��
    bool result = false;
    for (auto it = tasks.begin(); it != tasks.end(); ++it) {
        if (it->id == taskId) {
            tasks.erase(it);//����vector���Դ�����erase()���������ɾ��;Ϊʹ�ô˺�������forѭ��ʹ��ָ�����tasksʸ��
            result = true;
            break;
        }
    }
    if(result){
            update();//�����û��ļ�
    }
    return result;
}

//��ʾ����ĺ���
void AccountManager::showTasks() {
// ���������������ݿ�ʼʱ������
    sortTasksByStartTime();
    std::cout <<"-----------------------------------------------------------------------" << std::endl;
    //������������������š�����������ʼʱ�䡢���ȼ������ࡢ����ʱ��
    std::cout <<std::setw(3)<< "ID|" << std::setw(13) << "Name|" << std::setw(21)<< "Start Time|" << std::setw(17) << "Priority|" << std::setw(17)<< "Category|" << std::setw(22) <<"Reminder Time" << std::endl;
    for (const Task& task : tasks) {
        std::cout <<std::setw(2)<< task.id << "|" << std::setw(12) << task.name << "|" << std::setw(20) << task.startTime << "|" << std::setw(16) << task.priority << "|" << std::setw(16)<< task.category << "|" << std::setw(21) << ctime(&task.reminderTime)  <<std::endl;//����ctime������time_t���ͱ���ת��Ϊtm�ṹ��������
    }
    std::cout <<"-----------------------------------------------------------------------" << std::endl;
}

//�޸�����ĺ���
void AccountManager::changeTask(const Task& task) {
	bool deleted = deleteTask(task.id); //��������ɾȥͬID��task
	if(deleted){
                tasks.push_back(task); //�������������task
                update();//�����û��ļ�
	}
        return;
}


//�ṩ�ļ����Ľӿں���
std::string AccountManager::getFilename(){
        return filename;
}

//�ṩ�û����Ľӿں���
std::string AccountManager::getUsername(){
        return username;
}


