#!/bin/sh
NEUTRAL=0
READER=1
WRITER=2
READER_OPT=3

BENCHMARK="pthread_bench"	# Name of the benchmark executable
THREADS=32		# Number of threads to be used
PREFERENCE=$NEUTRAL	# Preference 
WCSLEN=100		# time to be elapsed in the critical section when in read-write mode
RCSLEN=100		# time to be elapsed in the critical section when in read-only mode
NCSLEN=100		# time to be elapsed in the non-critical section
PROBABILITY=10		# probability to enter the CS in read-write mode
RUNTIME=10000		# Runtime in ms
ARRAYSIZE=10000		# Array Size

echo -e "\nExecuting Benchmark: $BENCHMARK"
echo -e "Threads:\t $THREADS"
echo -e "Runtime:\t $RUNTIME milliseconds"
echo -e "Probability:\t $PROBABILITY %"
RESULT=$("./$BENCHMARK" $WCSLEN $RCSLEN $NCSLEN $PROBABILITY $THREADS $PREFERENCE $RUNTIME $ARRAYSIZE)
echo -e "\nThroughput: $RESULT\n"