#!/bin/bash

echo "Compiling..."
gcc -g -DNUMA_NODES=2 -std=c11 -o bo_bench bo_bench.c -pthread -lrt
gcc -g -DNUMA_NODES=2 -std=c11 -o rw_bo_mcs_bench rw_bo_mcs_bench.c -pthread -lrt
gcc -g -DNUMA_NODES=2 -std=c11 -o pthread_bench pthread_bench.c -pthread -lrt
echo "Done!!!"
