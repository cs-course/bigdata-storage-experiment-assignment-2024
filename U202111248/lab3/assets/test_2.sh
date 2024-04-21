#!/bin/bash
cd ./big-data

export END_POINT=http://ip:port

cargo test --package big-data --test concurrent -- test_2 --exact --nocapture > test_2.log

