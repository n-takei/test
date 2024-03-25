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
#include <queue>
#include <filesystem>

#define MAX_ROW 65536
#define NUMBER_OF_THREAD 5

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

void safe_print(std::string x)
{
    std::lock_guard<std::mutex> lock(print_mtx_);
    std::cout << x << std::endl;
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

class ThreadTest
{
private:
    std::vector<std::string> list;
    std::mutex mtx_;
public:
    std::unique_lock<std::mutex> get_lock()
    {
        return std::unique_lock<std::mutex>(mtx_); // ロックを取得する
    }
    // vectorオブジェクトへのアクセスを排他的にする
    void add_value(std::string value)
    {
        std::unique_lock<std::mutex> lk = get_lock(); // ロックされたunique_lockを受け取る

        list.push_back(value);

    } // ロックを手放す(unique_lockのデストラクタ)

    void print()
    {
        std::unique_lock<std::mutex> lk = get_lock();

        for (std::string x : list) {
            safe_print(x);
        }
    }

    int number_of_list() {
        return list.size();
    }
};

std::mutex que_mtx_;

/**
* @brief フォルダ以下のファイル一覧を取得する関数
* @param[in]    folderPath  フォルダパス
* @param[out]   file_names  ファイル名一覧
* return        true:成功, false:失敗
*/
bool getFileNames(std::string folderPath, std::vector<std::string>& file_names)
{
    using namespace std::filesystem;
    directory_iterator iter(folderPath), end;
    std::error_code err;

    for (; iter != end && !err; iter.increment(err)) {
        const directory_entry entry = *iter;

        file_names.push_back(entry.path().string());
        //printf("%s\n", file_names.back().c_str());
    }

    /* エラー処理 */
    if (err) {
        std::cout << err.value() << std::endl;
        std::cout << err.message() << std::endl;
        return false;
    }
    return true;
}

int main()
{
    std::vector<std::pair<std::string, std::vector<std::string>>> q;
    std::vector<std::thread> thds;
    const std::string monitorDir = "C:\\temp2";
    const std::string moveToDir = "C:\\moveTo";
    std::string endFile = "end";
    bool processEnd = false;

    // id=1..5のスレッドを作成
    for (int id = 1; id <= NUMBER_OF_THREAD; id++) {
        // ループ変数idは必ずコピーキャプチャする
        //thds.emplace_back([&thTest, &q, &genVal, id] {
        thds.emplace_back([&q, &processEnd, id] {

            std::string searchWord = "#region";
            const std::string outLogDir = "C:\\outputLog";
            std::string filePath;
            std::vector<std::string> v;
            // 新しいスレッドで実行される処理
            while (!processEnd)
            {
                std::string logFilePath;

                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                {
                    std::unique_lock<std::mutex> uniq_lk(que_mtx_);// ここでロックされる
                
                    if (!q.empty()) {
                        filePath = q.back().first;
                        v = q.back().second;
                        //thTest.add_value(v);
                        q.pop_back();
                        safe_print(std::format("【スレッド{0}】値[ {1} ]がキューから取り出されました。(現在のキュー個数={2})", id, filePath, q.size()));
                    }
                    else
                    {
                        continue;
                    }
                }

                const std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
                std::time_t end_time = std::chrono::system_clock::to_time_t(now);
                tm* nowDate = std::gmtime(&end_time);
                const std::string nowDateStr;
                std::stringstream ss;
                ss << std::setw(4) << std::setfill('0') << nowDate->tm_year + 1900 <<
                    std::setw(2) << std::setfill('0') << nowDate->tm_mon + 1 <<
                    std::setw(2) << std::setfill('0') << nowDate->tm_mday <<
                    std::setw(2) << std::setfill('0') << nowDate->tm_hour <<
                    std::setw(2) << std::setfill('0') << nowDate->tm_min <<
                    std::setw(2) << std::setfill('0') << nowDate->tm_sec;

                //thTest.add_value(v);
                std::ofstream iof(outLogDir + "\\" + filePath.substr(filePath.find_last_of("\\") + 1));

                int foundCnt = 0;
                int rowNumber = 1;
                for (std::string buf : v) {
                    int foundIdx = 0;
                    int foundCntRow = 0;
                    do{
                        foundIdx = buf.find(searchWord, foundIdx);
                        if (foundIdx != std::string::npos) {
                            safe_print(std::format("【スレッド{0}】{1}行目{2}列目で文字列[ {3} ]がヒットしました。", id, rowNumber, foundIdx + 1, searchWord));
                            foundCntRow++;
                            foundIdx += searchWord.length();
                        }
                    } while (foundIdx != std::string::npos);
                    foundCnt += foundCntRow;
                    rowNumber++;
                }
                    
                if (foundCnt > 0) {
                    safe_print(std::format("【スレッド{0}】文字列[ {1} ]は全部で{2}個見つかりました。", id, searchWord, foundCnt));
                }
                else {
                    safe_print(std::format("【スレッド{0}】文字列[ {1} ]は見つかりませんでした。", id, searchWord));
                }
            }
        });
    }

    while (!processEnd)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        std::vector<std::string> filePathes;
        getFileNames(monitorDir, filePathes);

        for (std::string filePath : filePathes) {

            std::string fileName = filePath.substr(filePath.find_last_of("\\") + 1);
            if (endFile == fileName) {
                processEnd = true;
                std::filesystem::remove(filePath);
                break;
            }

            bool exist = false;
            //for (std::string d : q) {
            for (int i = 0; i < q.size(); i++) {
                //if (fileName == d) {
                if(filePath == q[i].first){
                    exist = true;
                    break;
                }
            }
            if (exist) {
                continue;
            }


            std::ifstream ifs(filePath);

            if (ifs.fail()) {
               safe_print(std::format("ファイルオープンエラー [ {0} ]", filePath));
               continue;
            }
            else {
                safe_print(std::format("ファイルオープン [ {0} ]", filePath));
                {
                    std::unique_lock<std::mutex> uniq_lk(que_mtx_); // ここでロックされる
                    q.push_back(std::pair<std::string, std::vector<std::string>>(filePath, std::vector<std::string>()));

                    std::string buf;
                    while (std::getline(ifs, buf)) {
                        //q.push_back(buf);
                        q.back().second.push_back(buf);
                    }
                }
                safe_print(std::format("値[ {0} ]がキューに追加されました。(現在のキュー個数={1})", filePath, q.size()));

                ifs.close();
                //std::filesystem::rename(filePath, std::format("{0}\\{1}", moveToDir, fileName));
                std::filesystem::remove(filePath);
            }
        }
    }

    // 全てのスレッド処理完了を待機
    for (auto& t : thds) {
        t.join();
    }
}
