#include <cpu.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

bool cpu_read_snapshot(cpu_snapshot_t snapshots[], size_t* snapshots_size)
{
    /*
        1. Open "/proc/stat" and read line by line  until  reaching  the  "intr"
           section.
        2. Filter lines starting with "cpu", distinguishing aggregate stat  from
           individual cores.
        3. Extract space-separated numerical ticks (user,  system,  idle,  etc.)
           into the snapshot fields.
        4. Store the snapshot in the array and increment the size counter.
        5. Close the file and return true.
    */
    FILE* fptr = fopen("/proc/stat", "r");
    if (fptr == NULL) {
        return false;
    }

    char line[256];
    while (fgets(line, sizeof(line), fptr))
    {
        if (strncmp(line, "intr", 4) == 0)
        {
            break;
        }

        if (strncmp(line, "cpu", 3) == 0)
        {
            int i = 3;

            cpu_snapshot_t current_cpu = {0};
            if (line[3] == ' ')
            {
                current_cpu.core_number = -1;
                i += 1;
            }
            else
            {
                current_cpu.core_number = *snapshots_size - 1;

                while (line[i] != '\0' && line[i] != ' ')
                {
                    i += 1;
                }
            }

            while (line[i] != '\0' && current_cpu.fields_count < MAX_CPU_FIELDS)
            {
                while (line[i] != '\0' && !isdigit(line[i]))
                {
                    i += 1;
                }

                if (line[i] == '\0') {
                    break;
                }

                char number[20];
                int number_size = 0;

                while (isdigit(line[i]) && number_size < (int)sizeof(number) - 1)
                {
                    number[number_size] = line[i];
                    number_size += 1;
                    i += 1;
                }
                number[number_size] = '\0';

                current_cpu.fields[current_cpu.fields_count] = (uint64_t)strtoull(number, NULL, 10);
                current_cpu.fields_count += 1;
            }

            if (*snapshots_size < MAX_CPUS)
            {
                snapshots[*snapshots_size] = current_cpu;
                *snapshots_size += 1;
            }
        }
    }

    fclose(fptr);

    return true;
}

bool cpu_calculate_usage(cpu_snapshot_t current_snapshot, cpu_snapshot_t previous_snapshot, cpu_usage_t* snapshot_usage)
{
    /*
        1. Iterate through all CPU statistic fields, calculating the tick  delta
           (difference between current and previous snapshots) for each.
        2. Accumulate the total delta and specific idle deltas (fields 3  and  4
           representing idle and iowait).
        3. If total_delta > 0, compute the  usage  percentage:  ((total_delta  -
           idle_delta) / total_delta) * 100.0.
    */
    uint64_t idle_delta = 0;
    uint64_t total_delta = 0;

    for (size_t j = 0; j < current_snapshot.fields_count; j++)
    {
        if (current_snapshot.fields[j] < previous_snapshot.fields[j])
        {
            return false;
        }

        uint64_t delta = current_snapshot.fields[j] - previous_snapshot.fields[j];
        total_delta += delta;

        if (j == 3 || j == 4)
        {
            idle_delta += delta;
        }
    }

    if (total_delta == 0)
    {
        return false;       
    }
    
    (*snapshot_usage).usage = (double)(total_delta - idle_delta) / total_delta * 100.0;

    return true;
}