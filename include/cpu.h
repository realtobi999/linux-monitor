#ifndef CPU_H
#define CPU_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#define MAX_CPU_FIELDS 10
#define MAX_SNAPSHOT 50

/// @brief Raw CPU counters for one core or for the aggregate CPU.
typedef struct cpu_snapshot {
    /// Zero-based core number, or the aggregate CPU identifier (-1).
    int core_number;
    /// Number of valid counters stored in fields.
    size_t fields_count;
    /// CPU counters. Only the first fields_count entries are valid.
    uint64_t fields [MAX_CPU_FIELDS];
} cpu_snapshot_t;

/// @brief Calculated CPU usage associated with a core or aggregate CPU.
typedef struct cpu_snapshot_usage {
    /// Zero-based core number, or the aggregate CPU identifier (-1).
    int core_number;
    /// Usage. Decimal value.
    double usage;
} cpu_snapshot_usage_t;

/// @brief Reads CPU statistics for the specified core.
/// @param snapshot Pointer to the snapshot to populate.
/// @param core_number Zero-based core number to read.
/// @return True if the snapshot was read successfully, False otherwise.
bool cpu_get_snapshot(cpu_snapshot_t* snapshot, int core_number);

/// @brief Reads aggregate CPU statistics across all cores.
/// @param snapshot Pointer to the snapshot to populate.
/// @return True if the snapshot was read successfully, False otherwise.
bool cpu_get_core_snapshot(cpu_snapshot_t* snapshot);

/// @brief Reads a snapshot of CPU statistics into the provided array.
/// @param snapshots Buffer of cpu_snapshot_t entries to populate. The maximum number of entries is MAX_CPU_FIELDS.
/// @param snapshots_size Pointer to the number of entries available in the array on input and the number actually written on output.
/// @return True if the snapshot was read successfully, False otherwise.
bool cpu_read_snapshots(cpu_snapshot_t snapshots [], size_t* snapshots_size);

/// @brief Calculates CPU usage for a core from two snapshots.
/// @param current_snapshot The latest CPU statistics snapshot.
/// @param previous_snapshot The earlier CPU statistics snapshot used for comparison.
/// @param usage Pointer to the structure to populate calculated usage.
/// @return True if the calculation was done successfully, False otherwise.
bool cpu_calculate_usage(cpu_snapshot_t current_snapshot, cpu_snapshot_t previous_snapshot, cpu_snapshot_usage_t* usage);

#endif