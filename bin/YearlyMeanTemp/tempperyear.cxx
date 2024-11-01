#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <string>
#include <iomanip>
#include <TH1F.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TLegend.h>
#include <TStyle.h>


int main() {
    //opens the source data
    std::ifstream inputFile("/home/stolonen/git/MNXB11-Project-TeamB/bin/CleanerAndFilter/1970_to_2010_filtered_smhi-opendata_1_162860_20231007_155220_Lulea.csv"); 
    
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
    
    // Map to store the mean temperature for each year
    std::map<int, double> meanTemperatures;

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
         //appends the mean temperature to meanTemperatures
        meanTemperatures[year]=(meanTemp);
    }

    inputFile.close();

    // Create a histogram
    TH1F *hist = new TH1F("hist", "Mean Temperature by Year;Year;Mean Temperature (°C)", 41, 1970, 2011);
    
    // Set histogram appearance
    hist->SetLineColor(9); // Outline color
    hist->SetLineWidth(2); // Outline width
    hist->SetFillColor(7); 
    hist->SetFillStyle(3001);


    //Fill the histogram
    for (const auto& [year, meanTemp]: meanTemperatures) {
        hist->Fill(year, meanTemp);
    }

    // Create a canvas to draw the histogram
    TCanvas *c1 = new TCanvas("c1", "Mean Temperature Histogram", 800, 600);
    
    // Draw the histogram
    hist->Draw("HIST"); 

    // Create a linear fit function
    TF1 *fitFunction = new TF1("fitFunction", "pol1", 1970, 2011);
    hist->Fit(fitFunction, "R"); // "R" for range; fits within the range of the histogram
    
    // Set the color and line style for the fit function
    fitFunction->SetLineColor(2); // Red color for the fit
    fitFunction->SetLineWidth(2);
    
    // Draw the fit function on the histogram
    fitFunction->Draw("SAME"); // Draw on the same canvas
    
    // Save the histogram and the fit
    c1->SaveAs("MeanTempOverYears.png");
    std::cout << "Plot saved as: MeanTempOverYears.png" << std::endl;
}