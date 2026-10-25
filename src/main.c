#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#define CPU_FIELDS 10
#define MAX_CPUS 100

typedef struct cpu_snapshot {
    int core_number;
    int fields_count;
    unsigned long long fields[CPU_FIELDS];
} cpu_snapshot_t;

int main(void)
{
    cpu_snapshot_t prev_cpus[MAX_CPUS];

    int prev_cpus_count = 0;
    int first_iteration = 1;

    while (1)
    {
        sleep(1);

        FILE* fptr = fopen("/proc/stat", "r");
        if (fptr == NULL) {
            perror("Failed to open file");
            return EXIT_FAILURE;
        }
        
        int current_cpus_count = 0;
        cpu_snapshot_t current_cpus[MAX_CPUS];

        char line[256];
        while (fgets(line, sizeof(line), fptr))
        {
            if (strncmp(line, "intr", 4) == 0)
            {
                break;
            }

            if (strncmp(line, "cpu", 3) == 0)
            {
                cpu_snapshot_t current_cpu = {0};
                int i = 3;

                if (line[3] == ' ')
                {
                    current_cpu.core_number = -1;
                    i += 1;
                }
                else
                {
                    current_cpu.core_number = current_cpus_count - 1;

                    while (line[i] != '\0' && line[i] != ' ')
                    {
                        i++;
                    }
                }

                while (line[i] != '\0' && current_cpu.fields_count < CPU_FIELDS)
                {
                    while (line[i] != '\0' && !isdigit(line[i]))
                    {
                        i++;
                    }

                    if (line[i] == '\0') {
                        break;
                    }

                    char number[20];
                    int number_size = 0;

                    while (isdigit(line[i]) && number_size < (int)sizeof(number) - 1)
                    {
                        number[number_size] = line[i];
                        number_size++;
                        i++;
                    }
                    number[number_size] = '\0';

                    current_cpu.fields[current_cpu.fields_count] = strtoull(number, NULL, 10);
                    current_cpu.fields_count++;
                }

                if (current_cpus_count < MAX_CPUS)
                {
                    current_cpus[current_cpus_count] = current_cpu;
                    current_cpus_count++;
                }
            }
        }

        fclose(fptr);

        if (first_iteration)
        {
            memcpy(prev_cpus, current_cpus, sizeof(current_cpus));
            prev_cpus_count = current_cpus_count;
            first_iteration = 0;
            continue;
        }

        system("clear");

        for (size_t i = 0; i < current_cpus_count; i++)
        {
            unsigned long long total_delta = 0;
            unsigned long long idle_delta = 0;

            for (size_t j = 0; j < current_cpus[i].fields_count; j++)
            {
                unsigned long long delta = current_cpus[i].fields[j] - prev_cpus[i].fields[j];

                total_delta += delta;

                if (j == 3 || j == 4)
                {
                    idle_delta += delta;
                }
            }

            if (total_delta > 0)
            {
                double cpu_usage = (double)(total_delta - idle_delta) / total_delta * 100.0;

                printf("CPU %d: %.2f%%\n", (int)(i) - 1, cpu_usage);
            }
        }

        memcpy(prev_cpus, current_cpus, sizeof(current_cpus));
        prev_cpus_count = current_cpus_count;
    }

    return EXIT_SUCCESS;
}