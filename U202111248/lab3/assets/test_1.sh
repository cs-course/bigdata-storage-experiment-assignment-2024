#!/bin/bash
cd ./big-data

export END_POINT=http://ip:port

cargo test --package big-data --test concurrent -- test_1 --exact --nocapture > test_1.log

