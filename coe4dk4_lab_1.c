
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

/*******************************************************************************/

/*
 * Simulation Parameters
 */

// #define RANDOM_SEED 5259140
// #define NUMBER_TO_SERVE 10e2

// #define SERVICE_TIME 1
#define ARRIVAL_RATE 0.1

#define BLIP_RATE 10000

/*
 * Student num ends in 7, thus service time = 1+7 
 */
#define SERVICE_TIME 8.0

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

/*******************************************************************************/

/*
 * run_simulation()
 *
 * Runs one M/D/1 simulation.
 *
 * The queueing logic is based directly on the supplied
 * coe4dk4_lab_1.c program.
 *
 * Inputs:
 *
 * random_seed     - random number generator seed
 * number_to_serve - number of customers to serve
 * arrival_rate    - mean arrival rate, lambda
 *
 */

SimulationResult run_simulation(
    unsigned random_seed,
    long int number_to_serve,
    double arrival_rate
)
{
    double clock = 0.0;

    /*
     * System state variables.
     */

    int number_in_system = 0;

    double next_arrival_time = 0.0;
    double next_departure_time = 0.0;

    /*
     * Data collection variables.
     */

    long int total_served = 0;
    long int total_arrived = 0;

    double total_busy_time = 0.0;
    double integral_of_n = 0.0;
    double last_event_time = 0.0;

    SimulationResult result;

    /*
     * Set the random number generator seed.
     */

    random_generator_initialize(random_seed);

    /*
     * Continue until NUMBER_TO_SERVE customers
     * have completed service.
     */

    while (total_served < number_to_serve)
    {
        /*
         * Determine whether the next event is
         * an arrival or departure.
         */

        if (
            number_in_system == 0 ||
            next_arrival_time < next_departure_time
        )
        {
            /***********************************************************/
            /*
             * ARRIVAL EVENT
             */

            clock = next_arrival_time;

            /*
             * Generate next arrival.
             *
             * Poisson arrivals have exponentially distributed
             * inter-arrival times.
             */

            next_arrival_time =
                clock +
                exponential_generator(1.0 / arrival_rate);

            /*
             * Update statistics.
             */

            integral_of_n +=
                number_in_system *
                (clock - last_event_time);

            last_event_time = clock;

            /*
             * Customer enters the system.
             */

            number_in_system++;
            total_arrived++;

            /*
             * If the customer arrived to an empty system,
             * immediately start service.
             *
             * This is an M/D/1 system, so every service
             * time is exactly SERVICE_TIME.
             */

            if (number_in_system == 1)
            {
                next_departure_time =
                    clock + SERVICE_TIME;
            }
        }

        else
        {
            /***********************************************************/
            /*
             * DEPARTURE EVENT
             */

            clock = next_departure_time;

            /*
             * Update statistics.
             */

            integral_of_n +=
                number_in_system *
                (clock - last_event_time);

            last_event_time = clock;

            /*
             * Customer leaves system.
             */

            number_in_system--;
            total_served++;

            /*
             * Server was busy for exactly SERVICE_TIME.
             */

            total_busy_time += SERVICE_TIME;

            /*
             * If another customer is waiting,
             * start serving them immediately.
             */

            if (number_in_system > 0)
            {
                next_departure_time =
                    clock + SERVICE_TIME;
            }
        }
    }

    /******************************************************************/
    /*
     * Calculate final simulation statistics.
     */

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

/*******************************************************************************/

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
    /*
     * Required simulation run lengths:
     *
     * 10^2
     * 10^4
     * 10^6
     */

    const long int run_lengths[] = {
        100L,
        10000L,
        1000000L
    };

    const int number_of_run_lengths = 3;

    /*
     * We define three representative traffic loads.
     *
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

    /*
     * Loop over:
     *
     * simulation run length
     * traffic load
     * random seed
     */

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
            /*
             * rho = lambda * X
             *
             * Therefore:
             *
             * lambda = rho / X
             */

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
                        arrival_rate
                    );

                /*
                 * Save results.
                 */

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
    /*
     * Stable traffic loads.
     *
     * These provide points from very low traffic
     * all the way to near-critical operation.
     */

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

    /*
     * Use a long run for final performance curves.
     */

    const long int number_to_serve =
        1000000L;

    FILE *raw_file;
    FILE *average_file;

    int load_index;
    int seed_index;

    /*
     * Raw results contain one row for every
     * individual random seed.
     */

    raw_file =
        fopen(
            "part2_raw_results.csv",
            "w"
        );

    /*
     * Averaged results contain one row for
     * each arrival rate.
     */

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

    /*
     * Run each stable traffic load.
     */

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
         *
         * rho = lambda * SERVICE_TIME
         */

        arrival_rate =
            traffic_loads[load_index] /
            SERVICE_TIME;

        /*
         * Repeat using several random seeds.
         */

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
                    arrival_rate
                );

            /*
             * Save individual run.
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

            /*
             * Add to averages.
             */

            sum_utilization +=
                result.utilization;

            sum_mean_number +=
                result.mean_number;

            sum_mean_delay +=
                result.mean_delay;
        }

        /*
         * Calculate averages.
         */

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
 * main() uses various simulation parameters and creates a clock variable to
 * simulate real time. A loop repeatedly determines if the next event to occur
 * is a customer arrival or customer departure. In either case the state of the
 * system is updated and statistics are collected before the next
 * iteration. When it finally reaches NUMBER_TO_SERVE customers, the program
 * outputs some statistics such as mean delay.
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

    /*
     * Run Part 1.
     */

    run_part1();

    /*
     * Run Part 2.
     */

    // run_part2();

    printf(
        "All Part 1 experiments complete.\n"
    );

    return 0;
}






