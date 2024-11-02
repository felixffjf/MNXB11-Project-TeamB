#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <limits>
#include "TCanvas.h"
#include "TGraph.h"
#include "TAxis.h"
#include "TLegend.h"
#include "TF1.h"

int main(int argc, char* argv[]) {
    // Check if input file is provided
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <path-to-csv-file>" << std::endl;
        return 1;
    }

    // Open the CSV file
    std::ifstream infile(argv[1]);
    if (!infile.is_open()) {
        std::cerr << "Error opening file: " << argv[1] << std::endl;
        return 1;
    }

    // Extract file name for title and output PNG
    std::string inputPath = argv[1];
    std::string fileName = std::filesystem::path(inputPath).stem().string();
    std::string outputFileName = "June20_TemperatureExtremes_" + fileName + ".png";

    // Map to store the max and min temperatures on June 20 for each year
    std::map<int, std::pair<double, double>> june20Temps;

    std::string line;
    while (getline(infile, line)) {
        std::istringstream ss(line);
        std::string date, time;
        double temperature;

        // Read date, time, and temperature
        if (getline(ss, date, ',') && getline(ss, time, ',') && ss >> temperature) {
            int year = std::stoi(date.substr(0, 4));
            std::string month_day = date.substr(5, 5); // Extract MM-DD

            // Filter for June 20 data between 1950 and 2023
            if (year >= 1950 && year <= 2023 && month_day == "06-20") {
                // Initialize or update max/min temperatures for June 20
                if (june20Temps.find(year) == june20Temps.end()) {
                    june20Temps[year] = {temperature, temperature}; // Initialize
                } else {
                    june20Temps[year].first = std::max(june20Temps[year].first, temperature); // Update max
                    june20Temps[year].second = std::min(june20Temps[year].second, temperature); // Update min
                }
            }
        }
    }

    infile.close();

    // Prepare data for plotting
    std::vector<double> years, maxTemps, minTemps, avgTemps;
    for (const auto& entry : june20Temps) {
        years.push_back(entry.first);
        maxTemps.push_back(entry.second.first);  // Highest temperature
        minTemps.push_back(entry.second.second); // Lowest temperature

        // Calculate average temperature
        double meanTemp = (entry.second.first + entry.second.second) / 2.0;
        avgTemps.push_back(meanTemp);
    }

    // Create title for the plot
    std::string title = "Highest, Lowest, and Average Temperatures on June 20 (" + fileName + ") 1950-2023";

    // Create a canvas for plotting
    TCanvas *c1 = new TCanvas("c1", title.c_str(), 800, 600);
    c1->SetGrid(); // Enable grid for better visibility
    
    // Set Y-axis range
    c1->SetLogy(0); // Ensure log scale is not set
    c1->DrawFrame(1950, 0, 2023, 35); // Set frame for the canvas

    // Plot maximum temperatures
    TGraph *graphMax = new TGraph(years.size(), &years[0], &maxTemps[0]);
    graphMax->SetTitle((title + ";Year;Temperature (°C)").c_str());
    graphMax->SetMarkerStyle(20);
    graphMax->SetMarkerSize(0.8);
    graphMax->SetLineColor(kGray + 2); // Light gray for line color
    graphMax->SetMarkerColor(kGray + 2); // Light gray for marker color
    graphMax->SetMarkerColorAlpha(kGray + 2, 0.5); // Set opacity
    graphMax->Draw("APL");

    // Plot minimum temperatures
    TGraph *graphMin = new TGraph(years.size(), &years[0], &minTemps[0]);
    graphMin->SetMarkerStyle(20);
    graphMin->SetMarkerSize(0.8);
    graphMin->SetLineColor(kGray + 2); // Light gray for line color
    graphMin->SetMarkerColor(kGray + 2); // Light gray for marker color
    graphMin->SetMarkerColorAlpha(kGray + 2, 0.5); // Set opacity
    graphMin->Draw("PL SAME");

    // Plot average temperatures
    TGraph *graphAvg = new TGraph(years.size(), &years[0], &avgTemps[0]);
    graphAvg->SetMarkerStyle(20);
    graphAvg->SetMarkerSize(0.8);
    graphAvg->SetLineColor(kBlue);
    graphAvg->SetMarkerColor(kBlue);
    graphAvg->Draw("PL SAME");

    // Add a fit to the average temperature data
    TF1 *fitFunc = new TF1("fitFunc", "pol1", 1950, 2023);
    graphAvg->Fit(fitFunc, "R"); // Fit to the average temperatures

    // Customize axes
    graphMax->GetXaxis()->SetTitle("Year");
    graphMax->GetYaxis()->SetTitle("Temperature (°C)");
    graphMax->GetXaxis()->CenterTitle();
    graphMax->GetYaxis()->CenterTitle();
    graphMax->GetYaxis()->SetRangeUser(0, 35); // Set Y-axis range

    // Add legend
    TLegend *legend = new TLegend(0.1, 0.8, 0.3, 0.9);
    legend->AddEntry(graphMax, "Highest Temperature on June 20", "l");
    legend->AddEntry(graphMin, "Lowest Temperature on June 20", "l");
    legend->AddEntry(graphAvg, "Average Temperature on June 20", "l");
    legend->AddEntry(fitFunc, "Linear Fit to Average Temperature", "l");
    legend->Draw();

    // Save the plot as a PNG file
    c1->SaveAs(outputFileName.c_str());

    std::cout << "Plot saved as: " << outputFileName << std::endl;

    // Clean up
    delete c1;
    delete graphMax;
    delete graphMin;
    delete graphAvg;
    delete fitFunc;

    return 0;
}
