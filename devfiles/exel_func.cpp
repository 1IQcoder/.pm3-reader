#include "exel_func.h"
#include <string>
#include <vector>
#include <map>
#include <variant>
#include <OpenXLSX.hpp>

using namespace OpenXLSX;
using namespace std;

void readCells(string& filename, string& sheetname, map<string, variant<int, float, string>> dataToTable)
{
    OpenXLSX::XLDocument doc;
    doc.open(filename);
    auto worksheet = doc.workbook().worksheet(sheetname);

    XLCell cell;
    int row = 1;
    int col = 1;

    // Определение последней заполненной строки
    while (true) {
        ++row;
        cell = worksheet.cell(XLCellReference(row, col));
        if (row > 3 && cell.value().type() == XLValueType::Empty) {
            break;
        }
    }

    col = 1;
    while (true) {
        cell = worksheet.cell(XLCellReference(1, col));
        if (cell.value().type() == XLValueType::Empty) {
            break;
        }
        string key = cell.value().get<string>();
        if (dataToTable.find(key) != dataToTable.end()) {
            cell = worksheet.cell(XLCellReference(row, col));
            cout << "set \"" << key << "\": ";
            visit([&cell](auto&& arg) {
                cell.value() = arg;
                cout << arg << endl;
            }, dataToTable[key]);
        } else {
            cout << key << " not found" << endl;
        }
        ++col;
    }

    doc.save();
    doc.close();
    cout << "Table was written at row: " << row << endl;
}



