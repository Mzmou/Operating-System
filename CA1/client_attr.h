#pragma once
#include <string>
#include <iostream>
using namespace std;
const char *colon = ":\n";
const char *end_line = "\n";
const char *win_statement = "number of matches won: ";
const char *loss_statement = "number of matches lost: ";
const char *equal_statement = "number of matches tied: ";
char *convert_int_to_char(int number)
{
  std::string strNumber = std::to_string(number);

  char *charNumber = new char[strNumber.length() + 1];

  strcpy(charNumber, strNumber.c_str());

  return charNumber;
}
class Client_attr
{

public:
  Client_attr()
  {
    if_name_set = 0;
    won = 0;
    loss = 0;
    equal = 0;
    got_choice = 0;
    in_game = 0;
  }
  Client_attr(int fd_, char *name_)
  {

    set_fd(fd_);
    set_name(name_);
    won = 0;
    loss = 0;
    equal = 0;
    got_choice = 0;
    in_game = 0;
  }
  char *get_name() { return name; }
  void set_name(char *name_)
  {
    strcpy(name, name_);
    if_name_set = 1;
    won = 0;
    loss = 0;
    equal = 0;
    got_choice = 0;
    in_game = 0;
  }
  void set_fd(int new_fd_)
  {
    fd = new_fd_;
    won = 0;
    loss = 0;
    equal = 0;
    got_choice = 0;
    in_game = 0;
  }
  int check_set_name() { return if_name_set; }
  int check_fd(int target_fd) { return target_fd == fd; }
  int get_fd() { return fd; }
  void update_loss() { loss++; };
  void update_win() { won++; };
  void update_equal() { equal++; };
  int check_get_choice() { return got_choice; }
  void update_got_choice() { got_choice = 1; }
  void reset_choice() { got_choice = 0; }
  void set_in_game() { in_game = 1; }
  void print_status()
  {
    auto win_char_ver = convert_int_to_char(won);
    auto loss_char_ver = convert_int_to_char(loss);
    auto tie_char_ver = convert_int_to_char(equal);
    write(1, name, strlen(name));
    write(1, colon, strlen(colon));
    write(1, end_line, strlen(end_line));
    write(1, win_statement, strlen(win_statement));
    write(1, win_char_ver, strlen(win_char_ver));
    write(1, end_line, strlen(end_line));

    write(1, loss_statement, strlen(loss_statement));
    write(1, loss_char_ver, strlen(loss_char_ver));
    write(1, end_line, strlen(end_line));

    write(1, equal_statement, strlen(equal_statement));
    write(1, tie_char_ver, strlen(tie_char_ver));
    write(1, end_line, strlen(end_line));
  }

private:
  int in_game;
  int got_choice;
  char name[1024];
  int won;
  int loss;
  int equal;
  int room_number;
  int port_number;
  int fd;
  int if_name_set;
};