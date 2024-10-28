#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <CLI11/CLI.hpp>
#include "date/date.h"  // Include the date library
#include <chrono>

// Function to split a string by a delimiter (used to split CSV rows)
std::vector<std::string> split_line(const std::string &line, char delimiter = ',') {
    std::vector<std::string> result;
    std::stringstream ss(line);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        result.push_back(token);
    }
    
    return result;
}

int main(int argc, char *argv[]) {
    CLI::App app{"CSV File Reader with Date Support"};

    // Command line option to accept the input file
    std::string input_file;
    app.add_option("-i,--input-file", input_file, "Input CSV file")
        ->required();

    // Parse the command line arguments
    CLI11_PARSE(app, argc, argv);

    std::cout << "CSV file to process: " << input_file << std::endl;

    // Open the CSV file
    std::ifstream file(input_file);
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << input_file << std::endl;
        return 1;
    }

    std::cout << "File opened successfully!" << std::endl;

    // Read and output the file content line by line
    std::string line;
    int line_count = 0;

    // Read the header line first (assuming there is a header)
    if (std::getline(file, line)) {
        std::vector<std::string> header = split_line(line);
        std::cout << "Header: ";
        for (const auto &col : header) {
            std::cout << col << " ";
        }
        std::cout << std::endl;
    }

    // Use date library in the loop to print dates
    using namespace date;
    using namespace std::chrono;

    // Read each row
    while (std::getline(file, line)) {
        std::vector<std::string> row = split_line(line);
        
        // Assume CSV structure: day, year, month, ignoreme, measurement
        if (row.size() >= 5) {
            // Convert the int values into date-specific types
            date::year y{std::stoi(row[1])};
            date::month m{static_cast<unsigned>(std::stoi(row[2]))};  // Ensure month is unsigned
            date::day d{static_cast<unsigned>(std::stoi(row[0]))};    // Ensure day is unsigned

            // Create a date from the year, month, and day
            year_month_day ymd{y, m, d};

            // Get the weekday from the date
            auto weekday = date::weekday{sys_days{ymd}};

            std::cout << "Date: " << ymd << " (" << weekday << ") ";
            std::cout << "Measurement: " << row[4] << std::endl;
        }

        line_count++;
    }

    if (line_count == 0) {
        std::cout << "The file is empty or could not be read." << std::endl;
    }

    file.close();
    return 0;
}

