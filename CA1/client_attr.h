#pragma once
#include <string>
#include <iostream>
using namespace std;

class Client_attr
{

public:
  Client_attr() { if_name_set = 0; }
  void get_name() { cout << name; }
  void set_name(char *name_)
  {
    strcpy(name, name_);
    if_name_set = 1;
  }
  void set_fd(int new_fd_) { fd = new_fd_; }
  int check_set_name() { return if_name_set; }
  int check_fd(int target_fd) { return target_fd == fd; }
  int get_fd() { return fd; }

private:
  char name[1024];
  int won;
  int loss;
  int equal;
  int room_number;
  int port_number;
  int fd;
  int if_name_set;
};