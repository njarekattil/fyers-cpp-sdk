#include "fyers_optimized.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <vector>
#include <numeric>
#include <algorithm>

int main() {
    FyersClient client(FyersCredentials::APP_ID, FyersCredentials::ACCESS_TOKEN);

    std::vector<std::string> symbols = {
        "NSE:SBIN-EQ",
        "NSE:RELIANCE-EQ",
        "NSE:TCS-EQ"
    };

    std::cout << "========================================\n";
    std::cout << "C++ 100-REQUEST BENCHMARK TEST\n";
    std::cout << "========================================\n\n";

    std::vector<double> request_times;
    request_times.reserve(100);

    auto total_start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 100; i++) {
        auto start = std::chrono::high_resolution_clock::now();
        std::string response = client.get_quote(symbols);
        auto end = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double> diff = end - start;
        double request_time = diff.count();
        request_times.push_back(request_time);

        // Progress indicator every 10 requests
        if ((i + 1) % 10 == 0) {
            std::cout << "Completed " << (i + 1) << " requests... "
                << "Last request: " << std::fixed << std::setprecision(4)
                << request_time << "s" << std::endl;
        }
    }

    auto total_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> total_diff = total_end - total_start;

    // Calculate statistics
    double total_time = total_diff.count();
    double sum = std::accumulate(request_times.begin(), request_times.end(), 0.0);
    double average = sum / request_times.size();

    double min_time = *std::min_element(request_times.begin(), request_times.end());
    double max_time = *std::max_element(request_times.begin(), request_times.end());

    // Calculate median
    std::vector<double> sorted_times = request_times;
    std::sort(sorted_times.begin(), sorted_times.end());
    double median = sorted_times[sorted_times.size() / 2];

    // Calculate standard deviation
    double sq_sum = 0.0;
    for (double time : request_times) {
        sq_sum += (time - average) * (time - average);
    }
    double std_dev = std::sqrt(sq_sum / request_times.size());

    // First request (cold start)
    double first_request = request_times[0];

    // Average of requests 2-100 (warm)
    double warm_sum = std::accumulate(request_times.begin() + 1, request_times.end(), 0.0);
    double warm_average = warm_sum / (request_times.size() - 1);

    // Print detailed results
    std::cout << "\n========================================\n";
    std::cout << "RESULTS\n";
    std::cout << "========================================\n\n";

    std::cout << std::fixed << std::setprecision(4);
    std::cout << "Total Requests:       100\n";
    std::cout << "Total Time:           " << total_time << " seconds\n";
    std::cout << "Average per Request:  " << average << " seconds (" << (average * 1000) << " ms)\n\n";

    std::cout << "First Request:        " << first_request << " seconds (" << (first_request * 1000) << " ms)\n";
    std::cout << "Warm Average (2-100): " << warm_average << " seconds (" << (warm_average * 1000) << " ms)\n\n";

    std::cout << "Min Time:             " << min_time << " seconds (" << (min_time * 1000) << " ms)\n";
    std::cout << "Max Time:             " << max_time << " seconds (" << (max_time * 1000) << " ms)\n";
    std::cout << "Median Time:          " << median << " seconds (" << (median * 1000) << " ms)\n";
    std::cout << "Std Deviation:        " << std_dev << " seconds (" << (std_dev * 1000) << " ms)\n\n";

    // Breakdown by request ranges
    std::cout << "========================================\n";
    std::cout << "BREAKDOWN BY REQUEST RANGES\n";
    std::cout << "========================================\n\n";

    auto calc_range_avg = [&](int start, int end) {
        double range_sum = 0.0;
        for (int i = start; i < end && i < 100; i++) {
            range_sum += request_times[i];
        }
        return range_sum / (end - start);
        };

    std::cout << "Requests 1-10:    " << (calc_range_avg(0, 10) * 1000) << " ms\n";
    std::cout << "Requests 11-20:   " << (calc_range_avg(10, 20) * 1000) << " ms\n";
    std::cout << "Requests 21-30:   " << (calc_range_avg(20, 30) * 1000) << " ms\n";
    std::cout << "Requests 31-40:   " << (calc_range_avg(30, 40) * 1000) << " ms\n";
    std::cout << "Requests 41-50:   " << (calc_range_avg(40, 50) * 1000) << " ms\n";
    std::cout << "Requests 51-60:   " << (calc_range_avg(50, 60) * 1000) << " ms\n";
    std::cout << "Requests 61-70:   " << (calc_range_avg(60, 70) * 1000) << " ms\n";
    std::cout << "Requests 71-80:   " << (calc_range_avg(70, 80) * 1000) << " ms\n";
    std::cout << "Requests 81-90:   " << (calc_range_avg(80, 90) * 1000) << " ms\n";
    std::cout << "Requests 91-100:  " << (calc_range_avg(90, 100) * 1000) << " ms\n\n";

    // Performance verdict
    std::cout << "========================================\n";
    std::cout << "PERFORMANCE ANALYSIS\n";
    std::cout << "========================================\n\n";

    if (warm_average < 0.030) {
        std::cout << "Status: EXCELLENT! Sustained performance < 30ms\n";
    }
    else if (warm_average < 0.040) {
        std::cout << "Status: GOOD! Sustained performance < 40ms\n";
    }
    else if (warm_average < 0.050) {
        std::cout << "Status: ACCEPTABLE! Sustained performance < 50ms\n";
    }
    else {
        std::cout << "Status: NEEDS IMPROVEMENT! Sustained performance > 50ms\n";
    }

    double improvement = (first_request / warm_average);
    std::cout << "Connection Reuse Improvement: " << std::setprecision(2) << improvement << "x faster\n";

    if (std_dev < 0.005) {
        std::cout << "Consistency: EXCELLENT! Very stable response times\n";
    }
    else if (std_dev < 0.010) {
        std::cout << "Consistency: GOOD! Stable response times\n";
    }
    else {
        std::cout << "Consistency: VARIABLE! Network might be unstable\n";
    }

    std::cout << "\n========================================\n";

    return 0;
}