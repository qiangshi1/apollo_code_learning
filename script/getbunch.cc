#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <cstddef>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <regex>

std::vector<std::regex> ignores_files_regex;

bool isIgnored(std::string a_str)
{
    if (ignores_files_regex.size() == 0)
        return false;
    for (int i = 0; i < ignores_files_regex.size(); i++)
    {
        if (std::regex_match(a_str, ignores_files_regex.at(i)))
        {
            return true;
        }
    }
    return false;
}

void getFiles(std::string path, std::vector<std::string> &files)
{
    DIR *dir;
    struct dirent *ptr;

    if ((dir = opendir(path.c_str())) == NULL)
    {
        perror("Open dir error...");
        exit(1);
    }

    while ((ptr = readdir(dir)) != NULL)
    {
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)
            continue;
        else if (isIgnored(ptr->d_name))
            continue;
        else if (ptr->d_type == 8)
            files.push_back(path + ptr->d_name);
        else if (ptr->d_type == 10)
            continue;
        else if (ptr->d_type == 4)
        {
            getFiles(path + ptr->d_name + "/", files);
        }
    }
    closedir(dir);
}

int main(int argc, char *argv[])
{
    std::string path = "./";
    if (argc >= 2)
    {
        path = std::string(argv[1]);
    }
    // path 要查找的根目录
    std::vector<std::string> ignores_files = {"script", "\\..*", "bunch.md", "README.md", "file_index.md"}; //正则方式匹配
    for (int i = 0; i < ignores_files.size(); i++)
    {
        ignores_files_regex.push_back(std::regex(ignores_files.at(i)));
    }
    //ignores_files 要忽略的文件的正则规则
    std::vector<std::string> files;
    getFiles(path, files);

    // for (int i = 0; i < files.size(); i++)
    // {
    //     std::cout << files.at(i) << std::endl;
    // }

    std::string output_filename = "bunch.md";
    if (argc >= 3)
    {
        output_filename = std::string(argv[2]);
    }
    std::ofstream out(output_filename,std::ios::out);
    if (!out.is_open())
    {
        std::cout << "error open file " << output_filename << std::endl;
        exit(EXIT_FAILURE);
    }
    std::string file_index = "file_index.md";
    std::ofstream out2(file_index,std::ios::out);
    if (!out2.is_open())
    {
        std::cout << "error open file " << file_index << std::endl;
        exit(EXIT_FAILURE);
    }


    for (int i = 0; i < files.size(); i++)
    {
        std::string source_filename = files.at(i);
        int li = source_filename.size() - 1;
        // 只允许.md, 不允许.markdown
        if (source_filename.at(li) != 'd' || source_filename.at(li - 1) != 'm' || source_filename.at(li - 2) != '.')
        {
            continue;
        }
        std::ifstream in(source_filename);
        if (!in.is_open())
        {
            std::cout << "error open file "
                      << "./getbunch.sh" << std::endl;
            exit(EXIT_FAILURE);
        }
        // if (src == dst) {
        //     cout << "the src file can't be same with dst file" << endl;
        //     exit(EXIT_FAILURE);
        // }
        std::string added;
        added=added+"\n# "+"["+source_filename+"]("+source_filename+")\n\n";
        std::string added2;
        added2=added2+"["+source_filename+"]("+source_filename+")\n";
        out<<added;
        out2<<added2;

        char buf[2048];
        long long totalBytes = 0;
        while (in)
        {
            //read从in流中读取2048字节，放入buf数组中，同时文件指针向后移动2048字节
            //若不足2048字节遇到文件结尾，则以实际提取字节读取。
            
            in.read(buf, 2048);
            //gcount()用来提取读取的字节数，write将buf中的内容写入out流。
            out.write(buf, in.gcount());
            totalBytes += in.gcount();
        }
        // out<<in;
        in.close();
    }
    out.close();
    out2.close();

    return 0;
}