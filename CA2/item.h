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
    string get_name() const { return name; }
    void decrease_number(int number_) { number -= number_; }
    bool check_name(string name_) { return (name_ == name); }
    int get_value() { return value; }
    int get_number() { return number; }
    int get_price() { return value * number; }
    item(string name_, int value_, int number_, bool input_)
    {
        name = name_;
        value = value_;
        number = number_;
        input = input_;
    }
    void print_attribute() const
    {
        cout << "Item: " << name
             << ", Value: " << value
             << ", Number: " << number
             << ", Input: " << (input ? "Input" : "Output")
             << endl;
    }
};
