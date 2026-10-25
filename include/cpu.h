#ifndef CPU_H
#define CPU_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#define MAX_CPU_FIELDS 10
#define MAX_CPUS 50

typedef struct cpu_snapshot {
    int core_number;
    size_t fields_count;
    uint64_t fields [MAX_CPU_FIELDS];
} cpu_snapshot_t;

typedef struct cpu_usage {
    int core_number;
    double usage;
} cpu_usage_t;

/// @brief Reads a snapshot of CPU statistics into the provided array.
/// @param snapshots Buffer of cpu_snapshot_t entries to populate. The maximum number of entries is MAX_CPU_FIELDS.
/// @param snapshots_size Pointer to the number of entries available in the array on input and the number actually written on output.
/// @return True if the snapshot was read successfully, false otherwise.
bool cpu_read_snapshot(cpu_snapshot_t snapshots [], size_t* snapshots_size);

/// @brief Calculates CPU usage for a core from two snapshots.
/// @param core_number Number of the CPU core whose usage is calculated.
/// @param current_snapshot The latest CPU statistics snapshot.
/// @param previous_snapshot The earlier CPU statistics snapshot used for comparison.
/// @return A cpu_usage_t containing the core number and calculated usage.
cpu_usage_t cpu_calculate_usage(int core_number, cpu_snapshot_t current_snapshot, cpu_snapshot_t previous_snapshot);

#endif