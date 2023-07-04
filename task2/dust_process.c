#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 256
//Task 2.1 - Done
void filterOutliers(char* dataFilename) {
    
    FILE* dataFile = fopen(dataFilename, "r");
    FILE* outliersFile = fopen("dust_outlier.csv" , "w");

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
        float value = atof(valueStr);

        // Check if id is valid
        if (id <= 0) {
            printf("Error 04: data is missing or invalid at line %d\n", lineCount);
            continue;
        }

        // Check if value is an outlier
        if (value < 5.0 || value > 550.5) {
            fprintf(outliersFile, "%d,%s,%s\n", id, time, valueStr);
            numOutliers++;
        }
    }

    fprintf(outliersFile, "number of outliers: %d\n", numOutliers);

    fclose(dataFile);
    fclose(outliersFile);
    
}

//Task 2.2 - Done
void calculateAQI(char* dataFilename) {
    FILE* dataFile = fopen(dataFilename, "r");
    FILE* aqiFile = fopen("dust_aqi.csv", "w");

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
        //Validate data
        if (value < 5.0 || value > 550.5) {
            continue;
        }

        // Calculate AQI and pollution level. Tính lại công thức AQI hộ bố mày.
        int aqi;
        char* pollution;

        if (value >= 0 && value < 12)
        {
            aqi = 0;
            pollution = "Good";
        }
        else if (value >= 12 && value < 35.5)
        {
            aqi = 50;
            pollution = "Moderate";
        }
        else if (value >= 35.5 && value < 55.5)
        {
            aqi = 100;
            pollution = "Slightly unhealthy";
        }
        else if (value >= 55.5 && value < 150.5)
        {
            aqi = 150;
            pollution = "Unhealthy";
        }
        else if (value >= 150.5 && value < 250.5)
        {
            aqi = 200;
            pollution = "Very unhealthy";
        }
        else if (value >= 250.5 && value < 350.5)
        {
            aqi = 300;
            pollution = "Hazardous";
        }
        else
        {
            aqi = 400;
            pollution = "Extremely hazardous";
        }

        fprintf(aqiFile, "%d,%s,%s,%d,%s\n", id, time, valueStr, aqi, pollution);
    }

    fclose(dataFile);
    fclose(aqiFile);
}

//Task 2.3
typedef struct {
    int id;
    double max;
    double min;
    double sum;
    int count;
} SensorData;

void processSensorData(char* dataFilename) {
    FILE* dataFile = fopen(dataFilename, "r");
    FILE* summaryFile = fopen("dust_summary.csv", "w");
    SensorData sensors[3] = {0};
    char line[MAX_LINE_LENGTH];

    // Skip the header line
    fgets(line, sizeof(line), dataFile);

    while (fgets(line, sizeof(line), dataFile) != NULL) {
        int id;
        char timestamp[20];
        double value;

        sscanf(line, "%d,%[^,],%lf", &id, timestamp, &value);

        if (id < 1 || id > 3) {
            printf("Invalid sensor ID: %d\n", id);
            continue;
        }

        SensorData* sensor = &sensors[id - 1];
        
        if (sensor->count == 0 || value > sensor->max) {
            sensor->max = value;
        }

        if (sensor->count == 0 || value < sensor->min) {
            sensor->min = value;
        }
        
        sensor->sum += value;
        sensor->count++;
    }

    // Write summary to the output file
    fprintf(summaryFile, "id,parameter,time,value\n");

    for (int i = 0; i < 3; i++) {
        SensorData* sensor = &sensors[i];
        printf("%d", sensor->id);

        fprintf(summaryFile, "%d,max,%s,%.1f\n", sensor->id, sensor->max);
        fprintf(summaryFile, "%d,min,%s,%.1f\n", sensor->id,sensor->min);

        double mean = sensor->sum / sensor->count;
        fprintf(summaryFile, "%d,mean,%s,%.1f\n", sensor->id, mean);
    }
  
    fclose(dataFile);
    fclose(summaryFile);
}
//Task 2.4


int main(int argc, char* argv[]) {
    
    if (argc != 2) {
        printf("Error 03: invalid command\n");
        printf("Usage: program_name data_file outliers_file aqi_file summary_file\n");
        return 1;
    }

    char* dataFilename = argv[1];
    
    
    // char outliersFilename = ".\dust_outlier.csv";
    // char aqiFilename = argv[3];
    // char summaryFilename = argv[4];

    filterOutliers(dataFilename);
    calculateAQI(dataFilename);
    processSensorData(dataFilename);

    return 0;
}
