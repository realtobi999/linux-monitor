#include <cpu.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

/*
    Internals - prototypes.
*/

static bool parse_snapshot_fields(cpu_snapshot_t* current_snapshot, char line [], int* i);
static bool parse_snapshot_core_number(size_t snapshots_size, cpu_snapshot_t* current_snapshot, char line [], int* i);

/*
    Internals - implementation.
*/

static bool parse_snapshot_fields(cpu_snapshot_t* current_snapshot, char line[], int *i)
{
    while (line[*i] != '\0')
    {
        while (line[*i] != '\0' && !isdigit((unsigned char)line[*i]))
        {
            *i += 1;
        }

        if (line[*i] == '\0') {
            break;
        }

        if ((*current_snapshot).fields_count >= MAX_CPU_FIELDS)
        {
            return false;
        }

        int number_size = 0;
        char number[20];

        while (isdigit((unsigned char)line[*i]) && number_size < (int)sizeof(number) - 1)
        {
            number[number_size] = line[*i];
            number_size += 1;
            *i += 1;
        }
        number[number_size] = '\0';

        (*current_snapshot).fields[(*current_snapshot).fields_count] = (uint64_t)strtoull(number, NULL, 10);
        (*current_snapshot).fields_count += 1;
    }

    return true;
}

static bool parse_snapshot_core_number(size_t snapshots_size, cpu_snapshot_t* current_snapshot, char line [], int* i)
{
    if (strstr(line, "cpu "))
    {
        (*current_snapshot).core_number = -1;
        *i += 1;
    }
    else if (strstr(line, "cpu") && line[3] != ' ')
    {
        (*current_snapshot).core_number = snapshots_size - 1;
        while (line[*i] != ' ') 
        {
            *i += 1;
        }
    }
    else
    {
        return false;
    }

    return true;
}

/*
    Exposed.
*/

bool cpu_get_snapshot(cpu_snapshot_t* snapshot, int core_number)
{
    if (core_number < -1)
    {
        return false;
    }

    size_t snapshots_size = 0;
    cpu_snapshot_t snapshots[MAX_SNAPSHOT];

    if (!cpu_read_snapshots(snapshots, &snapshots_size))
    {
        return EXIT_FAILURE;
    }

    for (size_t i = 0; i < snapshots_size; i++)
    {
        if (core_number == snapshots[i].core_number)
        {
            *snapshot = snapshots[i];
        }
    }

    return true;
}

bool cpu_get_core_snapshot(cpu_snapshot_t* snapshot)
{
    return cpu_get_snapshot(snapshot, -1);
}

bool cpu_read_snapshots(cpu_snapshot_t snapshots[], size_t *snapshots_size)
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
    if (snapshots == NULL || snapshots_size == NULL)
    {
        return false;
    }

    *snapshots_size = 0;
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

        if (strncmp(line, "cpu", 3) != 0)
        {
            continue;
        }

        int i = 3;

        cpu_snapshot_t current_snapshot = {0};
        if (!parse_snapshot_core_number(*snapshots_size, &current_snapshot, line, &i))
        {
            return false;
        }
        if (!parse_snapshot_fields(&current_snapshot, line, &i))
        {
            return false;
        }

        if (*snapshots_size < MAX_SNAPSHOT)
        {
            snapshots[*snapshots_size] = current_snapshot;
            *snapshots_size += 1;
        }
    }

    fclose(fptr);

    return true;
}

bool cpu_calculate_usage(cpu_snapshot_t current_snapshot, cpu_snapshot_t previous_snapshot, cpu_snapshot_usage_t* snapshot_usage)
{
    /*
        1. Iterate through all CPU statistic fields, calculating the tick  delta
           (difference between current and previous snapshots) for each.
        2. Accumulate the total delta and specific idle deltas (fields 3  and  4
           representing idle and iowait).
        3. If total_delta > 0, compute the  usage  percentage:  ((total_delta  -
           idle_delta) / total_delta) * 100.0.
    */
    
    if (current_snapshot.core_number != previous_snapshot.core_number)
    {
        return false;
    }
    if (snapshot_usage == NULL)
    {
        return false;
    }

    uint64_t idle_delta = 0;
    uint64_t total_delta = 0;

    if (current_snapshot.fields_count < previous_snapshot.fields_count)
    {
        return false;
    }

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
    
    (*snapshot_usage).usage = (double)(total_delta - idle_delta) / total_delta;

    return true;
}
