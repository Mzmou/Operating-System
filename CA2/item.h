#include <stdio.h>
#include <iostream>
using namespace std;
class item
{
private:
    string name;
    float value;
    int number;
    bool input;
    int price;
    int leftover;

public:
    string get_name() const { return name; }
    void decrease_number(int number_) { number -= number_; }
    bool check_name(string name_) { return (name_ == name); }
    float get_value() { return value; }
    int get_number() { return number; }
    float get_price() { return value * float(number); }
    item(string name_, float value_, int number_, bool input_)
    {
        name = name_;
        value = value_;
        number = number_;
        input = input_;
    }
    item(string name_, float price_, int leftover_)
    {
        name = name_;
        price = price_;
        leftover = leftover_;
    }
    void print_attribute_end()
    {
        cout << "Item: " << name
             << ", price: " << price
             << ", leftover: " << leftover << '\n';
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
