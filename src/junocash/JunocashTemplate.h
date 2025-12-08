#pragma once
#include <array>
#include <cstdint>
#include <string>
#include <vector>

#include "3rdparty/rapidjson/fwd.h"

namespace xmrig {

struct JunocashTemplate {
    uint32_t version = 0;
    std::string previous_block_hash;
    std::string merkle_root;
    std::string block_commitments_hash;
    uint32_t time = 0;
    uint32_t bits = 0;
    std::array<uint8_t, 32> target{};
    std::string target_hex;
    uint32_t height = 0;

    uint64_t seed_height = 0;
    std::array<uint8_t, 32> seed_hash{};
    std::array<uint8_t, 32> next_seed_hash{};
    bool has_next_seed_hash = false;

    std::array<uint8_t, 140> header_base{}; // 108 header + 32 zeroed nonce

    std::string coinbase_txn_hex;
    std::vector<std::string> txn_hex;

    bool parse(const rapidjson::Value &tpl);
};

} // namespace xmrig
