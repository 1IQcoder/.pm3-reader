#include <vector>
#include <cstdint>
#include <cstring>
#include <map>
#include <variant>
#include <fstream>
#include <chrono>
#include "image_utils.h"
#include <OpenXLSX.hpp>
#include "exel_func.h"
#include "bytes_func.h"
using namespace std;
using namespace OpenXLSX;


int getFileSize(const string& filepath);
string validPath(const string& path);
template<typename T>
T input(string text, string type);
void savePreview(const vector<uint8_t>& buffer);

struct findValue {
    string base;
    size_t start;
    short width;
    bool isBigEndian;
};


class DataToTable {
    private:
        vector<uint8_t> buffer;
        map<string, variant<int, float, string>>& tableMap;

        vector<uint8_t> getBytesVector(const string& base, size_t start, short size)
        {
            size_t newStart = 0;
            if (base == "HEADER") {
                newStart = this->HEADER.end+start;
            }  else if (base == "EXTRA") {
                newStart = this->EXTRA.end+start;
            } else if (base == "MACHINE") {
                newStart = this->MACHINE.end+start;
            } else {
                newStart = start;
            }
            return getByteFromVector(this->buffer, newStart, size);
        }

    public:
        bytesPos HEADER;
        bytesPos EXTRA;
        bytesPos MACHINE;

        DataToTable(const vector<uint8_t>& buffer, map<string, variant<int, float, string>>& table)
        : buffer(buffer), tableMap(table) {
            string find = "HEADER";
            this->HEADER = findBytes(buffer, find)[0];
            find = "EXTRA";
            this->EXTRA = findBytes(buffer, find)[0];
            find = "MACHINE";
            this->MACHINE = findBytes(buffer, find)[0];
        };


        void appendVector(const string& mapName, vector<findValue> findValues, char separator)
        {
            string res;
            short i = 0;
            for (findValue findData : findValues) {
                auto bytesVector = this->getBytesVector(findData.base, findData.start, findData.width);
                float value = joinBytes<float>(bytesVector, findData.isBigEndian);
                res += to_string(value).erase(4); // округление
                if (i != findValues.size()-1) {
                    res += separator;
                }
                ++i;
            }
            this->tableMap[mapName] = res;
            cout << "add parametr: \"" << mapName << "\": " << res << endl;
        };


        template<typename T>
        void append(const string& mapName, const string& base, size_t start, short width, bool isBigEndian) 
        {
            vector<uint8_t> bytesVector = this->getBytesVector(base, start, width);
            T value = joinBytes<T>(bytesVector, isBigEndian);
            this->tableMap[mapName] = value;
            cout << "add parametr: \"" << mapName << "\": " << value << endl;
        };
};


int main() {
    auto filepath = input<string>(string("Path to .pm3 file: "), string("path"));
    auto exelPath = input<string>(string("Path to exel file: "), string("path"));

    size_t lastSlash = filepath.find_last_of("/\\");
    string fileName = filepath.substr(lastSlash + 1);
    cout << "READING \"" << fileName << "\"" << endl;

    int size = getFileSize(filepath);
    if (size < 0) {
        cerr << "Error with open file";
        return 1;
    }
    float mbsize = static_cast<float>(size)/1024/1024;
    cout << "file size: " << size << "b / " << mbsize << " mb" << endl;
    
    vector<uint8_t> buffer = getByteFromFile(filepath, 0, size);


    map<string, variant<int, float, string>> dataToTable;
    DataToTable tableMap(buffer, dataToTable);

    // температура и смола
    int temp = input<int>(string("t(°C): "), " ");
    string resinName = input<string>(string("resin name: "), " ");
    dataToTable["temp"] = temp;
    dataToTable["resin"] = resinName;

    // Время и дата
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    std::tm* localTime = std::localtime(&currentTime);
    std::stringstream DateAndTime;
    DateAndTime << std::put_time(localTime, "%Y-%m-%d\n%H:%M:%S"); // Формат: ГГГГ-ММ-ДД ЧЧ:ММ:СС
    dataToTable["date"] = DateAndTime.str();

    // MACHINE
    auto machineV = getByteFromVector(buffer, tableMap.MACHINE.end+10, 95);
    machineV.erase(remove(machineV.begin(), machineV.end(), 0), machineV.end());
    string machineName(machineV.begin(), machineV.end());
    dataToTable["machine"] = machineName;

    tableMap.append<float>(string("thick"), string("HEADER"), 15, 4, false); // толщина слоя
    tableMap.append<float>(string("flayer"), string("0"), 88, 4, false); // кол-во первых слоев
    tableMap.append<int>(string("ftranslayer"), string("0"), 140, 4, false); // кол-во переходных слоев
    int layersCount = findBytes(buffer, get<float>(dataToTable["thick"])).size()-1; // кол-во обч. слоев
    dataToTable["layerscount"] = layersCount;
    tableMap.append<float>(string("delay"), string("0"), 80, 4, false); // задержка выкл. осн. слоев
    tableMap.append<float>(string("expo"), string("0"), 76, 4, false); // время засветки осн. слоев
    tableMap.append<float>(string("fexpo"), string("0"), 84, 4, false); // время засветки первых слоев

    vector<findValue> findValueV;

    // f lift distance
    findValueV.push_back(findValue {string("EXTRA"), 16, 4, false});
    findValueV.push_back(findValue {string("EXTRA"), 28, 4, false});
    tableMap.appendVector(string("fliftdist"), findValueV, '>');

    // f lift speed
    findValueV.clear();
    findValueV.push_back(findValue {string("EXTRA"), 20, 4, false});
    findValueV.push_back(findValue {string("EXTRA"), 32, 4, false});
    tableMap.appendVector(string("fliftspeed"), findValueV, '>');

    // f retract speed
    findValueV.clear();
    findValueV.push_back(findValue {string("EXTRA"), 36, 4, false});
    findValueV.push_back(findValue {string("EXTRA"), 24, 4, false});
    tableMap.appendVector(string("fretractspeed"), findValueV, '>');

    // lift distance
    findValueV.clear();
    findValueV.push_back(findValue {string("HEADER"), 35, 4, false});
    findValueV.push_back(findValue {string("EXTRA"), 56, 4, false});
    tableMap.appendVector(string("liftdist"), findValueV, '>');

    // lift speed
    findValueV.clear();
    findValueV.push_back(findValue {string("HEADER"), 39, 4, false});
    findValueV.push_back(findValue {string("EXTRA"), 60, 4, false});
    tableMap.appendVector(string("liftspeed"), findValueV, '>');

    // retract speed
    findValueV.clear();
    findValueV.push_back(findValue {string("EXTRA"), 64, 4, false});
    findValueV.push_back(findValue {string("EXTRA"), 52, 4, false});
    tableMap.appendVector(string("retractspeed"), findValueV, '>');

    string tableSheetName = "main";
    readCells(exelPath, tableSheetName, dataToTable);

    // char g;
    // cin >> g;
    return 0;
}


// получение размеров файла в байтах
int getFileSize(const string& filepath)
{
    ifstream file(filepath.c_str(), ios::binary | ios::ate);
    int size = file.tellg();
    return size;
}


string validPath(const string& path)
{
    string valid_filename;
    for (char s : path) {
        if (s != '"') {
            valid_filename += s;
        }
    }

    return valid_filename;
}


template<typename T>
T input(string text, string type)
{
    T res;
    cout << text;
    if constexpr (is_same_v<T, string>) {
        getline(cin, res);
        if (type == "path") {
            res = validPath(res);
        }
    } else {
        cin >> res;
    }
    cout << endl;
    return res;
}


void savePreview(const vector<uint8_t>& buffer)
{
    string find = "PREVIEW";
    vector<bytesPos> res = findBytes(buffer, find);
    if (res.size() == 0) {
        cerr << "PREVIEW not found";
        return;
    }

    vector<uint8_t> marginBytes = getByteFromVector(buffer, res[0].end+5, 4);
    int margin = joinBytes<int>(marginBytes, false);

    vector<uint8_t> previewBytes = getByteFromVector(buffer, res[0].end+19, margin);
    // Сохранение в .BMP
    saveBMP("preview.bmp", 224, 120, previewBytes);
    cout << "Preview image saved" << endl;
}






