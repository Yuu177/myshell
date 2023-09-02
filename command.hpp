#ifndef COMMAND
#define COMMAND

#include <dirent.h>
#include <sys/stat.h>

#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "util.hpp"

inline void Pwd() { std::cout << GetPwd() << std::endl; }

inline void Echo(const std::string& target) {
  std::cout << target << std::endl;
}

inline void Ls(std::string path) {
  if (path == "") {
    path = GetPwd();
  }
  DIR* directory = opendir(path.c_str());
  if (!directory) {
    std::cerr << "无法打开目录: " << strerror(errno) << std::endl;
    return;
  }

  struct dirent* entry;
  int count = 0;
  while ((entry = readdir(directory))) {
    std::cout << entry->d_name << "\t";  // 使用制表符分隔目录项
    count++;
    if (count >= 5) {
      std::cout << std::endl;  // 每五个目录项换行
      count = 0;
    }
  }
  if (count != 0) {
    std::cout << std::endl;  // 如果目录项总数不是5的倍数，则换行
  }

  closedir(directory);
}

inline void Cd(const std::string& path) {
  if (chdir(path.c_str()) != 0) {
    std::cerr << "无法进入目录：" << path << std::endl;
  }
}

inline void Mkdir(const std::string& directory_name) {
  if (mkdir(directory_name.c_str(), 0777) == 0) {
    std::cout << "成功创建目录：" << directory_name << std::endl;
  } else {
    std::cerr << "无法创建目录：" << directory_name << std::endl;
  }
}

inline void Rm(const std::string& file_name) {
  if (remove(file_name.c_str()) == 0) {
    std::cout << "成功删除文件：" << file_name << std::endl;
  } else {
    std::cerr << "无法删除文件：" << file_name << std::endl;
  }
}

inline void Cat(std::string file_name) {
  std::ifstream file(file_name);

  if (file.is_open()) {
    std::string line;
    while (getline(file, line)) {
      std::cout << line << std::endl;
    }
    file.close();
  } else {
    std::cerr << "无法打开文件：" << file_name << std::endl;
  }
}

inline void Wc(const std::string& file_name) {
  std::ifstream file(file_name);

  if (!file.is_open()) {
    std::cerr << "无法打开文件：" << file_name << std::endl;
    return;
  }

  int line_count = 0;
  int word_count = 0;
  int char_count = 0;

  std::string line;
  while (getline(file, line)) {
    line_count++;

    std::stringstream ss(line);
    std::string word;
    while (ss >> word) {
      word_count++;
      char_count += word.length();
    }
    char_count++;  // 加上行末的换行符
  }

  file.close();

  std::cout << "行数: " << line_count << std::endl;
  std::cout << "单词数: " << word_count << std::endl;
  std::cout << "字符数: " << char_count << std::endl;
}

inline void Clear() {
  printf("\033[2J");
  printf("\033[H");
}

// 从文件相对于结尾的位置向后读取并打印
inline void Tail(const std::string& file_name) {
  std::ifstream file(file_name);

  if (!file.is_open()) {
    std::cerr << "无法打开文件：" << file_name << std::endl;
    return;
  }

  // 获取文件大小
  file.seekg(0, std::ios::end);
  int file_size = file.tellg();

  // 计算相对于文件结尾的位置
  const int offset_from_end = 1024;
  int start_position = std::max(0, file_size - offset_from_end);

  // 定位到计算后的位置
  file.seekg(start_position);

  std::vector<std::string> lines;
  std::string line;

  // 读取文件从计算后的位置开始的内容
  while (getline(file, line)) {
    lines.push_back(line);
  }

  file.close();

  // 打印文件从计算后的位置开始的内容
  for (const std::string& line : lines) {
    std::cout << line << std::endl;
  }
}

inline void Exit() {
  exit(0);
}

#endif  // COMMAND
