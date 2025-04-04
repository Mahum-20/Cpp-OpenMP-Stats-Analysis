#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <ctime>
#include <algorithm> 
#include <regex>
#include <omp.h>

using namespace std;

struct CountryData {
    string entity;
    string code;
    vector<int> year;
    vector<double> births_estimates;
    vector<double> deaths_estimates;
};

vector<CountryData> countries;

string safe_token(const string& token) {
    return token.empty() ? "0" : token;
}

string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t");
    size_t last = str.find_last_not_of(" \t");
    if (first == string::npos) return "";
    return str.substr(first, last - first + 1);
}

void process_line(const string& line) {
    int thread_id = omp_get_thread_num();
    cout << "Thread " << thread_id << " processing line: " << line << endl;

    string entity, code, year_str, deaths_str, births_str;

    regex csv_regex(R"((?:\"([^\"]*)\"|([^,]*)),?)");
    sregex_iterator iter(line.begin(), line.end(), csv_regex);
    sregex_iterator end;

    vector<string> tokens;
    for (; iter != end; ++iter) {
        if ((*iter)[1].matched) {
            tokens.push_back((*iter)[1].str());
        }
        else {
            tokens.push_back((*iter)[2].str());
        }
    }

    if (tokens.size() < 6) {
        cerr << "Thread " << thread_id << ": Error parsing line: " << line << endl;
        return;
    }

    entity = trim(tokens[0]);
    code = trim(tokens[1]);
    year_str = tokens[2];
    deaths_str = safe_token(tokens[3]);
    births_str = safe_token(tokens[5]);

    deaths_str = trim(deaths_str);
    births_str = trim(births_str);

    try {
        int year = stoi(year_str);
        double deaths = stod(deaths_str);
        double births = stod(births_str);

#pragma omp critical
        {
            cout << "Thread " << thread_id << " adding/updating data for: " << entity << " (" << code << ")" << endl;

            for (auto& country : countries) {
                if (country.entity == entity && country.code == code) {
                    country.year.push_back(year);
                    country.deaths_estimates.push_back(deaths);
                    country.births_estimates.push_back(births);
                    continue;
                }
            }

            CountryData new_country;
            new_country.entity = entity;
            new_country.code = code;
            new_country.year.push_back(year);
            new_country.deaths_estimates.push_back(deaths);
            new_country.births_estimates.push_back(births);
            countries.push_back(new_country);
        }
    }
    catch (...) {
        cerr << "Thread " << thread_id << ": Error parsing line: " << line << endl;
    }
}

void read_and_preprocess_data(const string& filename) {
    ifstream file(filename);
    if (!file) {
        cerr << "Error: Could not open file " << filename << endl;
        exit(EXIT_FAILURE);
    }

    vector<string> lines;
    string line;

    getline(file, line);
    while (getline(file, line)) {
        lines.push_back(line);
    }

    cout << "Starting data processing with " << omp_get_max_threads() << " threads..." << endl;

#pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < lines.size(); ++i) {
        process_line(lines[i]);
    }
}

vector<double> calculate_quartiles(vector<double> data) {
    if (data.empty()) return { 0, 0, 0 };

    sort(data.begin(), data.end());

    int n = data.size();
    double Q1 = data[n / 4];
    double Q2 = data[n / 2];
    double Q3 = data[3 * n / 4];

    if (n % 2 == 0) {
        Q2 = (data[(n / 2) - 1] + data[n / 2]) / 2.0;
    }

    return { Q1, Q2, Q3 };
}

void calculate_overall_stats() {
    vector<double> all_births, all_deaths;

    for (const auto& country : countries) {
        all_births.insert(all_births.end(), country.births_estimates.begin(), country.births_estimates.end());
        all_deaths.insert(all_deaths.end(), country.deaths_estimates.begin(), country.deaths_estimates.end());
    }

    int total_records = all_births.size();
    if (total_records == 0) {
        cout << "No data available for overall statistics." << endl;
        return;
    }

    double births_sum = 0, deaths_sum = 0;

#pragma omp parallel for reduction(+:births_sum, deaths_sum)
    for (int i = 0; i < total_records; ++i) {
        births_sum += all_births[i];
        deaths_sum += all_deaths[i];
    }

    double births_mean = births_sum / total_records;
    double deaths_mean = deaths_sum / total_records;

    double births_var = 0, deaths_var = 0;
#pragma omp parallel for reduction(+:births_var, deaths_var)
    for (int i = 0; i < total_records; ++i) {
        births_var += pow(all_births[i] - births_mean, 2);
        deaths_var += pow(all_deaths[i] - deaths_mean, 2);
    }

    double births_sd = sqrt(births_var / total_records);
    double deaths_sd = sqrt(deaths_var / total_records);

    vector<double> births_quartiles = calculate_quartiles(all_births);
    vector<double> deaths_quartiles = calculate_quartiles(all_deaths);

    cout << "===== Overall Statistics =====" << endl;
    cout << "Total Records: " << total_records << endl;
    cout << "Births - Mean: " << births_mean << ", SD: " << births_sd << endl;
    cout << "        Quartiles (Q1, Median, Q3): " << births_quartiles[0] << ", "
        << births_quartiles[1] << ", " << births_quartiles[2] << endl;
    cout << "Deaths - Mean: " << deaths_mean << ", SD: " << deaths_sd << endl;
    cout << "        Quartiles (Q1, Median, Q3): " << deaths_quartiles[0] << ", "
        << deaths_quartiles[1] << ", " << deaths_quartiles[2] << endl;
}

void calculate_stats() {
#pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < countries.size(); ++i) {
        const auto& country = countries[i];
        int count = country.births_estimates.size();
        if (count == 0) continue;

        double births_sum = 0, deaths_sum = 0;

        for (int j = 0; j < count; ++j) {
            births_sum += country.births_estimates[j];
            deaths_sum += country.deaths_estimates[j];
        }

        double births_mean = births_sum / count;
        double deaths_mean = deaths_sum / count;

        double births_var = 0, deaths_var = 0;

        for (int j = 0; j < count; ++j) {
            births_var += pow(country.births_estimates[j] - births_mean, 2);
            deaths_var += pow(country.deaths_estimates[j] - deaths_mean, 2);
        }

        double births_sd = sqrt(births_var / count);
        double deaths_sd = sqrt(deaths_var / count);

        vector<double> births_quartiles = calculate_quartiles(country.births_estimates);
        vector<double> deaths_quartiles = calculate_quartiles(country.deaths_estimates);

#pragma omp critical
        {
            cout << "Country: " << country.entity << " (" << country.code << ")" << endl;
            cout << "  Records: " << count << endl;
            cout << "  Births - Mean: " << births_mean << ", SD: " << births_sd << endl;
            cout << "          Quartiles (Q1, Median, Q3): " << births_quartiles[0] << ", "
                << births_quartiles[1] << ", " << births_quartiles[2] << endl;
            cout << "  Deaths - Mean: " << deaths_mean << ", SD: " << deaths_sd << endl;
            cout << "          Quartiles (Q1, Median, Q3): " << deaths_quartiles[0] << ", "
                << deaths_quartiles[1] << ", " << deaths_quartiles[2] << endl;
        }
    }
}

int main() {
    const string filename = "C:\\Users\\PMLS\\Documents\\births-and-deaths-projected-to-2100.csv";

    double start_time = omp_get_wtime();

    read_and_preprocess_data(filename);
    calculate_overall_stats();
    calculate_stats();

    double end_time = omp_get_wtime();

    cout << "\nExecution time of OpenMP program: " << end_time - start_time << " seconds" << endl;

    return 0;
}
