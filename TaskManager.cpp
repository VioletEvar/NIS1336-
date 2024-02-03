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
#include <chrono>//����chronoʱ�������߳�����ʱ��
#include "account.h"

//�������ռ� std::chrono �е���������׺���뵱ǰ�������ռ䣬�Ա�ֱ��ʹ����Щ������
using namespace std::chrono_literals;

std::mutex fileMutex;//����ȫ�ֻ���������
bool quit_program = false;

//�����̺߳���
void userAlarm(std::string filename){

        while(!quit_program){
                std::this_thread::sleep_for(2002ms);
                std::lock_guard<std::mutex> lock(fileMutex);//��ס�ļ�
                time_t now; //��ȡ��ǰʱ��
                time(&now);
    std::ifstream inf(filename);
    if(!inf){//����Ƿ�ɹ���

     std::cout << "fail to open the file" << std::endl;
    }
                //��ȡ�����ļ�
                time_t tasktime;
                std::string line;
                std::string taskname,startime;
                //����ƥ��ʱ���ж��Ƿ�����
                while(getline(inf,line)){
                  std::istringstream is(line);//�ָ�ÿһ���ַ�����ȡ������ʱ�ֽ���д��
                  if(is >> tasktime >> taskname >> startime){
                                        time_t diff = now - tasktime;
                                        if(diff <= 1 && diff >= -1){
                                                //Remind
                                                std::cout<<std::endl;
                                                std::cout<<taskname<<"����"<<startime<<"��ʼ"<<std::endl;
                                                continue;
                                        }
                  } //д���ʽΪ time_t �������� ��ʼʱ��
                }
        }
}

//�û�ʹ�ý����̺߳���
void userOperate(AccountManager account, bool &quit){
	std::string command;// ��������������ָ����ַ�������command
        // ѭ������û�ָ��
        while(true){
                std::cin>>command;// �������ж���ָ��
                std::transform(command.begin(), command.end(), command.begin(), ::tolower);//ʹ��vector�Դ���transform������ָ����������ĸת��ΪСд��ʵ�ִ�Сд������
                // ִ����Ӧ������
        if (command == "addtask") {//�������
                std::cout<<"������������������ʱ��(�� �� �� ʱ ��),������������ʼʱ��(xxxx/xx/xx/xx:xx)"<<std::endl;
                // �������������в������������������
                tm tm_tasktime = {0};//����ctime����tm�ṹ���ȡ����ʱ��
        		Task task;

        		task.id = ++account.max_id;//����account��������ID�ų�ʼ��������id
        		//����tm�ṹ�������1900�꿪ʼ���´�0��ʼ����������int����year��mon���ղ�����ʵ���û���ϰ������ʱ��ĸ���
        		int year = 0;
        		int mon = 0;
        		std::cin >> year >> mon >> tm_tasktime.tm_mday >> tm_tasktime.tm_hour >> tm_tasktime.tm_min >> task.name >> task.startTime;// >> task.category >> task.priority;
 				//�������ȱʡֵ����
 				std::cout << "�������������(Ĭ��Ϊother���)" << std::endl;
 				std::cin.get();//ȥ�����ܶ�ȡ�Ļ��з�
 				std::string line;
 				std::getline(std::cin, line);//��һ����������ַ�������line����
 				if(line == ""){
                                        task.category = "other";//Ĭ���������Ϊ����
 				}
 				else{
                                        task.category = line;
 				}
 				//�������ȼ���ȱʡֵ���벢������ȼ��Ϸ���
                                std::cout << "�������������ȼ�(high | medium | low), Ĭ��Ϊlow" << std::endl;
 				std::getline(std::cin, line);
 				if(line == "" || line == "low"){
                                        task.priority = "low";
 				}
 				else if(line == "medium"){
                                        task.priority = "medium";
 				}
 				else if(line == "high"){
                                        task.priority = "high";
 				}
 				else{
                                        std::cout << "�������ȼ������ϸ�ʽ��������Ϊlow" << std::endl;
                                        task.priority = "low";//���벻�Ϸ�ʱ����Ϊlow
 				}

 				tm_tasktime.tm_year = year-1900; //tm_year��ʾ��1900�󾭹������
 				tm_tasktime.tm_mon = mon - 1;//tm_mon���·ݴ�0��ʼ����-1ȷ���·���ȷ
 				task.reminderTime = mktime(&tm_tasktime);//����mktime������tm�ṹ��ת��Ϊtime_t�ͱ�������
 				{
                                        std::lock_guard<std::mutex> lock(fileMutex);//�ڽ�������������ǰ��ס�ļ�
                                        account.addTask(task);
 				}
                std::cout << "Task added successfully." << std::endl;

        }
        else if (command == "deltask") {
        // �������������в�������ȡҪɾ���������ID
                int taskId; //����int����taskId��ɾ������ID
                std::cout << "������Ҫɾ�����������" << std::endl;
                std::cin >> taskId;
                bool deleted = false;
                {
                        std::lock_guard<std::mutex> lock(fileMutex);//�ڽ���ɾ�����ǰ��ס�ļ�
                        deleted = account.deleteTask(taskId);
                }
                if(deleted){
                        std::cout << "Task deleted successfully." << std::endl;
                }
                else{
                        std::cout << "Task not deleted successfully." << std::endl;
                }

        }
        else if (command == "changetask") {//�޸�����
         	std::string filename = account.getUsername() + "_task.txt";//��ȡ�ļ���
         		std::ifstream inf(filename);
         		if(!inf){//����Ƿ�ɹ����û��ļ�
     				std::cout << "fail to open the file" << std::endl;
   				}
                //��ȡ�����ļ�
                bool flag = false;
                Task task_c;
                std::cout << "�����������޸ĵ�����ID:" ;
                int taskid,id;
		std::cin >> taskid;
                std::string line;
                std::string tasktime,taskname,startime,catagory,priority;
                while(getline(inf,line)){
                  std::istringstream is(line);//�ָ�ÿһ���ַ�����ȡ������ʱ�ֽ���д��
                  if(is >> tasktime >> taskname >> startime >> catagory >> priority >> id){ //��ÿһ��д�����
                	if(id == taskid){
                		//���ļ��и���ָ����task�����ݵ�task_c�� ��������޸ĺ���changeTask����
                		task_c.category = catagory;
                		task_c.priority = priority;
                		task_c.id = id;
                		task_c.name = taskname;
                		task_c.startTime = startime;
                                task_c.reminderTime = stoi(tasktime);
                		int num;
                		flag = true;
                		std::cout << "����������Ҫ�޸ĵ����ݶ�Ӧ����ţ�1.�������� 2.����ʼʱ�� 3.������� 4.�������ȼ� 5.��������ʱ�䣩 : " ;
                		std::cin >> num;
                		switch(num){
                			case 1:{
                				std::cout << "�������޸ĺ���������ƣ�" ;
                				std::cin >> task_c.name;
								break;
							}
                			case 2:{
                				std::cout << "�������޸ĺ������ʼʱ�䣺" ;
                				std::cin >> task_c.startTime;
								break;
							}
                			case 3:{
                				std::cout << "�������޸ĺ���������" ;
                				std::cin >> task_c.category;
								break;
							}
                			case 4:{
                				std::cout << "�������޸ĺ���������ȼ���" ;
                				std::cin >> task_c.priority;
								break;
							}
                			case 5:{
                				tm t_tasktime = {0};
                				int year, mon;
                				std::cout <<"�����������µ�����ʱ��(�� �� �� ʱ ��):" ;
                				std::cin >> year >> mon >> t_tasktime.tm_mday >> t_tasktime.tm_hour >> t_tasktime.tm_min;
                				t_tasktime.tm_year = year-1900;
 								t_tasktime.tm_mon = mon - 1;
 								task_c.reminderTime = mktime(&t_tasktime);
								break;
							}
                			default:{
                				std::cout << "�Ƿ����룡" << std::endl;
								break;
							}
						}
					}
				}
            }
            if(!flag) {
                std::cout << "δ�ҵ�������" << std::endl; //�������ļ�����flag�ж��Ƿ��ѯ��������
                continue;
            }
            {
                std::lock_guard<std::mutex> lock(fileMutex);//��ס�ļ�
                account.changeTask(task_c);
            }

	}
        else if (command == "showtask") {//��ʾ�����������
		        account.showTasks();
        }
        else if(command == "h"){//��ʾ��������
                help();
        }
        else if(command == "quit"){//�˳�����
                std::cout << "�˳��ɹ�!" << std::endl;
                quit = true;//��quit������Ϊtrue����Ϊflag����һ�̺߳���userAlarm����
		        break;//����whileѭ��
        }
        else {
                // �û�����δ֪����
                std::cout << "����δ֪" << std::endl;

                //�����������
                help();
        }
}

}


int main(int argc, char* argv[]) {
    // ���������в�����ִ����Ӧ����
    if (argc < 3) {
        help();//�������˵��
        return 0;
    }

    std::string username = argv[1];
    std::string password = argv[2];


    // �����˻��������
    AccountManager account(username, password);

    // ����û���֤ʧ�ܣ����˳�����
    if (!account.authenticate(password)) {
        std::cout << "�û���֤ʧ�ܣ������µ�¼" << std::endl;
        return 0;
    }

        std::cout << "��¼�ɹ�!" << std::endl;

        std::thread thread1(userOperate, std::ref(account), std::ref(quit_program));//�û�����ģ��
        std::string filename = username + "_task.txt";
        std::thread thread2(userAlarm, std::ref(filename));//��������ģ��


    thread1.join();
    thread2.join();

    return 0;
}


