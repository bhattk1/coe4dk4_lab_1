
/*
 *
 * Simulation of Single Server Queueing System
 * 
 * Copyright (C) 2014 Terence D. Todd Hamilton, Ontario, CANADA,
 * todd@mcmaster.ca
 * 
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 3 of the License, or (at your option) any later
 * version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

/*******************************************************************************/

#include <stdio.h>
#include "simlib.h"
#include <stdlib.h> // The finite queue will store arrival times dynamically

/*******************************************************************************/

/*
 * Simulation Parameters
 */

// #define RANDOM_SEED 5259140
// #define NUMBER_TO_SERVE 10e2

#define ARRIVAL_RATE 0.1

#define BLIP_RATE 10000

/*
 * Student num ends in 7, thus service time = 1+7 
 */
#define SERVICE_TIME 8.0
/*
 * Part 4 requires the service time to be doubled.
 */
#define PART4_SERVICE_TIME (2.0 * SERVICE_TIME)

/*
 * Five random seeds are used so that we can examine
 * sensitivity to RANDOM_SEED and average results.
 */

static const unsigned RANDOM_SEEDS[] = {
    5259140U,
    1357911U,
    2468022U,
    3141592U,
    8675309U
};

#define NUM_SEEDS 5

typedef struct {

    double utilization;
    double fraction_served;
    double mean_number;
    double mean_delay;

    double final_clock;

    long total_arrived;
    long total_served;

    int final_number_in_system;

} SimulationResult;

typedef struct
{
    double utilization;
    double mean_number;
    double mean_delay;
    double rejection_probability;

    double final_clock;

    long total_arrived;
    long total_rejected;
    long total_served;

    int final_number_in_system;

} FiniteQueueResult;

SimulationResult run_simulation(
    unsigned random_seed,
    long int number_to_serve,
    double arrival_rate,
    double service_time
)
{
    double clock = 0.0;

    int number_in_system = 0;

    double next_arrival_time = 0.0;
    double next_departure_time = 0.0;

    long int total_served = 0;
    long int total_arrived = 0;

    double total_busy_time = 0.0;
    double integral_of_n = 0.0;
    double last_event_time = 0.0;

    SimulationResult result;

    random_generator_initialize(random_seed);

    while (total_served < number_to_serve)
    {
        if (
            number_in_system == 0 ||
            next_arrival_time < next_departure_time
        )
        {
            clock = next_arrival_time;

            next_arrival_time =
                clock +
                exponential_generator(1.0 / arrival_rate);

            integral_of_n +=
                number_in_system *
                (clock - last_event_time);

            last_event_time = clock;

            number_in_system++;
            total_arrived++;

            if (number_in_system == 1)
            {
                next_departure_time =
                    clock + service_time;
            }
        }

        else
        {
            clock = next_departure_time;

            integral_of_n +=
                number_in_system *
                (clock - last_event_time);

            last_event_time = clock;

            number_in_system--;
            total_served++;

            total_busy_time += service_time;

            if (number_in_system > 0)
            {
                next_departure_time =
                    clock + service_time;
            }
        }
    }

    result.utilization =
        total_busy_time / clock;

    result.fraction_served =
        (double) total_served /
        (double) total_arrived;

    result.mean_number =
        integral_of_n / clock;

    result.mean_delay =
        integral_of_n /
        (double) total_served;

    result.final_clock =
        clock;

    result.total_arrived =
        total_arrived;

    result.total_served =
        total_served;

    result.final_number_in_system =
        number_in_system;

    return result;
}

SimulationResult run_simulation_mm1(
    unsigned random_seed,
    long int number_to_serve,
    double arrival_rate,
    double service_time
)
{
    double clock = 0.0;

    int number_in_system = 0;

    double next_arrival_time = 0.0;
    double next_departure_time = 0.0;

    double current_service_time = 0.0;

    long int total_served = 0;
    long int total_arrived = 0;

    double total_busy_time = 0.0;
    double integral_of_n = 0.0;
    double last_event_time = 0.0;

    SimulationResult result;

    random_generator_initialize(random_seed);

    while (total_served < number_to_serve)
    {
        if (
            number_in_system == 0 ||
            next_arrival_time < next_departure_time
        )
        {
            clock = next_arrival_time;

            next_arrival_time =
                clock +
                exponential_generator(1.0 / arrival_rate);

            integral_of_n +=
                number_in_system *
                (clock - last_event_time);

            last_event_time = clock;

            number_in_system++;
            total_arrived++;

            if (number_in_system == 1)
            {
                current_service_time =
                    exponential_generator(
                        (double) service_time
                    );

                next_departure_time =
                    clock +
                    current_service_time;
            }
        }

        else
        {

            clock = next_departure_time;

            integral_of_n +=
                number_in_system *
                (clock - last_event_time);

            last_event_time = clock;

            number_in_system--;
            total_served++;

            total_busy_time +=
                current_service_time;

            if (number_in_system > 0)
            {
                current_service_time =
                    exponential_generator(
                        (double) service_time
                    );

                next_departure_time =
                    clock +
                    current_service_time;
            }
        }
    }

    result.utilization =
        total_busy_time / clock;

    result.fraction_served =
        (double) total_served /
        (double) total_arrived;

    result.mean_number =
        integral_of_n / clock;

    result.mean_delay =
        integral_of_n /
        (double) total_served;

    result.final_clock =
        clock;

    result.total_arrived =
        total_arrived;

    result.total_served =
        total_served;

    result.final_number_in_system =
        number_in_system;

    return result;
}

FiniteQueueResult run_simulation_finite_md1(
    unsigned random_seed,
    long int number_to_serve,
    double arrival_rate,
    double service_time,
    int max_queue_size
)
{
    double clock = 0.0;

    int number_in_system = 0;

    double next_arrival_time = 0.0;
    double next_departure_time = 0.0;

    long int total_arrived = 0;
    long int total_rejected = 0;
    long int total_served = 0;

    double total_busy_time = 0.0;
    double integral_of_n = 0.0;
    double last_event_time = 0.0;

    double total_delay_of_served = 0.0;

    int system_capacity =
        max_queue_size + 1;

    double *arrival_times =
        (double *) malloc(
            system_capacity *
            sizeof(double)
        );

    int queue_head = 0;
    int queue_tail = 0;

    FiniteQueueResult result;

    if (arrival_times == NULL)
    {
        printf(
            "ERROR: Could not allocate memory "
            "for finite queue simulation.\n"
        );

        exit(EXIT_FAILURE);
    }

    random_generator_initialize(
        random_seed
    );

    while (
        total_served <
        number_to_serve
    )
    {

        if (
            number_in_system == 0 ||
            next_arrival_time <
            next_departure_time
        )
        {
            clock =
                next_arrival_time;

            next_arrival_time =
                clock +
                exponential_generator(
                    1.0 /
                    arrival_rate
                );

            integral_of_n +=
                number_in_system *
                (
                    clock -
                    last_event_time
                );

            last_event_time =
                clock;

            total_arrived++;

            if (
                number_in_system >=
                system_capacity
            )
            {
                total_rejected++;
            }

            else
            {
                arrival_times[
                    queue_tail
                ] =
                    clock;

                queue_tail =
                    (
                        queue_tail + 1
                    ) %
                    system_capacity;

                number_in_system++;

                if (
                    number_in_system == 1
                )
                {
                    next_departure_time =
                        clock +
                        service_time;
                }
            }
        }

        else
        {
            clock =
                next_departure_time;

            integral_of_n +=
                number_in_system *
                (
                    clock -
                    last_event_time
                );

            last_event_time =
                clock;

            total_delay_of_served +=
                clock -
                arrival_times[
                    queue_head
                ];

            queue_head =
                (
                    queue_head + 1
                ) %
                system_capacity;

            number_in_system--;

            total_served++;

            total_busy_time +=
                service_time;

            if (
                number_in_system > 0
            )
            {
                next_departure_time =
                    clock +
                    service_time;
            }
        }
    }

    result.utilization =
        total_busy_time /
        clock;

    result.mean_number =
        integral_of_n /
        clock;

    result.mean_delay =
        total_delay_of_served /
        (double) total_served;

    result.rejection_probability =
        (double) total_rejected /
        (double) total_arrived;

    result.final_clock =
        clock;

    result.total_arrived =
        total_arrived;

    result.total_rejected =
        total_rejected;

    result.total_served =
        total_served;

    result.final_number_in_system =
        number_in_system;

    free(
        arrival_times
    );

    return result;
}

/*
 * PART 1
 *
 * Investigate the effect of:
 *
 * - random seed
 * - simulation run length
 * - traffic load
 *
 * for:
 *
 * clearly stable
 * near-critical
 * unstable
 *
 * systems.
 */

void run_part1(void)
{
    const long int run_lengths[] = {
        100L,
        10000L,
        1000000L
    };

    const int number_of_run_lengths = 3;

    /*
     * define three representative traffic loads
     * rho = 0.50 -> clearly stable
     * rho = 0.95 -> near-critical
     * rho = 1.10 -> unstable
     */

    const double traffic_loads[] = {
        0.50,
        0.95,
        1.10
    };

    const char *region_names[] = {
        "clearly_stable",
        "near_critical",
        "unstable"
    };

    const int number_of_loads = 3;

    FILE *file;

    int run_index;
    int load_index;
    int seed_index;

    /*
     * Create results CSV.
     */

    file = fopen(
        "part1_results.csv",
        "w"
    );

    if (file == NULL)
    {
        printf(
            "ERROR: Could not create "
            "part1_results.csv\n"
        );

        return;
    }

    /*
     * CSV headings.
     */

    fprintf(
        file,
        "region,"
        "rho,"
        "arrival_rate,"
        "service_time,"
        "seed,"
        "number_to_serve,"
        "utilization,"
        "fraction_served,"
        "mean_number,"
        "mean_delay,"
        "final_clock,"
        "total_arrived,"
        "total_served,"
        "final_number_in_system\n"
    );

    printf("\n");
    printf("========================================\n");
    printf("PART 1\n");
    printf("========================================\n\n");

    for (
        run_index = 0;
        run_index < number_of_run_lengths;
        run_index++
    )
    {
        for (
            load_index = 0;
            load_index < number_of_loads;
            load_index++
        )
        {
            double arrival_rate =
                traffic_loads[load_index] /
                SERVICE_TIME;

            for (
                seed_index = 0;
                seed_index < NUM_SEEDS;
                seed_index++
            )
            {
                SimulationResult result;

                result =
                    run_simulation(
                        RANDOM_SEEDS[seed_index],
                        run_lengths[run_index],
                        arrival_rate,
                        SERVICE_TIME
                    );

                fprintf(
                    file,

                    "%s,"
                    "%.5f,"
                    "%.8f,"
                    "%.2f,"
                    "%u,"
                    "%ld,"
                    "%.10f,"
                    "%.10f,"
                    "%.10f,"
                    "%.10f,"
                    "%.10f,"
                    "%ld,"
                    "%ld,"
                    "%d\n",

                    region_names[load_index],

                    traffic_loads[load_index],

                    arrival_rate,

                    SERVICE_TIME,

                    RANDOM_SEEDS[seed_index],

                    run_lengths[run_index],

                    result.utilization,

                    result.fraction_served,

                    result.mean_number,

                    result.mean_delay,

                    result.final_clock,

                    result.total_arrived,

                    result.total_served,

                    result.final_number_in_system
                );

                /*
                 * Print important values to screen.
                 */

                printf(
                    "N = %-7ld | "
                    "rho = %.2f | "
                    "lambda = %.5f | "
                    "seed = %-7u | "
                    "delay = %.6f | "
                    "util = %.6f\n",

                    run_lengths[run_index],

                    traffic_loads[load_index],

                    arrival_rate,

                    RANDOM_SEEDS[seed_index],

                    result.mean_delay,

                    result.utilization
                );
            }

            printf("\n");
        }
    }

    fclose(file);

    printf(
        "Part 1 complete.\n"
        "Results saved to part1_results.csv\n\n"
    );
}
/*******************************************************************************/

/*
 * PART 2
 *
 * Consider only stable systems:
 *
 * rho < 1
 *
 * Generate mean delay versus ARRIVAL_RATE.
 *
 * At each ARRIVAL_RATE, average the simulation
 * results across several random seeds.
 */

void run_part2(void)
{
    const double traffic_loads[] = {
        0.02,
        0.05,
        0.10,
        0.20,
        0.30,
        0.40,
        0.50,
        0.60,
        0.70,
        0.80,
        0.90,
        0.95,
        0.98
    };

    const int number_of_loads = 13;

    const long int number_to_serve =
        1000000L;

    FILE *raw_file;
    FILE *average_file;

    int load_index;
    int seed_index;

    raw_file =
        fopen(
            "part2_raw_results.csv",
            "w"
        );

    average_file =
        fopen(
            "part2_averages.csv",
            "w"
        );

    if (
        raw_file == NULL ||
        average_file == NULL
    )
    {
        printf(
            "ERROR: Could not create "
            "Part 2 result files.\n"
        );

        if (raw_file != NULL)
        {
            fclose(raw_file);
        }

        if (average_file != NULL)
        {
            fclose(average_file);
        }

        return;
    }

    /*
     * Raw CSV headings.
     */

    fprintf(
        raw_file,
        "rho,"
        "arrival_rate,"
        "service_time,"
        "seed,"
        "number_to_serve,"
        "utilization,"
        "mean_number,"
        "mean_delay\n"
    );

    /*
     * Average CSV headings.
     */

    fprintf(
        average_file,
        "rho,"
        "arrival_rate,"
        "service_time,"
        "number_to_serve,"
        "number_of_seeds,"
        "average_utilization,"
        "average_mean_number,"
        "average_mean_delay\n"
    );

    printf("\n");
    printf("========================================\n");
    printf("PART 2\n");
    printf("========================================\n\n");

    for (
        load_index = 0;
        load_index < number_of_loads;
        load_index++
    )
    {
        double arrival_rate;

        double sum_utilization = 0.0;
        double sum_mean_number = 0.0;
        double sum_mean_delay = 0.0;

        /*
         * Convert rho to lambda.
         * rho = lambda * SERVICE_TIME
         */

        arrival_rate =
            traffic_loads[load_index] /
            SERVICE_TIME;

        for (
            seed_index = 0;
            seed_index < NUM_SEEDS;
            seed_index++
        )
        {
            SimulationResult result;

            result =
                run_simulation(
                    RANDOM_SEEDS[seed_index],
                    number_to_serve,
                    arrival_rate,
                    SERVICE_TIME
                );

            fprintf(
                raw_file,

                "%.5f,"
                "%.8f,"
                "%.2f,"
                "%u,"
                "%ld,"
                "%.10f,"
                "%.10f,"
                "%.10f\n",

                traffic_loads[load_index],

                arrival_rate,

                SERVICE_TIME,

                RANDOM_SEEDS[seed_index],

                number_to_serve,

                result.utilization,

                result.mean_number,

                result.mean_delay
            );

            sum_utilization +=
                result.utilization;

            sum_mean_number +=
                result.mean_number;

            sum_mean_delay +=
                result.mean_delay;
        }

        {
            double average_utilization =
                sum_utilization /
                NUM_SEEDS;

            double average_mean_number =
                sum_mean_number /
                NUM_SEEDS;

            double average_mean_delay =
                sum_mean_delay /
                NUM_SEEDS;

            fprintf(
                average_file,

                "%.5f,"
                "%.8f,"
                "%.2f,"
                "%ld,"
                "%d,"
                "%.10f,"
                "%.10f,"
                "%.10f\n",

                traffic_loads[load_index],

                arrival_rate,

                SERVICE_TIME,

                number_to_serve,

                NUM_SEEDS,

                average_utilization,

                average_mean_number,

                average_mean_delay
            );

            printf(
                "rho = %.2f | "
                "lambda = %.5f | "
                "average delay = %.6f | "
                "average utilization = %.6f\n",

                traffic_loads[load_index],

                arrival_rate,

                average_mean_delay,

                average_utilization
            );
        }
    }

    fclose(raw_file);
    fclose(average_file);

    printf("\n");
    printf(
        "Part 2 complete.\n"
        "Raw results: part2_raw_results.csv\n"
        "Averages:    part2_averages.csv\n\n"
    );
}

/*******************************************************************************/

/*
 * PART 3
 *
 * Compare M/D/1 and M/M/1 systems.
 *
 * Both systems have:
 *
 * - the same arrival rate
 * - the same mean service time
 * - the same number of customers served
 *
 * M/D/1 has deterministic service times.
 * M/M/1 has exponentially distributed service times.
 *
 * The required result is:
 *
 * MEAN DELAY vs ARRIVAL RATE
 *
 * for M/D/1 and M/M/1 on the same graph.
 */

void run_part3(void)
{
    const double traffic_loads[] = {
        0.02,
        0.05,
        0.10,
        0.20,
        0.30,
        0.40,
        0.50,
        0.60,
        0.70,
        0.80,
        0.90,
        0.95,
        0.98
    };

    const int number_of_loads = 13;

    const long int number_to_serve =
        1000000L;

    FILE *raw_file;
    FILE *average_file;

    int load_index;
    int seed_index;


    raw_file =
        fopen(
            "part3_raw_results.csv",
            "w"
        );

    average_file =
        fopen(
            "part3_averages.csv",
            "w"
        );

    if (
        raw_file == NULL ||
        average_file == NULL
    )
    {
        printf(
            "ERROR: Could not create "
            "Part 3 result files.\n"
        );

        if (raw_file != NULL)
        {
            fclose(raw_file);
        }

        if (average_file != NULL)
        {
            fclose(average_file);
        }

        return;
    }

    /*
     * Raw results CSV header.
     */

    fprintf(
        raw_file,

        "rho,"
        "arrival_rate,"
        "service_time,"
        "seed,"
        "number_to_serve,"
        "md1_mean_delay,"
        "mm1_mean_delay,"
        "md1_utilization,"
        "mm1_utilization,"
        "md1_mean_number,"
        "mm1_mean_number\n"
    );

    /*
     * Averaged CSV header.
     */

    fprintf(
        average_file,

        "rho,"
        "arrival_rate,"
        "service_time,"
        "number_to_serve,"
        "number_of_seeds,"
        "average_md1_mean_delay,"
        "average_mm1_mean_delay,"
        "average_md1_utilization,"
        "average_mm1_utilization,"
        "average_md1_mean_number,"
        "average_mm1_mean_number\n"
    );

    printf("\n");
    printf("========================================\n");
    printf("PART 3 - M/D/1 vs M/M/1\n");
    printf("========================================\n\n");

    for (
        load_index = 0;
        load_index < number_of_loads;
        load_index++
    )
    {
        double arrival_rate;

        double sum_md1_delay = 0.0;
        double sum_mm1_delay = 0.0;

        double sum_md1_utilization = 0.0;
        double sum_mm1_utilization = 0.0;

        double sum_md1_mean_number = 0.0;
        double sum_mm1_mean_number = 0.0;

        /*
         * rho = lambda * X
         * therefore:
         * lambda = rho / X
         */

        arrival_rate =
            traffic_loads[load_index] /
            SERVICE_TIME;

        for (
            seed_index = 0;
            seed_index < NUM_SEEDS;
            seed_index++
        )
        {
            SimulationResult md1_result;
            SimulationResult mm1_result;

            md1_result =
                run_simulation(
                    RANDOM_SEEDS[seed_index],
                    number_to_serve,
                    arrival_rate,
                    SERVICE_TIME
                );

            mm1_result =
                run_simulation_mm1(
                    RANDOM_SEEDS[seed_index],
                    number_to_serve,
                    arrival_rate,
                    SERVICE_TIME
                );

            fprintf(
                raw_file,

                "%.5f,"
                "%.8f,"
                "%.2f,"
                "%u,"
                "%ld,"
                "%.10f,"
                "%.10f,"
                "%.10f,"
                "%.10f,"
                "%.10f,"
                "%.10f\n",

                traffic_loads[load_index],

                arrival_rate,

                SERVICE_TIME,

                RANDOM_SEEDS[seed_index],

                number_to_serve,

                md1_result.mean_delay,

                mm1_result.mean_delay,

                md1_result.utilization,

                mm1_result.utilization,

                md1_result.mean_number,

                mm1_result.mean_number
            );

            sum_md1_delay +=
                md1_result.mean_delay;

            sum_mm1_delay +=
                mm1_result.mean_delay;

            sum_md1_utilization +=
                md1_result.utilization;

            sum_mm1_utilization +=
                mm1_result.utilization;

            sum_md1_mean_number +=
                md1_result.mean_number;

            sum_mm1_mean_number +=
                mm1_result.mean_number;
        }

        {
            double average_md1_delay =
                sum_md1_delay /
                NUM_SEEDS;

            double average_mm1_delay =
                sum_mm1_delay /
                NUM_SEEDS;

            double average_md1_utilization =
                sum_md1_utilization /
                NUM_SEEDS;

            double average_mm1_utilization =
                sum_mm1_utilization /
                NUM_SEEDS;

            double average_md1_mean_number =
                sum_md1_mean_number /
                NUM_SEEDS;

            double average_mm1_mean_number =
                sum_mm1_mean_number /
                NUM_SEEDS;

            /*
             * Save averaged results.
             */

            fprintf(
                average_file,

                "%.5f,"
                "%.8f,"
                "%.2f,"
                "%ld,"
                "%d,"
                "%.10f,"
                "%.10f,"
                "%.10f,"
                "%.10f,"
                "%.10f,"
                "%.10f\n",

                traffic_loads[load_index],

                arrival_rate,

                SERVICE_TIME,

                number_to_serve,

                NUM_SEEDS,

                average_md1_delay,

                average_mm1_delay,

                average_md1_utilization,

                average_mm1_utilization,

                average_md1_mean_number,

                average_mm1_mean_number
            );

            /*
             * Print useful comparison to terminal.
             */

            printf(
                "rho = %.2f | "
                "lambda = %.5f | "
                "MD1 delay = %.6f | "
                "MM1 delay = %.6f\n",

                traffic_loads[load_index],

                arrival_rate,

                average_md1_delay,

                average_mm1_delay
            );
        }
    }

    fclose(raw_file);
    fclose(average_file);

    printf("\n");
    printf(
        "Part 3 complete.\n"
        "Raw results: part3_raw_results.csv\n"
        "Averages:    part3_averages.csv\n\n"
    );
}


/*
 * PART 4
 *
 * Repeat the M/D/1 and M/M/1 comparison from Part 3,
 * but DOUBLE the mean service time.
 *
 * Part 3:
 *
 *      SERVICE_TIME = 8
 *
 * Part 4:
 *
 *      SERVICE_TIME = 16
 *
 * We vary ARRIVAL_RATE over stable and near-critical
 * operating regions.
 *
 * The output contains:
 *
 * - mean delay
 * - mean number in system
 * - server utilization
 *
 * for both M/D/1 and M/M/1.
 */

void run_part4(void)
{
    const double traffic_loads[] = {
        0.02,
        0.05,
        0.10,
        0.20,
        0.30,
        0.40,
        0.50,
        0.60,
        0.70,
        0.80,
        0.90,
        0.95,
        0.98
    };

    const int number_of_loads = 13;

    const long int number_to_serve =
        1000000L;

    FILE *raw_file;
    FILE *average_file;

    int load_index;
    int seed_index;

    raw_file =
        fopen(
            "part4_raw_results.csv",
            "w"
        );

    average_file =
        fopen(
            "part4_averages.csv",
            "w"
        );

    if (
        raw_file == NULL ||
        average_file == NULL
    )
    {
        printf(
            "ERROR: Could not create "
            "Part 4 result files.\n"
        );

        if (raw_file != NULL)
        {
            fclose(raw_file);
        }

        if (average_file != NULL)
        {
            fclose(average_file);
        }

        return;
    }

    /*
     * Raw-results CSV header.
     */

    fprintf(
        raw_file,

        "rho,"
        "arrival_rate,"
        "service_time,"
        "seed,"
        "number_to_serve,"
        "md1_mean_delay,"
        "mm1_mean_delay,"
        "md1_mean_number,"
        "mm1_mean_number,"
        "md1_utilization,"
        "mm1_utilization\n"
    );

    /*
     * Averaged-results CSV header.
     */

    fprintf(
        average_file,

        "rho,"
        "arrival_rate,"
        "service_time,"
        "number_to_serve,"
        "number_of_seeds,"
        "average_md1_mean_delay,"
        "average_mm1_mean_delay,"
        "average_md1_mean_number,"
        "average_mm1_mean_number,"
        "average_md1_utilization,"
        "average_mm1_utilization\n"
    );

    printf("\n");
    printf("========================================\n");
    printf("PART 4 - DOUBLE SERVICE TIME\n");
    printf("========================================\n\n");

    printf(
        "Part 3 service time = %.1f\n",
        SERVICE_TIME
    );

    printf(
        "Part 4 service time = %.1f\n",
        PART4_SERVICE_TIME
    );

    printf(
        "Part 4 critical arrival rate = %.6f\n\n",
        1.0 / PART4_SERVICE_TIME
    );

    for (
        load_index = 0;
        load_index < number_of_loads;
        load_index++
    )
    {
        double arrival_rate;

        double sum_md1_delay = 0.0;
        double sum_mm1_delay = 0.0;

        double sum_md1_mean_number = 0.0;
        double sum_mm1_mean_number = 0.0;

        double sum_md1_utilization = 0.0;
        double sum_mm1_utilization = 0.0;

        /*
         * rho = lambda * X
         * therefore:
         * lambda = rho / X
         */

        arrival_rate =
            traffic_loads[load_index] /
            PART4_SERVICE_TIME;

        for (
            seed_index = 0;
            seed_index < NUM_SEEDS;
            seed_index++
        )
        {
            SimulationResult md1_result;
            SimulationResult mm1_result;

            md1_result =
                run_simulation(
                    RANDOM_SEEDS[seed_index],
                    number_to_serve,
                    arrival_rate,
                    PART4_SERVICE_TIME
                );

            mm1_result =
                run_simulation_mm1(
                    RANDOM_SEEDS[seed_index],
                    number_to_serve,
                    arrival_rate,
                    PART4_SERVICE_TIME
                );

            /*
             * Save raw result.
             */

            fprintf(
                raw_file,

                "%.5f,"
                "%.8f,"
                "%.2f,"
                "%u,"
                "%ld,"
                "%.10f,"
                "%.10f,"
                "%.10f,"
                "%.10f,"
                "%.10f,"
                "%.10f\n",

                traffic_loads[load_index],

                arrival_rate,

                PART4_SERVICE_TIME,

                RANDOM_SEEDS[seed_index],

                number_to_serve,

                md1_result.mean_delay,

                mm1_result.mean_delay,

                md1_result.mean_number,

                mm1_result.mean_number,

                md1_result.utilization,

                mm1_result.utilization
            );

            sum_md1_delay +=
                md1_result.mean_delay;

            sum_mm1_delay +=
                mm1_result.mean_delay;

            sum_md1_mean_number +=
                md1_result.mean_number;

            sum_mm1_mean_number +=
                mm1_result.mean_number;

            sum_md1_utilization +=
                md1_result.utilization;

            sum_mm1_utilization +=
                mm1_result.utilization;
        }

        {
            double average_md1_delay =
                sum_md1_delay /
                NUM_SEEDS;

            double average_mm1_delay =
                sum_mm1_delay /
                NUM_SEEDS;

            double average_md1_mean_number =
                sum_md1_mean_number /
                NUM_SEEDS;

            double average_mm1_mean_number =
                sum_mm1_mean_number /
                NUM_SEEDS;

            double average_md1_utilization =
                sum_md1_utilization /
                NUM_SEEDS;

            double average_mm1_utilization =
                sum_mm1_utilization /
                NUM_SEEDS;

            /*
             * Save averaged values.
             */

            fprintf(
                average_file,

                "%.5f,"
                "%.8f,"
                "%.2f,"
                "%ld,"
                "%d,"
                "%.10f,"
                "%.10f,"
                "%.10f,"
                "%.10f,"
                "%.10f,"
                "%.10f\n",

                traffic_loads[load_index],

                arrival_rate,

                PART4_SERVICE_TIME,

                number_to_serve,

                NUM_SEEDS,

                average_md1_delay,

                average_mm1_delay,

                average_md1_mean_number,

                average_mm1_mean_number,

                average_md1_utilization,

                average_mm1_utilization
            );

            /*
             * Display useful results.
             */

            printf(
                "rho = %.2f | "
                "lambda = %.5f | "
                "MD1 delay = %.6f | "
                "MM1 delay = %.6f | "
                "MD1 N = %.6f | "
                "MM1 N = %.6f\n",

                traffic_loads[load_index],

                arrival_rate,

                average_md1_delay,

                average_mm1_delay,

                average_md1_mean_number,

                average_mm1_mean_number
            );
        }
    }

    fclose(raw_file);
    fclose(average_file);

    printf("\n");

    printf(
        "Part 4 complete.\n"
        "Raw results: part4_raw_results.csv\n"
        "Averages:    part4_averages.csv\n\n"
    );
}

/*******************************************************************************/

/*
 * PART 5
 *
 * Analytical comparison.
 *
 * Compare simulated mean delay for:
 *
 *      M/D/1
 *      M/M/1
 *
 * against the queueing-theory equations given
 * in the lab handout.
 *
 *
 * M/D/1:
 *
 *                X(2 - rho)
 * d_MD1 = -------------------------
 *                2(1 - rho)
 *
 *
 * M/M/1:
 *
 *                   X
 * d_MM1 = ----------------
 *                1 - rho
 *
 *
 * These equations are only valid for:
 *
 *      rho < 1
 *
 *
 * The comparison is performed for:
 *
 *      X = 8
 *
 * and
 *
 *      X = 16
 *
 */

void run_part5(void)
{

    const double traffic_loads[] = {
        0.02,
        0.05,
        0.10,
        0.20,
        0.30,
        0.40,
        0.50,
        0.60,
        0.70,
        0.80,
        0.90,
        0.95,
        0.98
    };

    const int number_of_loads = 13;


    const double service_times[] = {
        SERVICE_TIME,
        PART4_SERVICE_TIME
    };

    const int number_of_service_times = 2;

    const long int number_to_serve =
        1000000L;

    FILE *file;

    int service_index;
    int load_index;
    int seed_index;

    /*
     * Create comparison CSV.
     */

    file =
        fopen(
            "part5_analytical_comparison.csv",
            "w"
        );

    if (file == NULL)
    {
        printf(
            "ERROR: Could not create "
            "part5_analytical_comparison.csv\n"
        );

        return;
    }

    /*
     * CSV header.
     */

    fprintf(
        file,

        "service_time,"
        "rho,"
        "arrival_rate,"
        "number_to_serve,"
        "number_of_seeds,"
        "simulated_md1_delay,"
        "theoretical_md1_delay,"
        "md1_percent_error,"
        "simulated_mm1_delay,"
        "theoretical_mm1_delay,"
        "mm1_percent_error\n"
    );

    printf("\n");
    printf("========================================\n");
    printf("PART 5 - ANALYTICAL COMPARISON\n");
    printf("========================================\n\n");


    for (
        service_index = 0;
        service_index < number_of_service_times;
        service_index++
    )
    {
        double service_time =
            service_times[service_index];

        printf(
            "Service time X = %.1f\n\n",
            service_time
        );


        for (
            load_index = 0;
            load_index < number_of_loads;
            load_index++
        )
        {
            double rho =
                traffic_loads[load_index];

            double arrival_rate =
                rho / service_time;

            double sum_md1_delay = 0.0;
            double sum_mm1_delay = 0.0;

            for (
                seed_index = 0;
                seed_index < NUM_SEEDS;
                seed_index++
            )
            {
                SimulationResult md1_result;
                SimulationResult mm1_result;

                md1_result =
                    run_simulation(
                        RANDOM_SEEDS[seed_index],
                        number_to_serve,
                        arrival_rate,
                        service_time
                    );

                mm1_result =
                    run_simulation_mm1(
                        RANDOM_SEEDS[seed_index],
                        number_to_serve,
                        arrival_rate,
                        service_time
                    );

                sum_md1_delay +=
                    md1_result.mean_delay;

                sum_mm1_delay +=
                    mm1_result.mean_delay;
            }

            {
                double simulated_md1_delay =
                    sum_md1_delay /
                    NUM_SEEDS;

                double simulated_mm1_delay =
                    sum_mm1_delay /
                    NUM_SEEDS;


                /*
                 * Analytical M/D/1 mean delay:
                 *
                 * X(2-rho)
                 * ----------
                 * 2(1-rho)
                 */

                double theoretical_md1_delay =
                    service_time *
                    (2.0 - rho) /
                    (
                        2.0 *
                        (1.0 - rho)
                    );


                /*
                 * Analytical M/M/1 mean delay:
                 *
                 * X
                 * -----
                 * 1-rho
                 */

                double theoretical_mm1_delay =
                    service_time /
                    (1.0 - rho);


                /*
                 * Percent error:
                 *
                 * |simulation - theory|
                 * --------------------- x 100
                 *       theory
                 */

                double md1_difference =
                    simulated_md1_delay -
                    theoretical_md1_delay;

                double mm1_difference =
                    simulated_mm1_delay -
                    theoretical_mm1_delay;

                if (md1_difference < 0.0)
                {
                    md1_difference =
                        -md1_difference;
                }

                if (mm1_difference < 0.0)
                {
                    mm1_difference =
                        -mm1_difference;
                }


                {
                    double md1_percent_error =
                        (
                            md1_difference /
                            theoretical_md1_delay
                        ) *
                        100.0;

                    double mm1_percent_error =
                        (
                            mm1_difference /
                            theoretical_mm1_delay
                        ) *
                        100.0;

                    fprintf(
                        file,

                        "%.2f,"
                        "%.5f,"
                        "%.8f,"
                        "%ld,"
                        "%d,"
                        "%.10f,"
                        "%.10f,"
                        "%.6f,"
                        "%.10f,"
                        "%.10f,"
                        "%.6f\n",

                        service_time,

                        rho,

                        arrival_rate,

                        number_to_serve,

                        NUM_SEEDS,

                        simulated_md1_delay,

                        theoretical_md1_delay,

                        md1_percent_error,

                        simulated_mm1_delay,

                        theoretical_mm1_delay,

                        mm1_percent_error
                    );

                    printf(
                        "rho = %.2f | "
                        "MD1 sim = %.4f | "
                        "MD1 theory = %.4f | "
                        "err = %.3f%% | "
                        "MM1 sim = %.4f | "
                        "MM1 theory = %.4f | "
                        "err = %.3f%%\n",

                        rho,

                        simulated_md1_delay,

                        theoretical_md1_delay,

                        md1_percent_error,

                        simulated_mm1_delay,

                        theoretical_mm1_delay,

                        mm1_percent_error
                    );
                }
            }
        }

        printf("\n");
    }

    fclose(file);

    printf(
        "Part 5 complete.\n"
        "Results saved to "
        "part5_analytical_comparison.csv\n\n"
    );
}
/*******************************************************************************/

/*
 * PART 6
 *
 * Finite-capacity M/D/1 queue.
 *
 * For several MAX_QUEUE_SIZE values:
 *
 *      plot mean delay vs ARRIVAL_RATE
 *
 * and
 *
 *      plot rejection probability vs ARRIVAL_RATE
 *
 * Unlike the infinite queue, ARRIVAL_RATE may
 * exceed the rho = 1 threshold.
 */

void run_part6(void)
{
    const int max_queue_sizes[] = {
        1,
        5,
        20
    };

    const int number_of_queue_sizes =
        3;

    const double arrival_rates[] = {
        0.001,
        0.005,
        0.010,
        0.025,
        0.050,
        0.075,
        0.100,
        0.125,
        0.150,
        0.200,
        0.300,
        0.500,
        1.000,
        2.000,
        5.000
    };

    const int number_of_arrival_rates =
        15;

    const long int number_to_serve =
        100000L;

    FILE *raw_file;
    FILE *average_file;

    int queue_index;
    int rate_index;
    int seed_index;

    raw_file =
        fopen(
            "part6_raw_results.csv",
            "w"
        );

    average_file =
        fopen(
            "part6_averages.csv",
            "w"
        );

    if (
        raw_file == NULL ||
        average_file == NULL
    )
    {
        printf(
            "ERROR: Could not create "
            "Part 6 result files.\n"
        );

        if (
            raw_file != NULL
        )
        {
            fclose(
                raw_file
            );
        }

        if (
            average_file != NULL
        )
        {
            fclose(
                average_file
            );
        }

        return;
    }


    /*
     * Raw CSV.
     */

    fprintf(
        raw_file,

        "max_queue_size,"
        "arrival_rate,"
        "rho,"
        "service_time,"
        "seed,"
        "number_to_serve,"
        "mean_delay,"
        "rejection_probability,"
        "mean_number,"
        "utilization,"
        "total_arrived,"
        "total_rejected,"
        "total_served,"
        "final_number_in_system\n"
    );

    /*
     * Averaged CSV.
     */

    fprintf(
        average_file,

        "max_queue_size,"
        "arrival_rate,"
        "rho,"
        "service_time,"
        "number_to_serve,"
        "number_of_seeds,"
        "average_mean_delay,"
        "average_rejection_probability,"
        "average_mean_number,"
        "average_utilization,"
        "theoretical_max_delay,"
        "theoretical_max_rejection\n"
    );

    printf("\n");

    printf(
        "========================================\n"
    );

    printf(
        "PART 6 - FINITE QUEUE\n"
    );

    printf(
        "========================================\n\n"
    );

    /******************************************************************/

    /*
     * Loop through each queue size.
     */

    for (
        queue_index = 0;
        queue_index <
            number_of_queue_sizes;
        queue_index++
    )
    {
        int max_queue_size =
            max_queue_sizes[
                queue_index
            ];


        double theoretical_max_delay =
            (
                max_queue_size + 1
            ) *
            SERVICE_TIME;

        printf(
            "MAX_QUEUE_SIZE = %d | "
            "maximum delay = %.2f\n",

            max_queue_size,

            theoretical_max_delay
        );

        /*
         * Loop through arrival rates.
         */

        for (
            rate_index = 0;
            rate_index <
                number_of_arrival_rates;
            rate_index++
        )
        {
            double arrival_rate =
                arrival_rates[
                    rate_index
                ];

            double rho =
                arrival_rate *
                SERVICE_TIME;

            double sum_mean_delay =
                0.0;

            double sum_rejection_probability =
                0.0;

            double sum_mean_number =
                0.0;

            double sum_utilization =
                0.0;

            /*
             * Repeat using all five seeds.
             */

            for (
                seed_index = 0;
                seed_index <
                    NUM_SEEDS;
                seed_index++
            )
            {
                FiniteQueueResult result;

                result =
                    run_simulation_finite_md1(
                        RANDOM_SEEDS[
                            seed_index
                        ],
                        number_to_serve,
                        arrival_rate,
                        SERVICE_TIME,
                        max_queue_size
                    );

                /*
                 * Raw result.
                 */

                fprintf(
                    raw_file,

                    "%d,"
                    "%.8f,"
                    "%.5f,"
                    "%.2f,"
                    "%u,"
                    "%ld,"
                    "%.10f,"
                    "%.10f,"
                    "%.10f,"
                    "%.10f,"
                    "%ld,"
                    "%ld,"
                    "%ld,"
                    "%d\n",

                    max_queue_size,

                    arrival_rate,

                    rho,

                    SERVICE_TIME,

                    RANDOM_SEEDS[
                        seed_index
                    ],

                    number_to_serve,

                    result.mean_delay,

                    result.rejection_probability,

                    result.mean_number,

                    result.utilization,

                    result.total_arrived,

                    result.total_rejected,

                    result.total_served,

                    result.final_number_in_system
                );

                /*
                 * Add to averages.
                 */

                sum_mean_delay +=
                    result.mean_delay;

                sum_rejection_probability +=
                    result.rejection_probability;

                sum_mean_number +=
                    result.mean_number;

                sum_utilization +=
                    result.utilization;
            }


            {
                double average_mean_delay =
                    sum_mean_delay /
                    NUM_SEEDS;

                double average_rejection_probability =
                    sum_rejection_probability /
                    NUM_SEEDS;

                double average_mean_number =
                    sum_mean_number /
                    NUM_SEEDS;

                double average_utilization =
                    sum_utilization /
                    NUM_SEEDS;

                fprintf(
                    average_file,

                    "%d,"
                    "%.8f,"
                    "%.5f,"
                    "%.2f,"
                    "%ld,"
                    "%d,"
                    "%.10f,"
                    "%.10f,"
                    "%.10f,"
                    "%.10f,"
                    "%.10f,"
                    "%.2f\n",

                    max_queue_size,

                    arrival_rate,

                    rho,

                    SERVICE_TIME,

                    number_to_serve,

                    NUM_SEEDS,

                    average_mean_delay,

                    average_rejection_probability,

                    average_mean_number,

                    average_utilization,

                    theoretical_max_delay,

                    1.0
                );


                printf(
                    "K = %-2d | "
                    "lambda = %-7.3f | "
                    "rho = %-6.2f | "
                    "delay = %-10.4f | "
                    "reject = %.4f\n",

                    max_queue_size,

                    arrival_rate,

                    rho,

                    average_mean_delay,

                    average_rejection_probability
                );
            }
        }

        printf("\n");
    }

    fclose(
        raw_file
    );

    fclose(
        average_file
    );

    printf(
        "Part 6 complete.\n"
        "Raw results: part6_raw_results.csv\n"
        "Averages:    part6_averages.csv\n\n"
    );
}
/*******************************************************************************/
/*
 * main() 
 * HANDOUT STATES: Rather than manually performing each simulation run, you may 
 * prefer to wrap the provided code in loops while changing the various parameters. 
 * The multiple runs can then be automated.
 */

int main(void)
{
    printf("\n");
    printf("COE4DK4 LAB 1\n");

    printf(
        "SERVICE_TIME = %.1f\n",
        SERVICE_TIME
    );

    printf(
        "Critical ARRIVAL_RATE = %.6f\n",
        1.0 / SERVICE_TIME
    );

    printf(
        "Because rho = lambda * X,\n"
        "rho = 1 when lambda = 1/8 = 0.125.\n"
    );

    run_part1();

    run_part2();

    run_part3();

    run_part4();

    run_part5();

    run_part6();

    return 0;
}






