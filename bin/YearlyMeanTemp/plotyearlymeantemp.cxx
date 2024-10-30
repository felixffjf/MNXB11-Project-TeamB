#include <fstream>
#include <sstream>
#include <TH1D.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TLegend.h>
#include <TStyle.h>

void plotyearlymeantemp() {
    // Open the CSV file
    std::ifstream infile("mean_temperatures.csv");
    std::string line;

    // Variables to store data
    int year;
    float meanTemp;

    // Create a histogram
    TH1F *hist = new TH1F("hist", "Mean Temperature by Year;Year;Mean Temperature (°C)", 41, 1970, 2011);
    
    // Set histogram appearance
    hist->SetLineColor(9); // Outline color
    hist->SetLineWidth(2); // Outline width
    hist->SetFillColor(7); 
    hist->SetFillStyle(3001);
    
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
}
