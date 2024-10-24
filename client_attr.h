#pragma once
#include <string>
#include <iostream>
using namespace std;

class Client_attr{


    public:
    Client_attr(string name_){
        string last_two=name_.substr(name_.length()-2,2);
        //cout<<last_two[0];
        // if(last_two[0]=='t' && last_two[1]=='\n'){
        //     name_=name_.substr(0,name_.length()-2);
        // }
        cout<<name_;
        name=name_;
    }
    void get_name(){cout<<name;}
    
    



    private:
    string name;
    int won;
    int loss;
    int equal;
    int room_number;
    int port_number;
};