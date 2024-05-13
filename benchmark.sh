#!/bin/bash

# Function that receives three arguments (total time, 
# minutes and seconds) and returns the accumulated time in 
# seconds

function accumulate_time() {
	total_time=$1
	minutes=$2
	seconds=$3

	accumulated_time_secs=$(echo "$minutes * 60 + $seconds + $total_time" | bc)
	echo "$accumulated_time_secs"
}

g++ carry_look_ahead.cc -o sequential
g++ carry_look_ahead_par_rec.cc -fopenmp -o parallelRec
g++ carry_look_ahead_iter.cc -o sequentialIter
g++ carry_look_ahead_iter_par.cc -fopenmp -o iterPara

iterations=20

total_time=0.0

## Evalutating sequential reccursive
for i in $(seq 1 $iterations)
do
	# Execute sequential and capture its real time
	real_time=`{ time ./sequential > /dev/null; } 2>&1 | grep real | awk '{print $2}'`
	minutes=${real_time%m*}
	seconds=${real_time#*m}
	seconds=${seconds%s*}
	total_time=$(accumulate_time "$total_time" "$minutes" "$seconds")
done

average_time=$(echo "scale=3; $total_time / $iterations" | bc)
echo "Average time for sequential execution is: $average_time"

## Evalutating parallel recursive
for i in $(seq 1 $iterations)
do 
	# Execute parallel and capture its real time	
	real_time=`{ time ./parallelRec > /dev/null; } 2>&1 | grep real | awk '{print $2}'`
	minutes=${real_time%m*}
	seconds=${real_time#*m}
	seconds=${seconds%s*}
	total_time=$(accumulate_time "$total_time" "$minutes" "$seconds")
done

average_time=$(echo "scale=3; $total_time / $iterations" | bc)
echo "Average time for parallel recursive (using tasks) execution is: $average_time"

## Evalutating sequential iterative
for i in $(seq 1 $iterations)
do 
	# Execute parallel and capture its real time	
	real_time=`{ time ./sequentialIter > /dev/null; } 2>&1 | grep real | awk '{print $2}'`
	minutes=${real_time%m*}
	seconds=${real_time#*m}
	seconds=${seconds%s*}
	total_time=$(accumulate_time "$total_time" "$minutes" "$seconds")
done

average_time=$(echo "scale=3; $total_time / $iterations" | bc)
echo "Average time for sequential iterative execution is: $average_time"

## Evalutating parallel iterative
for i in $(seq 1 $iterations)
do 
	# Execute parallel and capture its real time	
	real_time=`{ time ./iterPara > /dev/null; } 2>&1 | grep real | awk '{print $2}'`
	minutes=${real_time%m*}
	seconds=${real_time#*m}
	seconds=${seconds%s*}
	total_time=$(accumulate_time "$total_time" "$minutes" "$seconds")
done

average_time=$(echo "scale=3; $total_time / $iterations" | bc)
echo "Average time for parallel iterative execution is: $average_time"

rm -f sequential parallelRec
