#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <filesystem>  
#include "TCanvas.h"
#include "TGraph.h"
#include "TAxis.h"
#include "TF1.h"  

int main(int argc, char* argv[]) {
    // Check for command-line arguments
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <path-to-csv-file>" << std::endl;
        return 1;
    }

    // Open the CSV file specified as the first argument
    std::ifstream infile(argv[1]);
    if (!infile.is_open()) {
        std::cerr << "Error opening file: " << argv[1] << std::endl;
        return 1;
    }

    // Extract file name for the output PNG file and title
    std::string inputPath = argv[1];
    std::string fileName = std::filesystem::path(inputPath).stem().string();
    std::string outputFileName = "YearlyTemperatureDifference_" + fileName + ".png";

    // Data structure to store max and min temperatures
    std::map<int, std::pair<double, double>> yearlyTemps;

    std::string line;
    while (getline(infile, line)) {
        std::istringstream ss(line);
        std::string date, time;
        double temperature;

        // Extract date, time, and temperature 
        if (getline(ss, date, ',') && getline(ss, time, ',') && ss >> temperature) {
            int year = std::stoi(date.substr(0, 4));

            // Only consider data between 1950 and 2023
            if (year >= 1950 && year <= 2023) {
                // Initialize or update max/min temperatures for the year
                if (yearlyTemps.find(year) == yearlyTemps.end()) {
                    yearlyTemps[year] = {temperature, temperature};
                } else {
                    yearlyTemps[year].first = std::max(yearlyTemps[year].first, temperature);
                    yearlyTemps[year].second = std::min(yearlyTemps[year].second, temperature);
                }
            }
        }
    }

    infile.close();


    // Prepare data for plotting
    std::vector<double> years, tempDiffs;
    double minTempDiff = std::numeric_limits<double>::max();
    double maxTempDiff = std::numeric_limits<double>::lowest();

    for (const auto& entry : yearlyTemps) {
        years.push_back(entry.first);
        double tempDiff = entry.second.first - entry.second.second;
        tempDiffs.push_back(tempDiff);
        minTempDiff = std::min(minTempDiff, tempDiff);
        maxTempDiff = std::max(maxTempDiff, tempDiff);
    }

    // Create the title string
    std::string title = "Yearly Temperature Difference " + fileName + " 1950-2023";

    // Plot the temperature difference
    TCanvas *c1 = new TCanvas("c1", title.c_str(), 800, 600);
    TGraph *graph = new TGraph(years.size(), &years[0], &tempDiffs[0]);

    // Set titles and styles
    graph->SetTitle((title + ";Year;Temperature Difference (°C)").c_str());
    graph->SetMarkerStyle(20);
    graph->SetLineWidth(2);
    graph->SetMarkerSize(0.8);
    graph->SetLineColor(kBlue + 2);
    graph->SetMarkerColor(kRed + 1);
    graph->Draw("APL");

    
    graph->GetXaxis()->SetTitle("Year");
    graph->GetXaxis()->SetRangeUser(1950, 2023);
    graph->GetYaxis()->SetTitle("Temperature Difference (°C)");
    graph->GetXaxis()->CenterTitle();
    graph->GetYaxis()->CenterTitle();
    graph->GetYaxis()->SetRangeUser(minTempDiff - 5, maxTempDiff + 5);  // Dynamically set the y-axis range

    // Add a trendline (linear fit)
    TF1 *trendline = new TF1("trendline", "pol1", 1950, 2023);  // "pol1" is a linear polynomial (y = ax + b)
    graph->Fit(trendline, "R");  // "R" restricts the fit to the range of the data
    trendline->SetLineColor(kGreen + 1);  // Set trendline color
    trendline->SetLineWidth(2);           // Set trendline width

    // Save plot to PNG with unique filename
    c1->SaveAs(outputFileName.c_str());

    std::cout << "Plot saved to: " << outputFileName << std::endl;

    delete c1;  // Clean up canvas

    return 0;
}
