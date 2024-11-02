# MNXB11 Temperature Analysis Project

This project has been made as a part of the course MNXB11. The goal of the project is to use all the knowladge of GitHub and C++ together with Bash and Root as out filtering and graphing tools respectfully.

In order to get started a few libraries need to be added to the repository. To enable the libraries create a directory "external" and then a directory "include" within that directory. Then clone the following into external/include/
"https://github.com/ben-strasser/fast-cpp-csv-parser.git" 
"https://github.com/CLIUtils/CLI11.git" 
"https://github.com/HowardHinnant/date/tree/master" (Only include date/include/date directory)

This project has been made with "user-friendliness" in mind, meaning it was made as a well rounded package. It is very simple to use, the only command needed to run is

```console
make
```

This will export the tarball `datasets.tgz` from the folder `/datasets`. It will initialize the cleaning and filtering of the dataset `smhicleaner.sh` and `filter1950-2023.sh` which are both located in `/src/CleanerAndFilter`. This will create the final dataset which will be used in the C++ code.

This project is comprised of 3 indivitual 'experiments' with 2 different datasets, one for Lund and one for Luleå. If the user needs more datasets, follow the [README.md](/datasets/README.md) file in the `/datasets` folder.
The experiments are as follows:
 - Yearly mean temeprature
 - Yearly temperature difference
 - Yearly highest, lowest and average temperatures on June 20th (Midsommar)

The sourcecode for each of the individual experiements can be found in `/src/<experiment name>`. The MakeFile is made such that it will run all the filtering and all the experiments, finally eporting the individual plots. This is the ease of use previously mentioned! 

Adding new projects is also very easy and intuitive! Just make a new direcotry in the `/src` folder. Write the code in C++ for your desired experiment. In the MakeFile, add a few lines of code in order for everything to be executed, and then just enjoy the final plot!

The makers of this project can be found in [AUTHORS](AUTHORS). All the work done in the project can be seen in the [WorkPlan](Workplan.md) as well as the logs in the `/logs` folder.

## How to add your own project

In this section we will go through the [Makefile](Makefile) and how to set up you own project.

### Step 1: Set up your environment

This project has been made in the course Apptainer which already had ROOT and all the necceary tools except the libraries mentioned in the begining of the file. 

### Step 2: Set up your directory

In the folder `/src` make a new directory which will house your C++ code. Give it a descriptive name.
In the newly made directory, make a .cxx file.

### Step 3: Structure your code

Once you have made your .cxx file, import the necceary libraries 

```cpp
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <filesystem>
#include <TH1F.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TF1.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TLatex.h>
```
This is not a strict condition but is supposed to be a guide. The user is free to have as much or as little libraries.

### Step 4: File handling

Use command-line arguments to pass the file path of your CSV data

```cpp
if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <path-to-csv-file>" << std::endl;
        return 1;
    }
```
Check if the file was opened successfully

```cpp
    std::ifstream infile(argv[1]);
    if (!infile.is_open()) {
        std::cerr << "Error opening file: " << argv[1] << std::endl;
        return 1;
    }
```
### Step 5: Extracting and Filtering

Extract the `Date`,`Time`,`Temperature` using `std::istringstream` command:

```cpp
  //Loop gets line from input file and stores in line for all rows in the inputfile
    while (std::getline(inputFile, line)) {
        //makes string into stream
        std::istringstream ss(line);
        //make variables date,time and temp
        std::string date, time, temp;
```
Now, depending what your experiment entails, you will want to filter out the data set, either for a specific date range or at specific times:

```cpp
  // For example
  // Filter for June 20 data between 1950 and 2023
            if (year >= 1950 && year <= 2023 && month_day == "06-20") {
                // Your code for the temperature
            }
```

### Step 6: Performing Calculations and plotting

This is the part where it will differ from project to project. Overall, for the plotting part we will use the `TGraph` library since we will mostly be looking at temperature trends. In order to save the plot we use:

```cpp
    // Extract file name for the output PNG file and title
    std::string inputPath = argv[1];
    std::string fileName = std::filesystem::path(inputPath).stem().string();
    std::string outputFileName = "YearlyTemperatureDifference_" + fileName + ".png";

  // Save plot to PNG with unique filename
    c1->SaveAs(outputFileName.c_str());
```

## Changing the MakeFile code

Now that you have your project compleated, the makefile needs to be edited in order to execute the code and make the graph.
We first need to make the project-specific variables. This are the source file (SRC) and the executable (EXE):

```makefile
    # Source and executable for the plotting program FOR <PROJECTNAME>
    PLOT_PROGRAM_<PROJECTNAME>_SRC = src/<PROJECTNAME>/<PROJECTNAME>.cxx
    PLOT_PROGRAM_<PROJECTNAME>_EXE = src/<PROJECTNAME>/<PROJECTNAME>
```
Then we need to define the figures which will be exported

```makefile
    # Plot output files FOR <PROJECTNAME>
    LULEA_<PROJECTNAME>_PLOT = <PROJECTNAME>_Lulea.png
    LUND_<PROJECTNAME>_PLOT = <PROJECTNAME>_Lund.png
```

And finally, we need to add the define Plot Generation Rules

```makefile
    ##### FOR YEARLY MIDDSOMAR TEMP TEST ##################

    # Compile the plotting program if it hasn't been compiled already
    $(PLOT_PROGRAM_<PROJECTNAME>_EXE): $(PLOT_PROGRAM_<PROJECTNAME>_SRC)
        $(CXX) $(CXXFLAGS) -o $(PLOT_PROGRAM_<PROJECTNAME>_EXE) $(PLOT_PROGRAM_<PROJECTNAME>_SRC)

    # Rule to generate the plot from the Lulea filtered file
    $(LULEA_<PROJECTNAME>_PLOT): $(LULEA_FILTERED) $(PLOT_PROGRAM_<PROJECTNAME>_EXE)
        ./$(PLOT_PROGRAM_<PROJECTNAME>_EXE) $(LULEA_FILTERED)

    # Rule to generate the plot from the Lund filtered file
    $(LUND_<PROJECTNAME>_PLOT): $(LUND_FILTERED) $(PLOT_PROGRAM_<PROJECTNAME>_EXE)
        ./$(PLOT_PROGRAM_<PROJECTNAME>_EXE) $(LUND_FILTERED)

```

If everything has been done correctly, after running the 

```console
make
```

the figure should be in the main repository folder along with all the other plots.