//
// Created by cher on 08/08/2026.
//

#include "writeFile.h"
#include <iostream>
#include <fstream>

using namespace std;

#define TESTFILEPATH "sdmc:/TestFile.txt"

bool writeFileDebug()
{
    string result;
    ofstream TestFile(TESTFILEPATH);
    ifstream ReadFile("romfs:/nextendo/testfile.txt");
    getline(ReadFile,result);
    TestFile << result;
    TestFile.close();
    return true;
}
