#!/bin/bash
# parse aruguments
build=false
clean=false
draw=false
help=true
check_memory=false
run=false
verify=false
while getopts ":bcdhmrv" opt; do
	case $opt in
		b)
            build=true
            help=false
            ;;
        c)
            clean=true
            help=false
            ;;
        d)
			draw=true
            help=false
			;;
        h)
            help=true
            ;;
        m)
            check_memory=true
            help=false
            ;;
        r)
            run=true
            help=false
            ;;
		v)
			verify=true
            help=false
			;;
		\?)
			echo "Invalid option: -$OPTARG" >&2
            echo "Use -h to help"
			exit 1
			;;
	esac
done
if [ "$help" == true ]; then
    echo "Usage: $0 [-b] [-d] [-h] [-m] [-r] [-v]"
    echo "Options:"
    echo "  -b  Build the project"
    echo "  -d  Draw the Nets topology"
    echo "  -h  Show this help message"
    echo "  -m  Check for memory leaks"
    echo "  -r  Run the routing algorithm"
    echo "  -v  Verify the correctness of the output files"
    echo ""
    echo "Note: Options -b, -d, -m, -r, and -v can be accumulated."
    exit 0
fi
# declaring variables
case_dir="./case"
out_dir="./out"
draw_dir="./draw"
log_path="tmp.log"
# clean
if [ "$clean" == true ]; then
    make clean && make clean_out
fi
# build
if [ "$build" == true ]; then
    make -j16
fi
# run
if [ "$run" == true ] && [ "$check_memory" == false ]; then
    fault_files=""
    reroute_files=""
    for testcase in $(ls -1 "$case_dir"/* | sort -V); do
        testname=$(basename "$testcase")
        testnum=$(echo "$testname" | sed 's/^test\([0-9]\+\)\.txt$/\1/')
        echo "Running case $testname"
        ./router "$testcase" "$out_dir/$testname" >"$log_path"
        cat "$log_path" | egrep 'Violation|Total|Reroute'
        if [ $? -ne 0 ]; then
            fault_files="$fault_files$testnum, "
        elif grep -q "reroute" "$log_path"; then
            reroute_files="$reroute_files$testnum, "
        fi
    done
    rm -f "$log_path"
    if [ -n "$reroute_files" ]; then
        echo "Reroute at test{$reroute_files}"
    fi
    if [ -n "$fault_files" ]; then
        echo "Fault at test{$fault_files}"
    fi
fi

# memory leak
if [ "$check_memory" == true ]; then
    fault_files=""
    leaks_files=""
    for testcase in $(ls -1 "$case_dir"/* | sort -V); do
        testname=$(basename "$testcase")
        testnum=$(echo "$testname" | sed 's/^test\([0-9]\+\)\.txt$/\1/')
        valgrind --leak-check=full --log-file="$log_path" ./router "$testcase" "$out_dir/$testname" >/dev/null 2>&1
        if [ $? -ne 0 ]; then
            fault_files="$fault_files$testnum, "
        elif grep -q "ERROR SUMMARY: 0 errors" "$log_path"; then
            echo "./$out_dir/$testname No memory leaks detected"
        else
            echo "./$out_dir/$testname Memory leaks detected!"
            leaks_files="$leaks_files$i, "
        fi
    done
    if [ -n "$leaks_files" ]; then
        echo "Memory leak at test{$leaks_files}.txt"
    fi
fi
# verify
if [ "$verify" == true ]; then
    not_pass_files=""
    not_run_files=""
    for testcase in $(ls -1 "$out_dir"/* | sort -V); do
        testname=$(basename "$testcase")
        testnum=$(echo "$testname" | sed 's/^test\([0-9]\+\)\.txt$/\1/')
        if [ ! -e "$out_dir/$testname" ]; then
            not_run_files="$not_run_files$testnum, "
        elif (./verifier "$out_dir/$testname" | grep -q "Error"); then
            echo "Error found in ./$out_dir/$testname."
            echo "Please run the following code to check the log."
            echo "./verifier ./$out_dir/$testname"
            not_pass_files="$not_pass_files$testnum, "
        else
            echo "$out_dir/$testname Passed."
        fi
    done
    if [ -n "$not_run_files" ]; then
        echo "Not run at test{$not_run_files}"
    fi
    if [ -n "$not_pass_files" ]; then
        echo "Failure at test{$not_pass_files}"
    else
        echo "All pass!"
    fi
    
fi

# draw
if [ "$draw" = true ]; then
    if [ ! -d "$out_dir" ]; then
        echo "$out_dir directory does not exist"
    elif [ ! -d "$draw_dir" ]; then
        echo "$draw_dir directory does not exist"
    else
        python3 visual.py -i "$out_dir" -o "$draw_dir"
    fi
fi

# post jobs
if [ -e "$log_path" ]; then
    rm -f "$log_path"
fi