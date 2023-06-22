#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Function to generate a random value within the specified range
double generateRandomValue(double min, double max)
{
    double random = (double)rand() / RAND_MAX; // Random value between 0 and 1
    return min + random * (max - min);
}

void logError(const char *task, int errorCode, const char *description)
{
    char logFileName[20];
    sprintf(logFileName, "%s.log", task);
    FILE *logFile = fopen(logFileName, "a");

    if (logFile != NULL)
    {
        fprintf(logFile, "Error %02d: %s\n", errorCode, description);
        fclose(logFile);
    }
    else
    {
        printf("Error opening log file.\n");
    }
}

int main(int argc, char *argv[])
{
    // Default values
    int num_sensors = 1;
    int sampling_time = 30;
    int duration = 24;

    // Parse command-line arguments
    if (argc > 1)
    {
        for (int i = 1; i < argc; i += 2)
        {
            if (strcmp(argv[i], "-n") == 0)
            {
                num_sensors = atoi(argv[i + 1]);
            }
            else if (strcmp(argv[i], "-st") == 0)
            {
                sampling_time = atoi(argv[i + 1]);
            }
            else if (strcmp(argv[i], "-si") == 0)
            {
                duration = atoi(argv[i + 1]);
            }
        }
    }
    else
    {
        logError("task1", 1, "invalid command");
        return 1;
    }

    if (num_sensors <= 0 || sampling_time <= 0 || duration <= 0)
    {
        logError("task1", 2, "invalid argument");
        return 1;
    }

    // Generate simulation data
    time_t current_time = time(NULL);
    time_t start_time = current_time - duration * 3600; // Convert duration to seconds
    srand((unsigned int)time(NULL));                    // Initialize random seed

    FILE *file = fopen("dust_sensor.csv", "w");
    if (file == NULL)
    {
        logError("task1", 3, "dust_sensor.csv access denied");
        return 1;
    }

    fprintf(file, "id,time,value\n");

    time_t timestamp = start_time;

    while (timestamp <= current_time)
    {
        // Generate simulated measurement timestamp in the format YYYY:MM:DD hh:mm:ss
        struct tm *tm_info = localtime(&timestamp);
        char timestamp_str[20];
        strftime(timestamp_str, sizeof(timestamp_str), "%Y:%m:%d %H:%M:%S", tm_info);
        for (int i = 1; i <= num_sensors; i++)
        {
            // Generate simulated measurement value
            double measurement = generateRandomValue(0.0, 1000.0);

            // Write data to file
            fprintf(file, "%d,%s,%.1f\n", i, timestamp_str, measurement);
        }
        timestamp += sampling_time;
    }

    fclose(file);
    printf("Simulation data generated and stored in dust_sensor.csv\n");

    return 0;
}
