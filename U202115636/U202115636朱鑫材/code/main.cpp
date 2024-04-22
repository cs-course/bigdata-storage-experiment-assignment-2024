
#include "argparse/argparse.hpp"
#include "init.h"
#include <chrono>

int32_t test_size = 500000;
uint32_t stash_size = 200;
uint32_t max_loop = 20;
uint32_t BLOCK_SIZE = 2;
int32_t hashTypes = 4;
int32_t time_limit = 10;
uint32_t cnt = 0;
CuckooType cuckoo_type = CuckooType::Cuckoo;
std::vector<uint32_t> insert_rates;
int count = 0;
int count2 = 0;


void handle_alarm(int signal) {
  if (signal == SIGALRM) {
    //  for (auto rate: insert_rates) {
    //     std::cout << rate << std::endl;
    // }
    std::cout << std::setprecision(2) << std::fixed
              << "Load factor: " << ((double)(cnt * 100) / test_size) << "%"
              << std::endl;
    std::cout << count2 <<" " <<count<< std::endl;
    exit(0);
  }
}

void print_info() {
  std::cout << "Cuckoo type: " << cuckoo_type_map[cuckoo_type] << std::endl;
  std::cout << "Test size: " << test_size << std::endl;
  std::cout << "Stash size: " << stash_size << std::endl;
  std::cout << "Max loop: " << max_loop << std::endl;
  std::cout << "Block size: " << BLOCK_SIZE << std::endl;
  std::cout << "Hash function nums: " << hashTypes << std::endl;
  std::cout << "Time limit: " << time_limit << std::endl;
  std::cout << "---------------------------------" << std::endl;
}

int main(int argc, const char **argv) {
  argparse::ArgumentParser program("Cuckoo Hashing");
  program.add_argument("-h", "--help")
      .help("Print this help message")
      .default_value(false)
      .implicit_value(true)
      .action([&program](const std::string &value) {
        std::cout << program;
        exit(0);
      });
  program.add_argument("-c", "--cuckoo")
      .help("Cuckoo type")
      .default_value("cuckoo")
      .action([](const std::string &value) {
        if (value == "cuckoo") {
          cuckoo_type = CuckooType::Cuckoo;
        } else if (value == "stash_cuckoo") {
          cuckoo_type = CuckooType::StashCuckoo;
        } else if (value == "blocked_cuckoo") {
          cuckoo_type = CuckooType::BlockedCuckoo;
        } else if (value == "stash_blocked_cuckoo") {
          cuckoo_type = CuckooType::StashBlockedCuckoo;
        }
      });
  program.add_argument("-n", "--hash_types")
      .default_value(2)
      .help("hash types")
      .action([](const std::string &value) { hashTypes = std::stoi(value); });
  program.add_argument("-l", "--max_loop")
      .help("max loop")
      .default_value(5)
      .action([](const std::string &value) { max_loop = std::stoi(value); });

  program.add_argument("-t", "--test_size")
      .help("test size")
      .action([](const std::string &value) { test_size = std::stoi(value); });
  program.add_argument("-s", "--stash_size")
      .help("stash size")
      .action([](const std::string &value) { stash_size = std::stoi(value); });
  program.add_argument("-b", "--block_size")
      .help("block size")
      .action([](const std::string &value) { BLOCK_SIZE = std::stoi(value); });
  program.add_argument("-T", "--time_limit")
      .help("time limit")
      .default_value(10)
      .action([](const std::string &value) { time_limit = std::stoi(value); });
  program.parse_args(argc, argv);
  print_info();
  std::unordered_set<uint32_t> keys;

  insert_rates.reserve(test_size);
  std::signal(SIGALRM, handle_alarm);
  auto cuckoo = build_cuckoo(cuckoo_type, test_size, hashTypes);
  cuckoo->set_max_loop(max_loop);
  alarm(time_limit);

  for (int i = 0; i < test_size; i++) {
    auto start = std::chrono::high_resolution_clock::now();
    uint32_t key = DataGenerater::generate();
    if (keys.count(key)) {
      i--;
      continue;
    }
    while (!cuckoo->insert(key)) {
      int reconstruct_cnt = 0;
      while (!cuckoo->rehash()) {
        reconstruct_cnt++;
        if (reconstruct_cnt > test_size / 10) {
          handle_alarm(SIGALRM);
        }
      }
    }
    keys.insert(key);
    cnt++;
    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    auto insert_time = duration.count();
    insert_rates.push_back(insert_time);
  }
  handle_alarm(SIGALRM);
  return 0;
}