#pragma once

#include "../memtable/memtable.h"
#include "../sst/sst.h"
#include "merge_iterator.h"
#include <cstddef>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class LSMEngine {
public:
  std::string data_dir;
  MemTable memtable;
  std::list<size_t> l0_sst_ids;
  std::unordered_map<size_t, std::shared_ptr<SST>> ssts;
  std::shared_mutex ssts_mtx;
  std::shared_ptr<BlockCache> block_cache;

  LSMEngine(std::string path);
  ~LSMEngine();

  std::optional<std::string> get(const std::string &key);

  void put(const std::string &key, const std::string &value);
  void put_batch(const std::vector<std::pair<std::string, std::string>> &kvs);
  void remove(const std::string &key);
  void remove_batch(const std::vector<std::string> &keys);
  void clear();
  void flush();
  void flush_all();

  std::string get_sst_path(size_t sst_id);

  std::optional<std::pair<MergeIterator, MergeIterator>>
  lsm_iters_monotony_predicate(
      std::function<int(const std::string &)> predicate);

  MergeIterator begin();
  MergeIterator end();
};

class LSM {
private:
  LSMEngine engine;

public:
  LSM(std::string path);
  ~LSM();

  std::optional<std::string> get(const std::string &key);

  void put(const std::string &key, const std::string &value);
  void put_batch(const std::vector<std::pair<std::string, std::string>> &kvs);

  void remove(const std::string &key);
  void remove_batch(const std::vector<std::string> &keys);

  using LSMIterator = MergeIterator;
  LSMIterator begin();
  LSMIterator end();
  std::optional<std::pair<MergeIterator, MergeIterator>>
  lsm_iters_monotony_predicate(
      std::function<int(const std::string &)> predicate);
  void clear();
  void flush();
  void flush_all();
};