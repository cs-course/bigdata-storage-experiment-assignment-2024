#!/bin/bash
cd ./big-data

cargo test --package big-data --test basic_operations -- basic_operations_for_str --exact --nocapture

sleep 5

cargo test --package big-data --test basic_operations -- basic_operations_for_files --exact --nocapture 
