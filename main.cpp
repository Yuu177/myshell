#include <string>
#include <unordered_set>

#include "command.hpp"
#include "util.hpp"

void DealCommand(const std::string &action, const std::string &target) {
#define EXECUTE_COMMAND(input, handle) \
  if (action == input) {               \
    handle;                            \
    return;                            \
  }

  EXECUTE_COMMAND("pwd", Pwd())
  EXECUTE_COMMAND("echo", Echo(target))
  EXECUTE_COMMAND("ls", Ls(target))
  EXECUTE_COMMAND("cd", Cd(target))
  EXECUTE_COMMAND("mkdir", Mkdir(target))
  EXECUTE_COMMAND("rm", Rm(target))
  EXECUTE_COMMAND("cat", Cat(target))
  EXECUTE_COMMAND("wc", Wc(target))
  EXECUTE_COMMAND("clear", Clear())
  EXECUTE_COMMAND("tail", Tail(target))
  EXECUTE_COMMAND("exit", Exit())

  std::cout << action << ": command not found" << std::endl;
}

// 解析命令函数
void ParseCommand(const std::string &command, std::string &action,
                  std::string &target) {
  std::istringstream iss(command);
  iss >> action;
  iss >> target;
}

int main() {
  std::cout << "Welcome to my shell" << std::endl;
  while (1) {
    printf("\033[1;3;31m%s@%s\033[m:\033[34m%s\033[m$ ", "myshell", "550W", GetPwd().c_str());

    std::string action;
    std::string target;
    std::string input_command;
    getline(std::cin, input_command);
    if (input_command == "") {
      continue;
    }
    ParseCommand(input_command, action, target);
    DealCommand(action, target);
  }
  return 0;
}
