#include <stdio.h>
#include <iostream>
using namespace std;
class item
{
private:
    string name;
    int value;
    int number;
    bool input;

    /* data */
public:
    item(string name_,int value_,int number_,bool input_){
        name=name_;
        value=value_;
        number=number_;
        input=input_;
    }
   void print_attribute() const{
         cout << "Item: " << name 
                  << ", Value: " << value
                  << ", Number: " << number
                  << ", Input: " << (input ? "Input" : "Output") 
                  << endl;
   }
};

