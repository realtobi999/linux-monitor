#include "cpu.h"
#include "unity.h"
#include "stdio.h"
#include "strings.h"

void setUp(void) {}
void tearDown(void) {}

void test_cpu_read_snapshots_fails_when_null_args(void)
{
    size_t snapshots_size = 0;
    cpu_snapshot_t snapshots [MAX_SNAPSHOT];
    TEST_ASSERT_FALSE(cpu_read_snapshots(NULL, NULL));
    TEST_ASSERT_FALSE(cpu_read_snapshots(NULL, &snapshots_size));
    TEST_ASSERT_FALSE(cpu_read_snapshots(snapshots, NULL));
}

void test_cpu_read_snapshots_works(void)
{
    size_t snapshots_size = 0;
    cpu_snapshot_t snapshots [MAX_SNAPSHOT];
    TEST_ASSERT_TRUE(cpu_read_snapshots(snapshots, &snapshots_size));
}

void test_cpu_get_snapshot(void)
{
    cpu_snapshot_t snapshot;
    TEST_ASSERT_TRUE(cpu_get_snapshot(&snapshot, -1) == true);
    TEST_ASSERT_TRUE(snapshot.core_number == -1);
    TEST_ASSERT_TRUE(cpu_get_snapshot(&snapshot, 0) == true);
    TEST_ASSERT_TRUE(snapshot.core_number == 0);
    TEST_ASSERT_TRUE(cpu_get_snapshot(&snapshot, -2) == false);
}

void test_cpu_get_core_snapshot(void)
{
    cpu_snapshot_t snapshot;

    TEST_ASSERT_TRUE(cpu_get_core_snapshot(&snapshot) == true);
    TEST_ASSERT_TRUE(snapshot.core_number == -1);
}

void test_cpu_calculate_usage_fails_when_null_args(void)
{
    cpu_snapshot_t current_snapshot;
    cpu_snapshot_t previous_snapshot;

    TEST_ASSERT_TRUE(cpu_calculate_usage(current_snapshot, previous_snapshot, NULL) == false);
}

void test_cpu_calculate_usage_fails_when_cores_doesnt_match(void)
{
    cpu_snapshot_usage_t cpu_usage;
    cpu_snapshot_t current_snapshot = (cpu_snapshot_t)
    {
        .core_number = -1,
        .fields_count = 2,
        .fields = {10, 20}
    };
    cpu_snapshot_t previous_snapshot = (cpu_snapshot_t)
    {
        .core_number = 1,
        .fields_count = 2,
        .fields = {5, 10}
    };

    TEST_ASSERT_TRUE(cpu_calculate_usage(current_snapshot, previous_snapshot, &cpu_usage) == false);
}

void test_cpu_calculate_usage_fails_when_previous_snapshot_has_more_fields(void)
{
    cpu_snapshot_usage_t cpu_usage;
    cpu_snapshot_t current_snapshot = (cpu_snapshot_t)
    {
        .core_number = -1,
        .fields_count = 2,
        .fields = {10, 20}
    };
    cpu_snapshot_t previous_snapshot = (cpu_snapshot_t)
    {
        .core_number = -1,
        .fields_count = 3,
        .fields = {10, 20, 30}
    };

    TEST_ASSERT_TRUE(cpu_calculate_usage(current_snapshot, previous_snapshot, &cpu_usage) == false);
}

void test_cpu_calculate_usage_fails_when_previous_snapshot_has_bigger_field_value(void)
{
    cpu_snapshot_usage_t cpu_usage;
    cpu_snapshot_t current_snapshot = (cpu_snapshot_t)
    {
        .core_number = -1,
        .fields_count = 2,
        .fields = {10, 20}
    };
    cpu_snapshot_t previous_snapshot = (cpu_snapshot_t)
    {
        .core_number = -1,
        .fields_count = 2,
        .fields = {5, 40}
    };

    TEST_ASSERT_TRUE(cpu_calculate_usage(current_snapshot, previous_snapshot, &cpu_usage) == false);
}

void test_cpu_calculate_usage_fails_when_zero_total_delta(void)
{
    cpu_snapshot_usage_t cpu_usage;
    cpu_snapshot_t current_snapshot = (cpu_snapshot_t)
    {
        .core_number = -1,
        .fields_count = 2,
        .fields = {10, 20}
    };
    cpu_snapshot_t previous_snapshot = (cpu_snapshot_t)
    {
        .core_number = -1,
        .fields_count = 2,
        .fields = {10, 20}
    };

    TEST_ASSERT_TRUE(cpu_calculate_usage(current_snapshot, previous_snapshot, &cpu_usage) == false);
}

void test_cpu_calculate_usage_works(void)
{
    cpu_snapshot_usage_t cpu_usage;
    cpu_snapshot_t current_snapshot = (cpu_snapshot_t)
    {
        .core_number = -1,
        .fields_count = 4,
        .fields = {10, 20, 22, 55}
    };
    cpu_snapshot_t previous_snapshot = (cpu_snapshot_t)
    {
        .core_number = -1,
        .fields_count = 4,
        .fields = {5, 10, 12, 45}
    };

    TEST_ASSERT_TRUE(cpu_calculate_usage(current_snapshot, previous_snapshot, &cpu_usage) == true);
    TEST_ASSERT_FLOAT_WITHIN(0.71f, 0.73f, cpu_usage.usage);
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_cpu_read_snapshots_fails_when_null_args);
    RUN_TEST(test_cpu_read_snapshots_works);
    RUN_TEST(test_cpu_get_snapshot);
    RUN_TEST(test_cpu_get_core_snapshot);
    RUN_TEST(test_cpu_calculate_usage_fails_when_cores_doesnt_match);
    RUN_TEST(test_cpu_calculate_usage_fails_when_null_args);
    RUN_TEST(test_cpu_calculate_usage_fails_when_previous_snapshot_has_more_fields);
    RUN_TEST(test_cpu_calculate_usage_fails_when_previous_snapshot_has_bigger_field_value);
    RUN_TEST(test_cpu_calculate_usage_fails_when_zero_total_delta);
    RUN_TEST(test_cpu_calculate_usage_works);

    return UNITY_END();
}