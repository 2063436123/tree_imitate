//
// Created by Peter on 2020/5/7.
//

#ifndef CLIONC___REGEX_TREE_H
#define CLIONC___REGEX_TREE_H

#include <io.h>
#include <libloaderapi.h>
#include <iostream>
#include <string>
#include <set>
#include <vector>
#include <iterator>
#include <iomanip>
#include <algorithm>
#include <map>
#include <regex>

enum CONSTANT {
    version = 1,
    path,
    limitFormat,
    fileInfo,
    help,
    onlyDir,
};

//此类处理命令行参数，以正则引擎判断参数内容
class regex_tree {
private:
    //这个类包含了<regex>的部分
    class regularEx {
    public:
        static bool judgeEx(const std::string& str, const std::string &re) {
            //使用智能指针生成一个regularEx对象(其字段包含原串和模式串)，再调用本方法判断字符串类型
            bool temp = std::regex_match(str, std::regex(re));
            return temp;
        }
    };

    static bool flags[10];

public:

    void static prompt_grammar_error() {
        std::cerr << "syntax error! Please check your parameters or use tree /? to see the help.";
        exit(EXIT_FAILURE);
    }

    static void check(const std::vector<std::string> &parameters, std::map<CONSTANT, std::string> &results) {
        for (const auto &para : parameters) {
            //1.查询版本，应该只有这一个参数
            if (regularEx::judgeEx(para, R"(--version)")) {
                flags[1] = true;
                if (parameters.size() > 1)
                    prompt_grammar_error();
                results.insert(std::make_pair(version, "true"));
                return;
            }
                //2. 指定路径，只能出现一次,若不出现则为当前路径
            else if (regularEx::judgeEx(para,
                                        R"((([CDE]:\\)?(([A-Za-z0-9_])+)(\\([A-Za-z0-9_])+)*)|([CDE]:\\))")) {
                if (flags[2] == 0)
                    flags[2] = true;
                else prompt_grammar_error();
                results.insert(std::make_pair(path, para));
            }
                //3.限制文件的格式,只能出现一次
            else if (regularEx::judgeEx(para, R"((\.[a-z]+)|(\.[a-z]+)(\-\.[a-z]+)+)")) {
                if (flags[3] == 0)
                    flags[3] = true;
                else prompt_grammar_error();
                results.insert(std::make_pair(limitFormat, para));
            }
                //4.显示某个文件的信息，只能有这一个参数，需要查询过该文件所在的目录，查询结果(文件列表及路径)应保存在缓存文件中
            else if (regularEx::judgeEx(para, R"([A-Za-z0-9_]+\.[A-Za-z0-9_]+)")) {
                flags[4] = true;
                if (parameters.size() > 1)
                    prompt_grammar_error();
                results.insert(std::make_pair(fileInfo, para));
            }
            //5.显示帮助文档
            else if (para == "/?"){
                flags[5] = true;
                if (parameters.size() > 1)
                    prompt_grammar_error();
                results.insert(std::make_pair(help,"true"));
            }else if(para == "-s"){
                flags[6] = true;
                results.insert(std::make_pair(onlyDir,"true"));
            }
            else {
                prompt_grammar_error();
            }
        }
    }

};

typedef struct {
    _finddata_t file_info;
    char file_path[255];    //这里千万不能用string,会出错
}AFileInfo;

#endif //CLIONC___REGEX_TREE_H
