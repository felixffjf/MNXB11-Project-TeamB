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
#include <filesystem>  
#include <TLegend.h>
#include <TStyle.h>
#include <TLatex.h>
#include <TGraph.h>

int main(int argc, char* argv[]) {
    //check for two arguments, code-name and csv file
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1; 
    }

    // Opens input file, arg[1] is the csv path
    std::ifstream inputFile(argv[1]);

    // make sure file was opened successfully
    if (!inputFile.is_open()) {
        std::cerr << "Opening of file failed: " << argv[1] << std::endl;
        return 1; 
    }

    // added so that the cityname can be used
    std::string inputPath = argv[1];
    std::string fileName = std::filesystem::path(inputPath).stem().string();

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

        // Only append data for years 1970 to 2010
        if (year >= 1965 && year <= 2000) {
            //appends the temperature to the vector of temperatureData
            temperatureData[year].push_back(temperature);
        }
    }
    
    // Map to store the mean temperature for each year
    std::map<int, double> meanTemperatures;

    double sumMean = 0.0;
    int yearCounter = 0;  

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
        sumMean += meanTemp;
        yearCounter++;
    }

    double overallMean = 0.0;

    if (yearCounter > 0) {
        overallMean = sumMean/yearCounter;
    }

    inputFile.close();

    // Create a canvas to draw the histogram
    TCanvas *c1 = new TCanvas("c1", "Average Temperature Histogram", 800, 600);
    
    // Create a histogram
    TH1F *hist = new TH1F("hist", ("Average temperature per year in " + fileName).c_str(), yearCounter, 1965, 2001);

    // Set histogram appearance
    hist->SetStats(0);
    hist->SetLineColor(32); // Outline color
    hist->SetLineWidth(2); // Outline width
    hist->SetFillColor(31); 
    hist->SetFillStyle(3001);
    //hist->GetYaxis()->SetRangeUser(-1,overallMean+4);

    //Fill the histogram
    for (const auto& [year, meanTemp]: meanTemperatures) {
        hist->Fill(year, meanTemp);
    }

    // Draw the histogram
    hist->Draw("HIST"); 



    // Create a quadratic fit function
    TF1 *fitFunction = new TF1("fitFunction", "pol2", 1965, 2001);
    hist->Fit(fitFunction, "R"); // "R" for range; fits within the range of the histogram
    // Set the color and line style for the fit function
    fitFunction->SetLineColor(860); // Red color for the fit
    fitFunction->SetLineWidth(2);
    // Draw the fit function on the histogram
    fitFunction->Draw("SAME"); // Draw on the same canvas

    // Create a text box to display the overall mean
    std::ostringstream textbox;
    textbox << "Overall average temperature: " << std::setprecision(3)<< overallMean << " C";
    TLatex *latex = new TLatex(1980, hist->GetMaximum(), textbox.str().c_str());
    latex->SetTextColor(1); 
    latex->SetTextSize(0.03);
    latex->Draw(); 


    ////// make a moving average

    auto graph {new TGraph()};
    for(int bin = 1; bin < hist->GetNbinsX(); ++bin) {
        graph->Expand(graph->GetN() + 1, 100);
        double sumpoints = 0.0;
        sumpoints = hist->GetBinContent(bin) + hist->GetBinContent(bin+1) + hist->GetBinContent(bin+2);
        graph->SetPoint(graph->GetN(), hist->GetBinCenter(bin+1), (sumpoints/3));
    }
    graph->SetLineColor(616);
    graph->SetLineWidth(2);
    graph->Draw("SAME C");
    
    // Create a legend
    TLegend *legend = new TLegend(0.1, 0.8, 0.3, 0.9); 
    legend->AddEntry(hist, "Average temperature", "f"); 
    legend->AddEntry(fitFunction, "Quadratic fit", "l"); 
    legend->AddEntry(graph, "Moving average", "l"); 
    legend->Draw();
    
    //save plot as a png
    std::string outputFile = "YearlyMeanTemp_" + fileName + ".png";
    c1->SaveAs(outputFile.c_str());
    
    std::cout << "Plot saved as: " << outputFile << std::endl;

}