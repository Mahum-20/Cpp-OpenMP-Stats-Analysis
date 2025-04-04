# Cpp-OpenMP-Stats-Analysis

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen)](https://github.com/yourusername/repository-name/actions)
[![License](https://img.shields.io/badge/license-MIT-blue)](https://opensource.org/licenses/MIT)

This project analyzes projected births and deaths data by country using OpenMP to speed up data processing and calculations. The application reads a CSV file containing demographic estimates for multiple countries and computes statistical analysis, including mean, standard deviation, and quartiles.

## Features

- Parallel data processing with OpenMP for fast computation.
- Calculates **Mean**, **Standard Deviation**, and **Quartiles** (Q1, Median, Q3) for births and deaths data.
- Analyzes data across different countries and presents country-wise statistics.
- Overall statistics for all countries combined.

## Prerequisites

Before running the project, ensure you have the following installed:

- A C++ compiler (e.g., GCC)
- OpenMP support enabled in the compiler (`-fopenmp` flag)
- CMake or a Makefile for building the project

## Installation

Clone this repository to your local machine:

```bash
git clone https://github.com/yourusername/repository-name.git
cd repository-name
```
## Building the Project
To compile the C++ program, run the following command:
```bash
g++ -fopenmp -o analysis main.cpp
```
## Running the Program
To run the analysis, use the following command:
```bash
./analysis
```
Ensure you have a valid CSV file with the correct format. The program will process the file and output statistics to the terminal.
## Usage
The program processes a CSV file containing the following columns:

- **Entity** (Country or Region)
- **Code** (Country Code)
- **Year**
- **Deaths Estimate**
- **Births Estimate**

### Example of input data (CSV format):
```csv
"United States","USA",2020,3000000,4000000
"India","IND",2020,7000000,8000000
```

## Output
After running the program, you’ll get the following output:

- **Overall statistics** (mean, standard deviation, and quartiles for births and deaths)
- **Country-wise statistics** for each country's demographic data

### Example Output:
```mathematica
===== Overall Statistics =====
Total Records: 1000
Births - Mean: 5.6, SD: 1.2
        Quartiles (Q1, Median, Q3): 4.5, 5.7, 6.8
Deaths - Mean: 3.4, SD: 0.8
        Quartiles (Q1, Median, Q3): 2.9, 3.4, 3.9

Country: United States (USA)
  Records: 10
  Births - Mean: 5.8, SD: 1.1
          Quartiles (Q1, Median, Q3): 5.2, 5.8, 6.4
  Deaths - Mean: 3.5, SD: 0.7
          Quartiles (Q1, Median, Q3): 3.0, 3.5, 4.0
```
## License
This project is licensed under the MIT License - see the LICENSE file for details.
