#ifndef CUCKOO_FILTER_H
#define CUCKOO_FILTER_H 1

#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iostream>

#include "CuckooHelpers.h"

// 在以下模板中：
// Key是键类型，它必须是可复制和可移动的，此外，Key必须是默认可构造的（无参数）作为空，
// 并且必须有一个empty()方法来指示实例为空。
// 如果对对象的所有字节使用fasthash64不是一个合适的哈希函数，必须分别提供两种哈希函数类型作为第三和第四个参数进行实例化。
// 如果std::equal_to<Key>没有实现或者行为不正确，也必须提供一个比较类。
// Value是值类型，在模板中实际上并未使用，除了作为Value用于值的输入和输出。
// 模板参数基本上只是出于方便，提供valueAlign和valueSize的默认值并减少类型转换。值作为Value传入和传出，以允许
// 运行时配置字节大小和对齐。在表中不会为Value调用构造函数或析构函数或赋值操作符，数据仅通过std::memcpy复制。
// 所以Value必须只包含POD（纯老数据类型）！

template <class Key, class HashKey = HashWithSeed<Key, 0xdeadbeefdeadbeefULL>,
          class Fingerprint = HashWithSeed<Key, 0xabcdefabcdef1234ULL>,
          class HashShort = HashWithSeed<uint16_t, 0xfedcbafedcba4321ULL>,
          class CompKey = std::equal_to<Key>>
class CuckooFilter {

    static constexpr uint32_t SlotsPerBucket = 4;

  public:
    CuckooFilter(bool useMmap, uint64_t size)
        : _randState(0x2636283625154737ULL), _slotSize(sizeof(uint16_t)),
          _useMmap(useMmap) {

        size *= 2.0;
        size = (size >= 1024) ? size : 1024;

        size /= SlotsPerBucket;
        _size = size;
        _niceSize = 256;
        _logSize = 8;
        while (_niceSize < size) {
            _niceSize <<= 1;
            _logSize += 1;
        }
        _sizeMask = _niceSize - 1;
        _sizeShift = (64 - _logSize) / 2;
        _maxRounds = _size;
        _allocSize = _size * _slotSize * SlotsPerBucket + 64;

        if (_useMmap) {
            char *namePicked = std::tmpnam(_tmpFileName);
            if (namePicked == nullptr) {
                throw;
            }
            _tmpFile =
                open(_tmpFileName, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
            if (_tmpFile == -1) {
                throw;
            }
            try {
                int result = lseek(_tmpFile, _allocSize - 1, SEEK_SET);
                if (result == -1) {
                    throw;
                }
                result = write(_tmpFile, "", 1); // make the file a certain size
                if (result == -1) {
                    throw;
                }

                _allocBase = reinterpret_cast<char *>(
                    mmap(nullptr, _allocSize, PROT_READ | PROT_WRITE,
                         MAP_SHARED, _tmpFile, 0));
                if (_allocBase == MAP_FAILED) {
                    std::cout << "MAP_FAILED in filter" << std::endl;
                    throw;
                }
            } catch (...) {
                close(_tmpFile);
                std::remove(_tmpFileName);
            }
            _base = _allocBase;
        } else {
            _allocBase = new char[_allocSize];

            _base = reinterpret_cast<char *>(
                (reinterpret_cast<uintptr_t>(_allocBase) + 63) &
                ~((uintptr_t)0x3fu));
        }

        for (uint32_t b = 0; b < _size; ++b) {
            for (size_t i = 0; i < SlotsPerBucket; ++i) {
                uint16_t *f = findSlot(b, i);
                *f = 0; //
            }
        }
    }

    ~CuckooFilter() {
        if (_useMmap) {
            munmap(_allocBase, _allocSize);
            close(_tmpFile);
            std::remove(_tmpFileName);
        } else {
            delete[] _allocBase;
        }
    }

    CuckooFilter(CuckooFilter const &) = delete;
    CuckooFilter(CuckooFilter &&) = delete;
    CuckooFilter &operator=(CuckooFilter const &) = delete;
    CuckooFilter &operator=(CuckooFilter &&) = delete;

    bool lookup(Key const &k) const {
        // look up a key, return either false if no pair with key k is
        // found or true.
        uint64_t hash1 = _hasherKey(k);
        uint64_t pos1 = hashToPos(hash1);
        uint16_t fingerprint = keyToFingerprint(k);
        // We compute the second hash already here to allow the result to
        // survive a mispredicted branch in the first loop. Is this sensible?
        uint64_t hash2 = _hasherPosFingerprint(pos1, fingerprint);
        uint64_t pos2 = hashToPos(hash2);

        for (uint64_t i = 0; i < SlotsPerBucket; ++i) {
            uint16_t *fTable = findSlot(pos1, i);
            if (fingerprint == *fTable) {
                return true;
            }
        }
        for (uint64_t i = 0; i < SlotsPerBucket; ++i) {
            uint16_t *fTable = findSlot(pos2, i);
            if (fingerprint == *fTable) {
                return true;
            }
        }
        return false;
    }

    bool insert(Key &k) {

        uint16_t *fTable;

        uint64_t hash1 = _hasherKey(k);
        uint64_t pos1 = hashToPos(hash1);
        uint16_t fingerprint = keyToFingerprint(k);

        uint64_t hash2 = _hasherPosFingerprint(pos1, fingerprint);
        uint64_t pos2 = hashToPos(hash2);

        for (uint64_t i = 0; i < SlotsPerBucket; ++i) {
            fTable = findSlot(pos1, i);
            if (!*fTable) {
                *fTable = fingerprint;
                ++_nrUsed;
                return true;
            }
        }
        for (uint64_t i = 0; i < SlotsPerBucket; ++i) {
            fTable = findSlot(pos2, i);
            if (!*fTable) {
                *fTable = fingerprint;
                ++_nrUsed;
                return true;
            }
        }

        uint8_t r = pseudoRandomChoice();
        if ((r & 1) != 0) {
            std::swap(pos1, pos2);
        }
        for (unsigned attempt = 0; attempt < _maxRounds; attempt++) {
            std::swap(pos1, pos2);

            r = pseudoRandomChoice();
            uint64_t i = r & (SlotsPerBucket - 1);

            fTable = findSlot(pos1, i);
            uint16_t fDummy = *fTable;
            *fTable = fingerprint;
            fingerprint = fDummy;

            hash2 = _hasherPosFingerprint(pos1, fingerprint);
            pos2 = hashToPos(hash2);

            for (uint64_t i = 0; i < SlotsPerBucket; ++i) {
                fTable = findSlot(pos2, i);
                if (!*fTable) {
                    *fTable = fingerprint;
                    ++_nrUsed;
                    return true;
                }
            }
        }

        return false;
    }

    bool remove(Key const &k) {

        uint64_t hash1 = _hasherKey(k);
        uint64_t pos1 = hashToPos(hash1);
        uint16_t fingerprint = keyToFingerprint(k);

        uint64_t hash2 = _hasherPosFingerprint(pos1, fingerprint);
        uint64_t pos2 = hashToPos(hash2);
        for (uint64_t i = 0; i < SlotsPerBucket; ++i) {
            uint16_t *fTable = findSlot(pos1, i);
            if (fingerprint == *fTable) {
                *fTable = 0;
                _nrUsed--;
                return true;
            }
        }
        for (uint64_t i = 0; i < SlotsPerBucket; ++i) {
            uint16_t *fTable = findSlot(pos2, i);
            if (fingerprint == *fTable) {
                *fTable = 0;
                _nrUsed--;
                return true;
            }
        }
        return false;
    }

    uint64_t capacity() const { return _size * SlotsPerBucket; }

    uint64_t nrUsed() const { return _nrUsed; }

    uint64_t memoryUsage() const { return sizeof(CuckooFilter) + _allocSize; }

  private: // methods
    uint16_t *findSlot(uint64_t pos, uint64_t slot) const {
        char *address = _base + _slotSize * (pos * SlotsPerBucket + slot);
        auto ret = reinterpret_cast<uint16_t *>(address);
        return ret;
    }

    uint64_t hashToPos(uint64_t hash) const {
        uint64_t relevantBits = (hash >> _sizeShift) & _sizeMask;
        return ((relevantBits < _size) ? relevantBits : (relevantBits - _size));
    }

    uint16_t keyToFingerprint(Key const &k) const {
        uint64_t hash = _fingerprint(k);
        uint16_t fingerprint =
            (uint16_t)((hash ^ (hash >> 16) ^ (hash >> 32) ^ (hash >> 48)) &
                       0xFFFF);
        return (fingerprint ? fingerprint : 1);
    }

    uint64_t _hasherPosFingerprint(uint64_t pos, uint16_t fingerprint) const {
        return ((pos << _sizeShift) ^ _hasherShort(fingerprint));
    }

    uint8_t pseudoRandomChoice() {
        _randState = _randState * 997 + 17; // ignore overflows
        return static_cast<uint8_t>((_randState >> 37) & 0xff);
    }

  private:               // member variables
    uint64_t _randState; // pseudo random state for expunging

    size_t _slotSize; // total size of a slot

    uint64_t _logSize;   // logarithm (base 2) of number of buckets
    uint64_t _size;      // actual number of buckets
    uint64_t _niceSize;  // smallest power of 2 at least number of buckets, ==
                         // 2^_logSize
    uint64_t _sizeMask;  // used to mask out some bits from the hash
    uint32_t _sizeShift; // used to shift the bits down to get a position
    uint64_t _allocSize; // number of allocated bytes,
                         // == _size * SlotsPerBucket * _slotSize + 64
    bool _useMmap;
    char *_base; // pointer to allocated space, 64-byte aligned
    char _tmpFileName[L_tmpnam + 1];
    int _tmpFile;
    char *_allocBase;    // base of original allocation
    uint64_t _nrUsed;    // number of pairs stored in the table
    unsigned _maxRounds; // maximum number of cuckoo rounds on insertion

    HashKey _hasherKey;       // Instance to compute the first hash function
    Fingerprint _fingerprint; // Instance to compute a fingerprint of a key
    HashShort _hasherShort;   // Instance to compute the second hash function
    CompKey _compKey;         // Instance to compare keys
};

#endif