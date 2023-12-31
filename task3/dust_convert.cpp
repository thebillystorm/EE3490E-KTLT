#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <ctime>
#include <bitset>
#include <iomanip>
#include <cstring>
#include <cstdint>

using namespace std;
void logError(const std::string& errorMessage) {
    std::ofstream logFile("task3.log", std::ios::app); // Open log file in append mode
    if (logFile) {
        logFile << errorMessage << std::endl;
        logFile.close();
    }
}
bool checkCSVFormat(const std::string& dataFilename) {
    std::ifstream file(dataFilename);
    if (!file) {
        logError("Error 01: Input file not found or not accessible");
        return false;
    }

    std::string line;
    if (!std::getline(file, line)) {
        logError("Error 02: Input file does not have a header");
        return false;
    }

    std::istringstream iss(line);
    std::string field;
    std::vector<std::string> header;
    while (std::getline(iss, field, ',')) {
        header.push_back(field);
    }

    if (header.empty()) {
        logError("Error 02: Invalid CSV file format");
        return false;
    }

    std::string previousLine = line;
    int previousFieldCount = header.size();
    int lineNumber = 1;
    while (std::getline(file, line)) {
        lineNumber++;

        std::istringstream iss(line);
        int fieldCount = 0;
        while (std::getline(iss, field, ',')) {
            fieldCount++;
        }

        if (fieldCount != previousFieldCount) {
            logError("Error 02: Invalid CSV file format");
            return false;
        }

        previousLine = line;
    }

    return true;
}

bool checkOutputFile(const std::string& outputFilename) {
    std::ifstream outputFile(outputFilename);
    return outputFile.good();
}

void processFile(const std::string& outputFilename) {
  
    std::string errorFilename = "task3.log";
    // Check if the output file already exists
    if (checkOutputFile(outputFilename)) {
        // Output file exists, write an error message to the error log file
        std::ofstream errorFile(errorFilename, std::ios::app);
        if (errorFile) {
            errorFile << "Error 05: cannot override " << outputFilename << std::endl;
            errorFile.close();
        } else {
            std::cerr << "Failed to open error log file: " << errorFilename << std::endl;
        }

        return;
    }
 
    std::ofstream outputFileWrite(outputFilename, std::ios::binary);
    if (!outputFileWrite) {
        std::cerr << "Failed to open output file: " << outputFilename << std::endl;
        return;
    }
   
    outputFileWrite.close();
}

bool isBlank(const std::string& str) {
    return str.find_first_not_of(' ') == std::string::npos;
}

bool isTimeValid(const std::string& time) {
    return (time.length() == 19 &&
            time[4] == ':' && time[7] == ':' && time[10] == ' ' &&
            time[13] == ':' && time[16] == ':');
}

void checkInputFile(const std::string& filename) {
    std::ifstream inputFile(filename);
    std::string line;
    int lineNumber = 0;
    bool errorFound = false;

    // Create error log file
    std::ofstream errorLog("task3.log");

    // Process the lines
    while (std::getline(inputFile, line)) {
        lineNumber++;

        std::istringstream iss(line);
        std::string id, time, value, aqi, pollution;

        // Read the fields from the line
        std::getline(iss, id, ',');
        std::getline(iss, time, ',');
        std::getline(iss, value, ',');
        std::getline(iss, aqi, ',');
        std::getline(iss, pollution, ',');

        // Check for errors
        if (lineNumber > 1){
        if (isBlank(id) || isBlank(time) || isBlank(value) || isBlank(aqi) || isBlank(pollution) ||
            !isTimeValid(time)) {
            logError("Error 04: Invalid data format at line " + std::to_string(lineNumber));
            errorFound = true;
        }
        }
    }

    inputFile.close();
}

bool processFiles(const std::string& inputFile, const std::string& outputFile) {
    if (inputFile.empty() || outputFile.empty()) {
        logError("Error 03: invalid command");
        return false;
    }

    if (!checkCSVFormat(inputFile)) {
        return false;
    }

    // Rest of the processing logic for files...

    return true;
}
string addStartBytes(const string& input) {
    return "7a";
}
string addStopBytes(const string& input) {
    return "7f";
}
string formatAQI(uint16_t aqi) {
    stringstream ss;
    ss << setfill('0') << setw(4) << hex << aqi;
    return ss.str();
}


string floatToHex(float value) {
    stringstream ss;
    ss << hex << setfill('0') << setw(8);

    // Convert float to an integer representation
    uint32_t intValue;
    memcpy(&intValue, &value, sizeof(float));

    // Output the integer value as hexadecimal
    ss << intValue;


    return ss.str();
}

struct DataEntry {
    int id;
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    double value;
    int aqi;
};

vector<DataEntry> extractData(const string& filename) {
    ifstream file(filename);
    string line;
    vector<DataEntry> dataList;

    // Skip the header line
    getline(file, line);

    while (getline(file, line)) {
        istringstream iss(line);
        string idString, datetime, valueString, aqiString, pollution;

        getline(iss, idString, ',');
        getline(iss, datetime, ',');
        getline(iss, valueString, ',');
        getline(iss, aqiString, ',');
        getline(iss, pollution, ',');

        DataEntry data;
        data.id = stoi(idString);
        sscanf(datetime.c_str(), "%4d:%2d:%2d %2d:%2d:%2d",
               &data.year, &data.month, &data.day,
               &data.hour, &data.minute, &data.second);
        data.value = stod(valueString);
        data.aqi = stoi(aqiString);

        dataList.push_back(data);
    }

    file.close();

    return dataList;
}

string convert(int decimalNumber) {
    string hexadecimalNumber;
    const int base = 16;

    while (decimalNumber > 0) {
        int remainder = decimalNumber % base;

        if (remainder < 10)
            hexadecimalNumber += static_cast<char>(remainder + '0');
        else
            hexadecimalNumber += static_cast<char>(remainder - 10 + 'a');

        decimalNumber /= base;
    }

    reverse(hexadecimalNumber.begin(), hexadecimalNumber.end());
    return hexadecimalNumber;
}

int getUnixTimestamp(int year, int month, int day, int hour, int min, int sec) {
    time_t rawtime;
    struct tm* timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    timeinfo->tm_year = year - 1900;
    timeinfo->tm_mon = month - 1;
    timeinfo->tm_mday = day;
    timeinfo->tm_hour = hour;
    timeinfo->tm_min = min;
    timeinfo->tm_sec = sec;

    time_t timestamp = mktime(timeinfo);

    return static_cast<int>(timestamp);
}
string calculateChecksumTwoComplement(const string& input) {
    unsigned int sum = 0;

    // Iterate through each character pair in the string
    for (size_t i = 0; i < input.length(); i += 2) {
        // Extract the two characters representing a hexadecimal value
        string hexPair = input.substr(i, 2);

        // Convert the hexadecimal value to an integer
        unsigned int value;
        stringstream ss;
        ss << hex << hexPair;
        ss >> value;

        // Add the value to the sum
        sum += value;
    }

    // Calculate the least significant byte of the sum
    unsigned char lsb = static_cast<unsigned char>(sum & 0xFF);

    // Calculate the two's complement of the least significant byte
    unsigned char checksum = static_cast<unsigned char>((~lsb + 1) & 0xFF);

    // Convert the checksum to a string in hexadecimal format
    stringstream result;
    result << hex << setw(2) << setfill('0') << static_cast<int>(checksum);
    return result.str();
}

string insertSpaceEveryTwoCharacters(const string& input) {
    string result;
    int count = 0;
    
    for (char ch : input) {
        result += ch;
        count++;
        
        if (count % 2 == 0) {
            result += ' ';
        }
    }
    
    return result;
}
string autoFillZero(const string& input) {
    if (input.length() == 1) {
        return "0" + input;
    }
    
    return input;
}
string calculatePacketLength(const string& packet) {
    // Return "0f" regardless of the input
    return "0f";
}

int main(int argc, char *argv[]) {
    string filename = argv[1];
    string inputFilename = argv[2];
    checkInputFile(filename);
    processFile(inputFilename);
    checkCSVFormat(filename);
    ofstream fileout;
    fileout.open(inputFilename,ios_base::out);
    vector<DataEntry> dataList = extractData(filename);
    for (const auto& data : dataList) {
        int unixTimestamp = getUnixTimestamp(data.year, data.month, data.day, data.hour, data.minute, data.second); 
        string tmp = "0f" + (autoFillZero(convert(data.id)))+(convert(unixTimestamp))+(floatToHex(data.value))+ (formatAQI(data.aqi));
        string tmm = insertSpaceEveryTwoCharacters(addStopBytes((calculateChecksumTwoComplement(tmp)))) + insertSpaceEveryTwoCharacters(addStartBytes(autoFillZero(convert(data.id))))+ (autoFillZero(convert(data.id)))+(convert(unixTimestamp))+(floatToHex(data.value))+ (formatAQI(data.aqi));
        fileout << insertSpaceEveryTwoCharacters(addStartBytes(autoFillZero(convert(data.id))));
        fileout << insertSpaceEveryTwoCharacters(calculatePacketLength(tmm));
        fileout << insertSpaceEveryTwoCharacters(autoFillZero(convert(data.id)));
        fileout << insertSpaceEveryTwoCharacters(convert(unixTimestamp));
        fileout << insertSpaceEveryTwoCharacters(floatToHex(data.value));
        fileout << insertSpaceEveryTwoCharacters(formatAQI(data.aqi));
        fileout << insertSpaceEveryTwoCharacters(calculateChecksumTwoComplement(tmp));
        fileout << insertSpaceEveryTwoCharacters(addStopBytes((calculateChecksumTwoComplement(tmp))));
        fileout << endl;
    }

  
   
    fileout.close();
    return 0;
}