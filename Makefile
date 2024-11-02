# Compiler settings
CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++17 `root-config --cflags --libs`

# Paths
FILTER_SCRIPT = src/CleanerAndFilter/filter1950-2023.sh
DATASETS_DIR =  datasets
DATASETS_TGZ = $(DATASETS_DIR)/datasets.tgz

# Source and executable for the plotting program
PLOT_PROGRAM_SRC = src/TempDifference/yearly_temp_difference.cxx
PLOT_PROGRAM_EXE = src/TempDifference/yearly_temp_difference

# Source and executable for the plotting program FOR MEANTEMP
PLOT_PROGRAM_MEAN_SRC = src/YearlyMeanTemp/tempperyear.cxx
PLOT_PROGRAM_MEAN_EXE = src/YearlyMeanTemp/tempperyear

# Source and executable for the plotting program FOR MIDSOMMAR
PLOT_PROGRAM_MIDDSOMAR_SRC = src/MidsommarTemp/midsommar_temp_diff.cxx
PLOT_PROGRAM_MIDDSOMAR_EXE = src/MidsommarTemp/midsommar_temp_diff

# Input CSV files in the datasets directory (will be created by extracting datasets.tgz)
LULEA_CSV = $(DATASETS_DIR)/smhi-opendata_1_162860_20231007_155220_Lulea.csv
LUND_CSV = $(DATASETS_DIR)/smhi-opendata_1_53430_20231007_155558_Lund.csv

# Output filenames after filtering
LULEA_FILTERED = Lulea
LUND_FILTERED = Lund

# Plot output files
LULEA_PLOT = YearlyTemperatureDifference_Lulea.png
LUND_PLOT = YearlyTemperatureDifference_Lund.png

# Plot output files FOR MEANTEMP
LULEA_MEAN_PLOT = YearlyMeanTemp_Lulea.png
LUND_MEAN_PLOT = YearlyMeanTemp_Lund.png

# Plot output files FOR MIDDSOMAR
LULEA_MID_PLOT = MidsommarTemp_Lulea.png
LUND_MID_PLOT = MidsommarTemp_Lund.png

# Default target
all: $(LULEA_PLOT) $(LUND_PLOT) $(LULEA_MEAN_PLOT) $(LUND_MEAN_PLOT) $(LULEA_MID_PLOT) $(LUND_MID_PLOT)

# Step to extract datasets if they aren't already extracted
extract_datasets:
	tar zxvf $(DATASETS_TGZ) -C $(DATASETS_DIR)

# Ensure that every target depends on the extracted datasets
$(LULEA_CSV): extract_datasets
$(LUND_CSV): extract_datasets


# Input CSV files in the datasets directory (will be created by extracting datasets.tgz)
LULEA_CSV = $(DATASETS_DIR)/smhi-opendata_1_162860_20231007_155220_Lulea.csv
LUND_CSV = $(DATASETS_DIR)/smhi-opendata_1_53430_20231007_155558_Lund.csv



############# FOR TEMP DIFFERENCE TEST ##############

# Compile the plotting program if it hasn't been compiled already
$(PLOT_PROGRAM_EXE): $(PLOT_PROGRAM_SRC)
	$(CXX) $(CXXFLAGS) -o $(PLOT_PROGRAM_EXE) $(PLOT_PROGRAM_SRC)

# Rule to apply the filter script to the Lulea dataset
$(LULEA_FILTERED): $(FILTER_SCRIPT) $(LULEA_CSV)
	bash $(FILTER_SCRIPT) $(LULEA_CSV)

# Rule to apply the filter script to the Lund dataset
$(LUND_FILTERED): $(FILTER_SCRIPT) $(LUND_CSV)
	bash $(FILTER_SCRIPT) $(LUND_CSV)

# Rule to generate the plot from the Lulea filtered file
$(LULEA_PLOT): $(LULEA_FILTERED) $(PLOT_PROGRAM_EXE)
	./$(PLOT_PROGRAM_EXE) $(LULEA_FILTERED)

# Rule to generate the plot from the Lund filtered file
$(LUND_PLOT): $(LUND_FILTERED) $(PLOT_PROGRAM_EXE)
	./$(PLOT_PROGRAM_EXE) $(LUND_FILTERED)



##### FOR YEARLY MEAN TEMP TEST ##################



# Compile the plotting program if it hasn't been compiled already
$(PLOT_PROGRAM_MEAN_EXE): $(PLOT_PROGRAM_MEAN_SRC)
	$(CXX) $(CXXFLAGS) -o $(PLOT_PROGRAM_MEAN_EXE) $(PLOT_PROGRAM_MEAN_SRC)

# Rule to generate the plot from the Lulea filtered file
$(LULEA_MEAN_PLOT): $(LULEA_FILTERED) $(PLOT_PROGRAM_MEAN_EXE)
	./$(PLOT_PROGRAM_MEAN_EXE) $(LULEA_FILTERED)

# Rule to generate the plot from the Lund filtered file
$(LUND_MEAN_PLOT): $(LUND_FILTERED) $(PLOT_PROGRAM_MEAN_EXE)
	./$(PLOT_PROGRAM_MEAN_EXE) $(LUND_FILTERED)



##### FOR YEARLY MIDDSOMAR TEMP TEST ##################



# Compile the plotting program if it hasn't been compiled already
$(PLOT_PROGRAM_MIDDSOMAR_EXE): $(PLOT_PROGRAM_MIDDSOMAR_SRC)
	$(CXX) $(CXXFLAGS) -o $(PLOT_PROGRAM_MIDDSOMAR_EXE) $(PLOT_PROGRAM_MIDDSOMAR_SRC)

# Rule to generate the plot from the Lulea filtered file
$(LULEA_MID_PLOT): $(LULEA_FILTERED) $(PLOT_PROGRAM_MIDDSOMAR_EXE)
	./$(PLOT_PROGRAM_MIDDSOMAR_EXE) $(LULEA_FILTERED)

# Rule to generate the plot from the Lund filtered file
$(LUND_MID_PLOT): $(LUND_FILTERED) $(PLOT_PROGRAM_MIDDSOMAR_EXE)
	./$(PLOT_PROGRAM_MIDDSOMAR_EXE) $(LUND_FILTERED)

# Clean up all generated files
clean:
	rm -f $(LULEA_PLOT) $(LUND_PLOT) $(LULEA_MEAN_PLOT) $(LUND_MEAN_PLOT) $(LULEA_FILTERED) $(LUND_FILTERED) $(PLOT_PROGRAM_EXE)

