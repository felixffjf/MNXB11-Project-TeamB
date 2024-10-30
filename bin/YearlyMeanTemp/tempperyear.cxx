#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <string>
#include <iomanip>

int main() {
    //opens the source data
    std::ifstream inputFile("/home/stolonen/git/MNXB11-Project-TeamB/bin/CleanerAndFilter/1970_to_2010_filtered_smhi-opendata_1_162860_20231007_155220_Lulea.csv"); 
      //create the outputfile 
    std::ofstream outputFile("mean_temperatures.csv"); 
    
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

    //Loop over temperatureData calling first row year and the second for temp_vector).
    for (const auto& [year, temp_vector]: temperatureData) {
        // creates variable sum
        double sum = 0.0;
        // adds together all temp in vector temps
        for (double temp : temp_vector) {
            sum += temp;
        }
        // calculates the mean by dividing sum with numer of values in temp_vector
        double meanTemp = sum / temp_vector.size();
        // adds the year and mean temp into the created output file
        outputFile << year << "," << std::fixed << std::setprecision(2) << meanTemp << std::endl;
    }

    std::cout << "The calculated mean temperatures per year is found in mean_temperatures.csv" << std::endl;
}
