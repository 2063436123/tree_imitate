//
// Created by Peter on 2020/5/6.
//

#include "regex_tree.h"
#include "file_tree.h"

void showFileAll(std::string, std::set<std::string>, int, bool);

void indentHelper(int);

void processArgu(int, char **, std::map<CONSTANT, std::string> &);

void set_postfix(std::set<std::string> &, std::map<CONSTANT, std::string>);

inline std::string str_to_string(char *str) {
    return std::string(str);
}

/**
 * 想要实现一个tree工具，可以分析当前文件夹下的目录结构并显示出来
 * 希望能够实现：
 * tree --version 显示版本，版权信息
 * tree [路径] 显示当前[指定]路径下的所有文件夹和文件 [c++自带正则库不支持中文字符，因此不支持指定路径中含有中文以后引入boost+ICU解决]
 * tree .jpg 只显示指定格式的文件 [使用-连接]
 * tree 某个文件名.后缀，显示其信息 [waiting...]
 * tree /? 显示帮助 [waiting...]
 * tree -s 只显示文件夹
 * 要尽可能多的使用c++高级语法
 * */
bool regex_tree::flags[10] = {
        //在main前初始化static成员
        0,
};
std::fstream file_tree::finout = std::fstream();//[不知道怎么初始化]
std::ifstream file_tree::fin = std::ifstream();

int main(int argc, char *argv[]) {
    using namespace std;
    system("@echo off\nCHCP 65001");
    // 解析命令行参数 √
    map<CONSTANT, string> results;    //指定功能的常数对应的string明确阐释了此功能的所需信息
    if (argc != 1)      // 有参数再解析
        processArgu(argc, argv, results); //现在results里存储了经过正则验证的参数值和对应的enum值

    // 解析结果应用：CONSTANT.limitFormat √
    set<string> postfix;           //存储文件后缀格式限制列表
    set_postfix(postfix, results); //用result[2]设置文件后缀格式限制列表

    // 解析结果应用：CONSTANT.path √
    char str_path[255] = "";
    if (!results[limitFormat].empty() && argc == 2 || argc == 1 || !results[onlyDir].empty() && argc == 2) {  //if tree参数为空或tree + limitFormat或tree + -s
        GetModuleFileName(nullptr, str_path, 255); // api，显示当前exe所在的文件夹路径
        string strTemp(str_path);
        strcpy(str_path, strTemp.substr(0, strTemp.find_last_of('\\')).c_str());
    } else  //if 有path
        strcpy(str_path, results[path].c_str()); //正常情况，将路径赋给str_path

    // 解析结果应用：CONSTANT.version √
    if (results[version] == "true")
        cout << "\nTree 1.0\n©2004 Adobe Systems Incorporated. All rights reserved. \n"
                "目前只支持基础tree功能，不能限制显示层级，建议不要 tree C:\\ 输出量惊人\n"
                "文件信息现实的是原始信息，还没有解读\n"
             << endl;

    // 解析结果应用：CONSTANT.help √
    if (results[help] == "true")
        cout << "\nMAN:\tHow to use tree?\n"
             << "tree [path] [limitFormat] [-s]\t显示指定文件的树图\n"
                "tree [fileName]\t\t\t\t显示文件信息\n"
                "tree --version\t\t\t\t显示版本信息\n"
                "tree /?\t\t\t\t显示帮助文档\n"
                "\n"
                "path 指定路径，不指定则为tree.exe所在路径,不能含有-\n"
                "limitFormat 限定要显示的文件格式，以-分隔 如.jpg-.java\n"
                "-s 只显示文件夹"
                "fileName 需要文件的全名，此文件应该已被tree显示过,不能含有-\n"
                "\n"
                "默认不显示隐藏文件，不支持path中含有中文，不支持limitFormat中含有大写字母,但可以显示中文目录和文件名" << endl;

    // 解析结果应用：CONSTANT.fileInfo [有待样式美化]
    if (results.find(fileInfo) != results.end()) {
        auto file_data = file_tree::showFileInfo(results[fileInfo]);
        if (strcmp(file_data->file_path, "") == 0) {
            cerr << "can't find file!" << endl;
            exit(EXIT_FAILURE);
        }
        cout << "The information of file " << file_data->file_info.name << " :\n";
        cout <<"属性: "<< file_data->file_info.attrib << "\t" <<"大小: "<< file_data->file_info.size << "KB\t"
                <<"创建日期: "<< file_data->file_info.time_create << "\t" <<"修改日期: "<< file_data->file_info.time_write << "\t" << endl;
        exit(EXIT_SUCCESS);
    }

    bool showFile = true;
    // 解析结果应用：CONSTANT.onlyDir √
    if (results[onlyDir] == "true")
        showFile = false;

    //str_path不为空,说明有path参数的传入或者无任何参数
    if (strlen(str_path) != 0)
        showFileAll(str_path, postfix, 0, showFile); //levels表示层级，用于控制缩进


    return 0;
}

void set_postfix(std::set<std::string> &postfix, std::map<CONSTANT, std::string> results) {
    if (results.find(limitFormat) == results.end()) {
        return;
    }
    std::string limit_list = results[limitFormat];
    while (int pos = limit_list.find('-')) {
        postfix.insert(limit_list.substr(0, pos)); //参数pos是要复制的长度
        if (pos == std::string::npos)
            break;
        limit_list.assign(limit_list.substr(pos + 1)); //加1，跳过这个|
    }
    return;
}

void processArgu(int len, char **parameters, std::map<CONSTANT, std::string> &results) {
    std::vector<std::string> parameters_vector;
    std::transform(parameters + 1, parameters + len,
                   std::insert_iterator<std::vector<std::string>>(parameters_vector, parameters_vector.begin()),
                   str_to_string);
    regex_tree::check(parameters_vector, results);
    /*
    测试：vector中是否含有完全的参数
    std::cout << "The parameters in vector:\n";
    copy(parameters_vector.begin(), parameters_vector.end(), std::ostream_iterator<std::string>(std::cout," "));*/
}

void indentHelper(int nums) { // 输出指定的空格 [样式尚需完善]
    if (nums < 0) {
        throw std::out_of_range("Nums can't < 0! ---- In function indent()");
        exit(EXIT_FAILURE);
    }
    if (nums == 0)
        return;

    //nums > 1:先输出nums-1个tab，再输出--
    std::cout << std::setw(4 * (nums - 1)) << "";
    std::ostream_iterator<char> iout(std::cout);
    std::string temp(3, '-');
    temp += " ";
    std::copy(temp.begin(), temp.end(), iout);
}

void showFileAll(std::string path, std::set<std::string> postfix, int levels, bool showFile) {

    struct _finddata_t fileinfo{}; // 存储文件信息
    long ld;
    // 读取目录结构
    if ((ld = _findfirst((path + "\\*").c_str(), &fileinfo)) != -1) { // BUG:不能用&&将ld!=-1和_findfirst分开判断!
        do {

            if (!(fileinfo.attrib & _A_HIDDEN) && fileinfo.attrib & _A_SUBDIR) { // 文件夹
                if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) {
                    indentHelper(levels);
                    std::cout << fileinfo.name << std::endl;
                    std::string temp = std::string(path).append("\\").append(
                            fileinfo.name); // BUG: 用path.append()会修改path！
                    showFileAll(temp, postfix, levels + 1, showFile);
                }
            } else if (!(fileinfo.attrib & _A_HIDDEN) && showFile) { // 文件
                char *x = strchr(fileinfo.name, '.');
                if (x == nullptr)
                    x = fileinfo.name;
                if (postfix.empty() ||
                    !postfix.empty() && postfix.find(x) != postfix.end()) {
                    indentHelper(levels);
                    std::cout << fileinfo.name << std::endl;
                    //记录每个显示过的文件
                    file_tree::addFileInfo(fileinfo, path);
                }
            }

        } while (_findnext(ld, &fileinfo) == 0);
        _findclose(ld);
    } else {
        std::cerr << "Directory not found!";
        exit(EXIT_FAILURE);
    }
}
