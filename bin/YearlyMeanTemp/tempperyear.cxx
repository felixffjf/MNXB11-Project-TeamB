#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <string>
#include <iomanip>

int main() {
    //opens the source data
    std::ifstream inputFile("/home/stolonen/git/MNXB11-Project-TeamB/bin/CleanerAndFilter/1990_to_2000_filtered_smhi-opendata_1_162860_20231007_155220_Lulea.csv"); 
    
    //make variable line as a string to store each row
    std::string line; 

    //make variable temperatureData, int of map is year and vector is array of temperatures
    std::map<int, std::vector<double>> temperatureData;  

    //Loop gets line from input file and stores in line for all rows in the inputfile
    while (std::getline(inputFile, line)) {
        //makes string into stream
        std::istringstream ss(line);
        //make variables date,time and temp
        std::string date, time, temp;
        // get the date, time and temp from the line 
        std::getline(ss, date, ',');
        std::getline(ss, time, ',');
        std::getline(ss, temp, ',');
        // Get year first 4 char of data aka year
        int year = std::stoi(date.substr(0, 4)); 
        //converts the temp string to a double
        double temperature = std::stod(temp);
        //appends the temperature to the vector of temperatureData
        temperatureData[year].push_back(temperature);
    }

}
