#include <cpu.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

int main(void)
{
    bool is_first_iteration = true;
    
    cpu_snapshot_t previous_snapshots[MAX_SNAPSHOT];

    while (true)
    {
        /*
            Clear up and wait.
        */
        sleep(1);
        system("clear");
        
        /*
            Read the snapshots.
        */
        size_t current_snapshots_size = 0;
        cpu_snapshot_t current_snapshots[MAX_SNAPSHOT];

        if (!cpu_read_snapshots(current_snapshots, &current_snapshots_size))
        {
            return EXIT_FAILURE;
        }

        if (is_first_iteration)
        {
            is_first_iteration = false;

            /*  
                Set the current snapshot as the previous one for next iteration.
            */
            memcpy(previous_snapshots, current_snapshots, sizeof(current_snapshots));
            continue;
        }

        /*
            Calculate the usage and print it out.
        */
        size_t cpu_usages_size = 0;
        cpu_snapshot_usage_t cpu_usages [MAX_SNAPSHOT];
        for (size_t i = 0; i < current_snapshots_size; i++)
        {
            cpu_snapshot_t current = current_snapshots[i];
            cpu_snapshot_t previous = previous_snapshots[i];

            if (current.core_number != previous.core_number)
            {
                return EXIT_FAILURE;
            }
            
            cpu_snapshot_usage_t cpu_usage;
            cpu_usage.core_number = current.core_number;

            if (!cpu_calculate_usage(current, previous, &cpu_usage))
            {
                return EXIT_FAILURE;
            }

            cpu_usages[cpu_usages_size] = cpu_usage;
            cpu_usages_size += 1;
        }

        for (size_t i = 0; i < cpu_usages_size; i++)
        {
            if (i == 0)
            {
                printf("[*] / core total\t%.2f%%\n\n", cpu_usages[i].usage * 100);
            }
            else
            {
                printf("[*] / core %d.\t\t%.2f%%\n", cpu_usages[i].core_number + 1, cpu_usages[i].usage * 100);
            }
        }
        
        /*
            Set the current snapshot as the previous one for next iteration.
        */
        memcpy(previous_snapshots, current_snapshots, sizeof(current_snapshots));
    }

    return EXIT_SUCCESS;
}