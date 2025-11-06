import time
import subprocess
import sys
import statistics

# Restart Python process to clear all caches
if len(sys.argv) == 1:
    print("Restarting Python to clear connection cache...")
    subprocess.call([sys.executable, __file__, "second_run"])
    sys.exit()

from fyers_apiv3 import fyersModel

client_id = "ZYXABCDEF-100"
access_token = "eyJhbGc******B7ayZGPyuIqRb***KJqaNg"

data = {"symbols": "NSE:SBIN-EQ,NSE:RELIANCE-EQ,NSE:TCS-EQ"}

print("=" * 50)
print("PYTHON 100-REQUEST BENCHMARK TEST")
print("=" * 50)
print()

# Create client and prepare for testing
fyers = fyersModel.FyersModel(client_id=client_id, token=access_token, is_async=False, log_path="")

request_times = []
total_start = time.time()

for i in range(100):
    start_time = time.time()
    response = fyers.quotes(data=data)
    end_time = time.time()

    request_time = end_time - start_time
    request_times.append(request_time)

    # Progress indicator every 10 requests
    if (i + 1) % 10 == 0:
        print(f"Completed {i + 1} requests... Last request: {request_time:.4f}s")

total_end = time.time()

# Calculate statistics
total_time = total_end - total_start
average = sum(request_times) / len(request_times)
min_time = min(request_times)
max_time = max(request_times)
median = statistics.median(request_times)
std_dev = statistics.stdev(request_times)

# First request (cold start)
first_request = request_times[0]

# Average of requests 2-100 (warm)
warm_average = sum(request_times[1:]) / len(request_times[1:])

# Print detailed results
print()
print("=" * 50)
print("RESULTS")
print("=" * 50)
print()

print(f"Total Requests:       100")
print(f"Total Time:           {total_time:.4f} seconds")
print(f"Average per Request:  {average:.4f} seconds ({average * 1000:.2f} ms)")
print()

print(f"First Request:        {first_request:.4f} seconds ({first_request * 1000:.2f} ms)")
print(f"Warm Average (2-100): {warm_average:.4f} seconds ({warm_average * 1000:.2f} ms)")
print()

print(f"Min Time:             {min_time:.4f} seconds ({min_time * 1000:.2f} ms)")
print(f"Max Time:             {max_time:.4f} seconds ({max_time * 1000:.2f} ms)")
print(f"Median Time:          {median:.4f} seconds ({median * 1000:.2f} ms)")
print(f"Std Deviation:        {std_dev:.4f} seconds ({std_dev * 1000:.2f} ms)")
print()

# Breakdown by request ranges
print("=" * 50)
print("BREAKDOWN BY REQUEST RANGES")
print("=" * 50)
print()


def calc_range_avg(start, end):
    return sum(request_times[start:end]) / (end - start)


print(f"Requests 1-10:    {calc_range_avg(0, 10) * 1000:.2f} ms")
print(f"Requests 11-20:   {calc_range_avg(10, 20) * 1000:.2f} ms")
print(f"Requests 21-30:   {calc_range_avg(20, 30) * 1000:.2f} ms")
print(f"Requests 31-40:   {calc_range_avg(30, 40) * 1000:.2f} ms")
print(f"Requests 41-50:   {calc_range_avg(40, 50) * 1000:.2f} ms")
print(f"Requests 51-60:   {calc_range_avg(50, 60) * 1000:.2f} ms")
print(f"Requests 61-70:   {calc_range_avg(60, 70) * 1000:.2f} ms")
print(f"Requests 71-80:   {calc_range_avg(70, 80) * 1000:.2f} ms")
print(f"Requests 81-90:   {calc_range_avg(80, 90) * 1000:.2f} ms")
print(f"Requests 91-100:  {calc_range_avg(90, 100) * 1000:.2f} ms")
print()

# Performance verdict
print("=" * 50)
print("PERFORMANCE ANALYSIS")
print("=" * 50)
print()

if warm_average < 0.030:
    print("Status: EXCELLENT! Sustained performance < 30ms")
elif warm_average < 0.040:
    print("Status: GOOD! Sustained performance < 40ms")
elif warm_average < 0.050:
    print("Status: ACCEPTABLE! Sustained performance < 50ms")
else:
    print("Status: NEEDS IMPROVEMENT! Sustained performance > 50ms")

improvement = first_request / warm_average
print(f"Connection Reuse Improvement: {improvement:.2f}x faster")

if std_dev < 0.005:
    print("Consistency: EXCELLENT! Very stable response times")
elif std_dev < 0.010:
    print("Consistency: GOOD! Stable response times")
else:
    print("Consistency: VARIABLE! Network might be unstable")

print()
print("=" * 50)