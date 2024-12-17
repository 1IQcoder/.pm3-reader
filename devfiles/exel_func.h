#ifndef EXEL_FUNC_H
#define EXEL_FUNC_H

#include <string>
#include <vector>
#include <variant>
#include <map>

void readCells(std::string& filename, std::string& sheetname, std::map<std::string, std::variant<int, float, std::string>> dataToTable);

#endif  // EXEL_FUNC_H
