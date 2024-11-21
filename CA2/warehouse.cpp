#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream> 

#include "item.h"
using namespace std;
void printItems(const vector<item>& items) {
    for (const auto& i : items) {
     i.print_attribute();
    }
}
int main(int argc, char* argv[]) {
    // Check if the file name is provided
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <csv_file_path>" << endl;
        return 1;
    }

    string filePath = argv[1];
    ifstream file(filePath);

    // Check if the file opened successfully
    if (!file.is_open()) {
        cerr << "Error: Could not open file " << filePath << endl;
        return 1;
    }

    string line;
     vector<item> items_in; 
     vector<item> items_out; 
    while (getline(file, line)) {  // Read each line from the CSV
       // Parse the line
      // cout << "Processing line: " << line << endl;
        stringstream ss(line);
        string name, valueStr, numberStr, inputStr;
        
        // Read each value (assuming the CSV format is correct)
        getline(ss, name, ',');  // Read name
        getline(ss, valueStr, ',');  // Read value
        getline(ss, numberStr, ',');  // Read number
        getline(ss, inputStr, ',');  // Read input (assuming it's "output" or other)

        // Convert value and number to integers
        int value = stoi(valueStr);
        int number = stoi(numberStr);
        
        bool input = (inputStr.find("input") != std::string::npos ?true: false);  // Assuming "output" means false
        //cout<<name;
        // Create an item object and add it to the vector
        if(input)
        items_in.push_back(item(name, value, number, input));
        else
        items_out.push_back(item(name, value, number, input));
    }
  printItems(items_in);
  cout<<"hii\n";
  printItems(items_out);
    file.close();
    return 0;
}
