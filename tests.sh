#!/bin/bash

# Files to store expected outputs
EXPECTED_OUTPUTS="expected_outputs.txt"
LOG_FILE="logs_webserv"

# Start the web server in the background and redirect its output to a log file
./webserv > "$LOG_FILE" 2>&1 &
SERVER_PID=$!
sleep 2 #Gives some time to the server to be launched.
echo $SERVER_PID

# Function to stop the server with SIGQUIT
stop_server() {
    echo "Stopping the web server..."
    kill -SIGQUIT $SERVER_PID
    wait $SERVER_PID 2>/dev/null
    echo "Server stopped."
}

# Function to compare output of a single test
compare_output() {
    local test_name=$1
    local actual_output=$2

    # Extract the expected output for the current test
    local expected_output=$(grep "^$test_name:" "$EXPECTED_OUTPUTS" | cut -d':' -f2 | xargs)

    # Compare actual and expected outputs
    if [ "$actual_output" == "$expected_output" ]; then
        echo "$test_name passed. Output: $actual_output"
    else
        echo "$test_name failed!"
        echo "  Expected: $expected_output"
        echo "  Actual:   $actual_output"
    fi
}

# Run tests sequentially and compare outputs immediately
echo "Starting tests..."
(
    echo "Running Test 1..."
    output=$(curl -s -o /dev/null -w "%{http_code}" http://localhost:8080/)
    echo "Test 1 completed with HTTP code: $output"
    compare_output "Test 1" "$output"
)

(
    echo "Running Test 2..."
    output=$(curl -s -o /dev/null -w "%{http_code}" -X POST -d "data=example" http://localhost:8080/test2)
    echo "Test 2 completed with HTTP code: $output"
    compare_output "Test 2" "$output"
)

(
    echo "Running Test 3..."
    output=$(curl -s -o /dev/null -w "%{http_code}" http://localhost:8080/test3)
    echo "Test 3 completed with HTTP code: $output"
    compare_output "Test 3" "$output"
)

# Stop the server after tests
stop_server

echo "All tests completed."
