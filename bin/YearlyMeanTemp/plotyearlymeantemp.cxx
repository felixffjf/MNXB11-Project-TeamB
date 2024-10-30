#include <fstream>
#include <sstream>
#include <TH1D.h>
#include <TCanvas.h>

void plotyearlymeantemp() {
    // Open the CSV file
    std::ifstream infile("mean_temperatures.csv");
    std::string line;

    // Variables to store data
    int year;
    float meanTemp;

    // Create a histogram
    TH1F *hist = new TH1F("hist", "Mean Temperature by Year;Year;Mean Temperature (degrees)", 42, 1969, 2011);

    hist->SetLineWidth(1);
    // Read the data from the CSV file
    while (std::getline(infile, line)) {
        std::stringstream ss(line);
        ss >> year;
        ss.ignore(1, ',');  // Ignore the comma
        ss >> meanTemp;
        // Fill the histogram
        hist->Fill(year, meanTemp);
    }

    // Create a canvas to draw the histogram
    TCanvas *c1 = new TCanvas("c1", "Mean Temperature Histogram", 800, 600);
    hist->Draw();

    // Save the histogram
    c1->SaveAs("MeanTempOverYears.png");
}
