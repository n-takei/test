#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<vector>
#include<iomanip>
#include<chrono>
#include<ctime>
#include<mutex>
#include<thread>
class my {
private:
	std::string name;
    std::tm birthDate;

public:
    int Age() {
        const std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        std::time_t end_time = std::chrono::system_clock::to_time_t(now);
        tm* nowDate = std::gmtime(&end_time);

        int nowYear = nowDate->tm_year + 1900;
        int nowMon = nowDate->tm_mon + 1;
        int nowDay = nowDate->tm_mday;

        int age = nowYear - this->birthDate.tm_year;

        if ((nowMon == this->birthDate.tm_mon && nowDay > this->birthDate.tm_mday) || nowMon > this->birthDate.tm_mon) {
            age--;
        }
        return age;
    }
    std::string Name() {
        return this->name;
    }
    std::string BirthDate() {
        std::stringstream ss;
        ss << std::setw(4) << std::setfill('0') << this->birthDate.tm_year << '/'
           << std::setw(2) << std::setfill('0') << this->birthDate.tm_mon << '/'
           << std::setw(2) << std::setfill('0') << this->birthDate.tm_mday;
        return ss.str();
    }

    void Show() {
        std::cout << "氏名　　　: " << this->Name() << std::endl;
        std::cout << "年齢　　　: " << this->Age() << std::endl;
        std::cout << "生年月日　: " << this->BirthDate() << std::endl;
    }

    my(std::string name, std::tm birthDate) {
        this->name = name;
        this->birthDate = birthDate;
    }
};
// std::coutへのアクセスを排他的にする
std::mutex print_mtx_;
void safe_print(my x)
{
    std::lock_guard<std::mutex> lock(print_mtx_);
    std::cout << std::endl;
    x.Show();
}

class ProfileList {
private:
    std::vector<my> list;
    std::mutex mtx_;
public:
    std::unique_lock<std::mutex> get_lock()
    {
        return std::unique_lock<std::mutex>(mtx_); // ロックを取得する
    }

    // vectorオブジェクトへのアクセスを排他的にする
    void add_value(my value)
    {
        std::unique_lock<std::mutex> lk = get_lock(); // ロックされたunique_lockを受け取る

        list.push_back(value);
    } // ロックを手放す(unique_lockのデストラクタ)

    void print()
    {
        std::unique_lock<std::mutex> lk = get_lock();

        for (my x : list) {
            safe_print(x);
        }
    }
};

int main()
{
    ProfileList list;
    std::vector<my> stack;

    std::thread t1([&list] {
        std::tm birthDate = { 0,0,0,15,3,1995 };
        my data1 = my("安田", birthDate);
        list.add_value(data1);
    });

    std::thread t2([&list] {
        std::tm birthDate = { 0,0,0,1,10,1996 };
        my data1 = my("佐藤", birthDate);
        list.add_value(data1);
    });

    
    t1.join();
    t2.join();

    list.print();
	//std::vector<my> list = std::vector<my>();

    //std::tm birthDate = { 0,0,0,15,3,1995 };
    //list.push_back(my("武井　直哉", birthDate));

 //   //my myinfo = list[0];
 //   //myinfo.Show();

 //   std::ifstream ifs("C:\\Users\\n.takei\\Desktop\\MUMSS\\support_status_20240313_1.csv");
 //   std::string buf;

 //   if (ifs.fail()) {
 //       std::cout << "ファイルオープンエラー" << std::endl;
 //       return -1;
 //   }

 //   while (std::getline(ifs, buf)) {
 //       std::cout << buf << std::endl;
 //   }
	//const std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
 //   std::time_t end_time = std::chrono::system_clock::to_time_t(now);
 //   std::cout << "Current Time and Date: " << std::ctime(&end_time) << std::endl;

	//std::cout << std::endl;
	//std::cout << "from Unix Time Stamp millisecond" << std::endl;
	//const std::chrono::system_clock::duration duration = now.time_since_epoch();
	//const long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
	//std::cout << "       ms: " << ms << std::endl;

 //   const long long sec = ms / 1000;
 //   std::tm* gt = std::gmtime(&sec);
 //   output("   gmtime: ", gt, ms % 1000);

 //   std::tm* lt = std::localtime(&sec);
 //   output("localtime: ", lt, ms % 1000);
}
