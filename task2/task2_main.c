#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 256

int sensor_count = 0;

// Task 2.1
void filterOutliers(char *dataFilename) {
    FILE *dataFile = fopen(dataFilename, "r");
    FILE *outliersFile = fopen("dust_outlier.csv", "w");

    if (dataFile == NULL || outliersFile == NULL) {
        printf("Error 01: input file not found or not accessible\n");
        return;
    }

    fprintf(outliersFile, "id,time,value\n");

    char line[MAX_LINE_LENGTH];
    int lineCount = 0;
    int numOutliers = 0;

    while (fgets(line, sizeof(line), dataFile) != NULL) {
        lineCount++;

        // Skip header line
        if (line[0] == 'i') {
            continue;
        }

        char idStr[10];
        char time[20];
        char valueStr[10];

        sscanf(line, "%[^,],%[^,],%[^,\n]", idStr, time, valueStr);

        // Check if any field is blank
        if (idStr[0] == '\0' || time[0] == '\0' || valueStr[0] == '\0') {
            printf("Error 04: data is missing at line %d\n", lineCount);
            continue;
        }

        int id = atoi(idStr);
        if (id > sensor_count) {
            sensor_count = id;
        }
        float value = atof(valueStr);

        // Check if id is valid
        if (id <= 0) {
            printf("Error 04: data is missing or invalid at line %d\n", lineCount);
            continue;
        }

        // Check if value is an outlier
        if (value < 5.0 || value > 550.5) {
            fprintf(outliersFile, "%s,%s,%s\n", idStr, time, valueStr);
            numOutliers++;
        }
    }

    fprintf(outliersFile, "number of outliers: %d\n", numOutliers); //Sao mày ko lên trên đầu được :(

    fclose(dataFile);
    fclose(outliersFile);
    printf("Outliers removed. Outliers stored in 'dust_outlier.csv'.\n");
}

// Task 2.2
double
AQIcalculating(double concentration, double breakpointLow, double breakpointHigh, double indexLow, double indexHigh) {
    return ((indexHigh - indexLow) / (breakpointHigh - breakpointLow)) * (concentration - breakpointLow) + indexLow;
}

void calculateAQIFromFile(char *dataFilename) {
    FILE *dataFile = fopen(dataFilename, "r");
    FILE *aqiFile = fopen("dust_aqi.csv", "w");


    if (dataFile == NULL || aqiFile == NULL) {
        printf("Error 01: input file not found or not accessible\n");
        return;
    }

    fprintf(aqiFile, "id,time,value,aqi,pollution\n");

    char line[MAX_LINE_LENGTH];
    int lineCount = 0;

    while (fgets(line, sizeof(line), dataFile) != NULL) {
        lineCount++;

        // Skip header line
        if (line[0] == 'i') {
            continue;
        }

        char idStr[10];
        char time[20];
        char valueStr[10];

        sscanf(line, "%[^,],%[^,],%[^,\n]", idStr, time, valueStr);

        // Check if any field is blank
        if (idStr[0] == '\0' || time[0] == '\0' || valueStr[0] == '\0') {
            printf("Error 04: data is missing at line %d\n", lineCount);
            continue;
        }

        int id = atoi(idStr);
        float value = atof(valueStr);

        // Check if id is valid
        if (id <= 0) {
            printf("Error 04: data is missing or invalid at line %d\n", lineCount);
            continue;
        }

        // Validate data
        if (value < 5.0 || value > 550.5) {
            continue;
        }

        // Calculate AQI and pollution level
        int aqi;
        const char *pollution;

        if (value >= 0 && value < 12) {
            aqi = AQIcalculating(value, 0, 12, 0, 50);
            pollution = "Good";
        } else if (value >= 12 && value < 35.5) {
            aqi = AQIcalculating(value, 12, 35.5, 51, 100);
            pollution = "Moderate";
        } else if (value >= 35.5 && value < 55.5) {
            aqi = AQIcalculating(value, 35.5, 55.5, 101, 150);
            pollution = "Slightly unhealthy";
        } else if (value >= 55.5 && value < 150.5) {
            aqi = AQIcalculating(value, 55.5, 150.5, 151, 200);
            pollution = "Unhealthy";
        } else if (value >= 150.5 && value < 250.5) {
            aqi = AQIcalculating(value, 150.5, 250.5, 201, 300);
            pollution = "Very unhealthy";
        } else if (value >= 250.5 && value < 350.5) {
            aqi = AQIcalculating(value, 250.5, 350.5, 301, 400);
            pollution = "Hazardous";
        } else {
            aqi = AQIcalculating(value, 350.5, 550.5, 401, 500);
            pollution = "Extremely hazardous";
        }

        fprintf(aqiFile, "%s,%s,%s,%d,%s\n", idStr, time, valueStr, aqi, pollution);
    }

    fclose(dataFile);
    fclose(aqiFile);
    printf("AQI calculation completed. Results stored in 'dust_aqi.csv'.\n");
}

// Task 2.3


void processSensorData(char *dataFilename) {
//    printf("hi");
    FILE *dataFile = fopen(dataFilename, "r");
    FILE *summaryFile = fopen("dust_summary.csv", "w");

    if (dataFile == NULL || summaryFile == NULL) {
        printf("Error 01: input file not found or not accessible\n");
        return;
    }


    fprintf(summaryFile, "id,parameter,time,value\n");

    char line[MAX_LINE_LENGTH];
    int lineCount = 0;

    int limit = sensor_count + 1;
    /// declare temp data
    float sensorStat[limit][4];
    for (int i = 0; i < limit; ++i) {
        sensorStat[i][0] = 600;
    }

    char timeData[limit][2][20];

    // Update maximum, minimum, and sum for each sensor
    float maxValues[10] = {0};
    float minValues[10] = {550.5};
    float sumValues[10] = {0};
    int countValues[10] = {0};

    while (fgets(line, sizeof(line), dataFile) != NULL) {
        lineCount++;

        // Skip header line
        if (line[0] == 'i') {
            continue;
        }

        char idStr[10];
        char time[20];
        char valueStr[10];

        sscanf(line, "%[^,],%[^,],%[^,\n]", idStr, time, valueStr);

        // Check if any field is blank
        if (idStr[0] == '\0' || time[0] == '\0' || valueStr[0] == '\0') {
            printf("Error 04: data is missing at line %d\n", lineCount);
            continue;
        }

        int id = atoi(idStr);
        float value = atof(valueStr);

        // Check if id is valid
        if (id <= 0) {
            printf("Error 04: data is missing or invalid at line %d\n", lineCount);
            continue;
        }

        // Validate data
        if (value < 5.0 || value > 550.5) {
            continue;
        } else {
            if (value < sensorStat[id][0]) {
                sensorStat[id][0] = value;
                strcpy(timeData[id][0], time);
            } else if (value > sensorStat[id][1]) {
                sensorStat[id][1] = value;
                strcpy(timeData[id][1], time);
            }
            sensorStat[id][2] += value;
            sensorStat[id][3]++;
        }

    }
    fclose(dataFile);

    // Write max, min, and mean to the summary file
    for (int i = 1; i < limit; ++i) {
        fprintf(summaryFile, "%d, max, %s, %.1f\n", i, timeData[i][1], sensorStat[i][1]);
        fprintf(summaryFile, "%d, min, %s, %.1f\n", i, timeData[i][0], sensorStat[i][0]);
        float mean = sensorStat[i][2] / sensorStat[i][3];
        fprintf(summaryFile, "%d, mean, 10:00:00, %.1f\n", i, mean);
    }
    fclose(summaryFile);
    printf("INPUT DESC HERE. Results stored in 'dust_summary.csv'.\n");
}

// Task 2.4

void calculatePollutionStatistics(char *dataFilename) {
    FILE *dataFile = fopen(dataFilename, "r");
    FILE *statisticsFile = fopen("dust_statistics.csv", "w");

    // Number of conditions hard-coded
    int condition_count = 7;

    if (dataFile == NULL || statisticsFile == NULL) {
        dataFile = fopen("./dust_aqi.csv", "r");
        printf("Error: Input file not found or not accessible.\n");
        // return;
    }

    // Create an array to store the pollution levels for each sensor

    char line[MAX_LINE_LENGTH];
    int lineCount = 0;
    char aqi_level[7][20] = {
            "Good", "Moderate", "Slightly unhealthy", "Unhealthy", "Very unhealthy", "Hazardous",
            "Extremely hazardous"};

    int limit = 10000; // maximum sensors
    int dataMatrix[limit][7];
    int visited[limit];

    /*
    Lưu vào data Matrix. Với mỗi giá trị ở tọa độ `i` `j` 
    sẽ chứa thông tin về số lượng lượt appear của level `j` của chỉ số `i`
    e.g dataMatrix[10][2] sẽ chứa số lần "Moderate" xuất hiện với sensor 10
    */


    lineCount = 0;
    while (fgets(line, sizeof(line), dataFile) != NULL) {
        lineCount++;

        // Skip header line
        if (line[0] == 'i') {
            continue;
        }

        char idStr[10];
        char time[20];
        char valueStr[10];

        sscanf(line, "%[^,],%[^,],%[^,\n]", idStr, time, valueStr);

        // Check if any field is blank. Lặp lại 2.1 check outlier.
        if (idStr[0] == '\0' || time[0] == '\0' || valueStr[0] == '\0') {
            printf("Error 04: Data is missing at line %d\n", lineCount);
            continue;
        }

        int id = atoi(idStr);
        float value = atof(valueStr);

        // Check if id is valid
        if (id <= 0) {
            printf("Error 04: Data is missing or invalid at line %d\n", lineCount);
            continue;
        }

        // Validate data
        if (value < 5.0 || value > 550.5) {
            continue;
        }

        // Increment the corresponding pollution level for the current sensor. Lặp lại 2.2 do input là file gốc.
        if (value >= 0 && value <= 12) {
            dataMatrix[id][0]++;
        } else if (value > 12 && value <= 35.4) {
            dataMatrix[id][1]++;
        } else if (value > 35.4 && value <= 55.4) {
            dataMatrix[id][2]++;
        } else if (value > 55.4 && value <= 150.4) {
            dataMatrix[id][3]++;
        } else if (value > 150.4 && value <= 250.4) {
            dataMatrix[id][4]++;
        } else if (value > 250.4 && value <= 350.4) {
            dataMatrix[id][5]++;
        } else if (value > 350.4 && value <= 550.5) {
            dataMatrix[id][6]++;
        }
        visited[id] = 1;
    }

    fclose(dataFile);

    // Write the pollution duration to the statistics file
    fprintf(statisticsFile, "id, pollution, duration\n");

    for (int i = 0; i < limit; i++) {
        if (visited[i] == 1) {
            for (size_t j = 0; j < 7; j++) {
                int duration = dataMatrix[i][j];
                fprintf(statisticsFile, "%d, %s, %d\n", i, aqi_level[j], duration);
            }
        }
    }

    fclose(statisticsFile);

    printf("Pollution duration calculation completed. Results stored in 'dust_statistics.csv'.\n");
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("Error 03: invalid command\n");
        printf("Usage: program_name data_file\n");
        return 1;
    }

    char *dataFilename = argv[1];

    filterOutliers(dataFilename);
    calculateAQIFromFile(dataFilename);
    processSensorData(dataFilename);
    calculatePollutionStatistics(dataFilename);
    return 0;
}
