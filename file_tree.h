//
// Created by Peter on 2020/5/7.
//

#ifndef CLIONC___FILE_TREE_H
#define CLIONC___FILE_TREE_H

#include "regex_tree.h"
#include <fstream>
#include <memory>
#include <utility>

const char *PATH = "E:\\tree.ini";

bool operator==(AFileInfo a, AFileInfo b) {
    return a.file_info.name == b.file_info.name;
}

class file_tree {
private:
    static std::fstream finout;
    static std::ifstream fin;

    static void open_file(const std::string &dataPath) {
        //读写二进制文件
        finout.open(dataPath, std::ios_base::out | std::ios_base::in | std::ios_base::binary);
        if (!finout.is_open()) {
            std::cerr << "file can not open!" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    static void open_file_only_read(const std::string &dataPath) {
        //读二进制文件
        fin.open(dataPath, std::ios_base::in | std::ios_base::binary);
        if (!fin.is_open()) {
            std::cerr << "file can not open!" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

public:
    static std::unique_ptr<AFileInfo> showFileInfo(const std::string &name) {
        open_file_only_read(PATH);  //打开缓存文件，准备读数据
        AFileInfo info{};
        //*info = AFileInfo {};//初始化？

        while (fin.read((char *)&info, sizeof(info))) { //BUG:不要先声明智能指针，这会导致解引用上的麻烦，返回时再声明可以减少工作量
            if (strcmp(info.file_info.name, name.c_str()) == 0) {  //就是它了
                break;
            }
        }

        fin.close();
        return std::make_unique<AFileInfo> (info);
    }

    static void addFileInfo(_finddata_t file_info, const std::string &path) {
        open_file(PATH);    //打开缓存文件，准备读写数据
        AFileInfo one_data{file_info,};
        strcpy(one_data.file_path,path.c_str());
        AFileInfo temp;

        bool has_rewrite = false;
        while (finout.read((char *) &temp, sizeof(temp))) {
            if (temp == one_data) { //覆写
                finout.clear();
                finout.seekp(-sizeof(temp), std::ios_base::cur);
                finout.write((char *) &one_data, sizeof(one_data));
                has_rewrite = true;
                break;
            }
        }

        if (!has_rewrite) {
            finout.clear();
            finout.write((char *) &one_data, sizeof(one_data));
        }
        if (finout.eof()) {
            finout.clear();
        }

        finout.close();
    }
};


#endif //CLIONC___FILE_TREE_H
