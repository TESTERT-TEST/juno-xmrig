#include "junocash/JunocashTemplate.h"
#include "3rdparty/rapidjson/document.h"
#include "base/io/json/Json.h"
#include <algorithm>
#include <cassert>
#include <cstring>

using namespace xmrig;

static inline uint32_t read_bits_hex(const char* hex)
{
    // parse compact bits from hex string
    return static_cast<uint32_t>(strtoul(hex, nullptr, 16));
}

static void compact_to_target(uint32_t compact, uint8_t out[32])
{
    // Bitcoin-style compact representation to 256-bit target
    memset(out, 0, 32);
    uint32_t exponent = compact >> 24;
    uint32_t mantissa = compact & 0x007fffffU;
    bool neg = (compact & 0x00800000U) != 0;
    (void)neg;
    int offset = (int)exponent - 3;
    if (offset < 0) offset = 0;
    if (offset + 3 > 32) offset = 29; // clamp
    // target is big-endian in usual display; store as big-endian here
    uint8_t m[4] = { uint8_t(mantissa >> 16), uint8_t(mantissa >> 8), uint8_t(mantissa) , 0 };
    for (int i = 0; i < 3; ++i) {
        int pos = 32 - (offset + (3 - i));
        if (pos >= 0 && pos < 32) out[pos] = m[i];
    }
}

static inline uint8_t from_hex_pair(char h, char l)
{
    auto v = [](char c)->int{ if (c>='0'&&c<='9') return c-'0'; if (c>='a'&&c<='f') return c-'a'+10; if (c>='A'&&c<='F') return c-'A'+10; return 0; };
    return (uint8_t)((v(h)<<4)|v(l));
}

static std::vector<uint8_t> hex_to_bytes(const std::string &hex)
{
    std::vector<uint8_t> out;
    out.reserve(hex.size()/2);
    for (size_t i = 0; i + 1 < hex.size(); i += 2) out.push_back(from_hex_pair(hex[i], hex[i+1]));
    return out;
}

static inline void write_le32(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)(v & 0xff);
    p[1] = (uint8_t)((v >> 8) & 0xff);
    p[2] = (uint8_t)((v >> 16) & 0xff);
    p[3] = (uint8_t)((v >> 24) & 0xff);
}

bool JunocashTemplate::parse(const rapidjson::Value &tpl)
{
#ifndef SUPPORT_JUNOCASH
    (void)tpl;
    return false;
#else
    using namespace rapidjson;
    if (!tpl.IsObject()) return false;

    version = Json::getUint(tpl, "version");
    previous_block_hash = Json::getString(tpl, "previousblockhash");
    time = Json::getUint(tpl, "curtime");
    height = Json::getUint(tpl, "height");

    const char* bits_hex = Json::getString(tpl, "bits");
    bits = read_bits_hex(bits_hex ? bits_hex : "");

    seed_height = Json::getUint64(tpl, "randomxseedheight");
    auto seed_hex = Json::getString(tpl, "randomxseedhash");
    if (!seed_hex) return false;
    auto seed_bytes = hex_to_bytes(seed_hex);
    if (seed_bytes.size() != 32) return false;
    std::copy(seed_bytes.begin(), seed_bytes.end(), seed_hash.begin());

    has_next_seed_hash = false;
    if (tpl.HasMember("randomxnextseedhash")) {
        auto next = Json::getString(tpl, "randomxnextseedhash");
        if (next) {
            auto next_bytes = hex_to_bytes(next);
            if (next_bytes.size() == 32) {
                std::copy(next_bytes.begin(), next_bytes.end(), next_seed_hash.begin());
                has_next_seed_hash = true;
            }
        }
    }

    if (tpl.HasMember("defaultroots") && tpl["defaultroots"].IsObject()) {
        const Value& roots = tpl["defaultroots"];
        merkle_root = Json::getString(roots, "merkleroot");
        block_commitments_hash = Json::getString(roots, "blockcommitmentshash");
    }
    if (block_commitments_hash.empty() && tpl.HasMember("blockcommitmentshash")) {
        block_commitments_hash = Json::getString(tpl, "blockcommitmentshash");
    }

    coinbase_txn_hex = Json::getString(tpl, "coinbasetxn", "data");
    if (tpl.HasMember("transactions") && tpl["transactions"].IsArray()) {
        for (auto& v : tpl["transactions"].GetArray()) {
            auto hex = Json::getString(v, "data");
            if (hex && *hex) txn_hex.emplace_back(hex);
        }
    }

    // Build target
    compact_to_target(bits, target.data());

    // Build header_base
    size_t offset = 0;
    write_le32(header_base.data() + offset, version); offset += 4;

    auto prev = hex_to_bytes(previous_block_hash); std::reverse(prev.begin(), prev.end());
    if (prev.size() != 32) return false; std::copy(prev.begin(), prev.end(), header_base.begin() + offset); offset += 32;

    auto merkle = hex_to_bytes(merkle_root); std::reverse(merkle.begin(), merkle.end());
    if (merkle.size() != 32) return false; std::copy(merkle.begin(), merkle.end(), header_base.begin() + offset); offset += 32;

    auto commits = hex_to_bytes(block_commitments_hash); std::reverse(commits.begin(), commits.end());
    if (commits.size() != 32) return false; std::copy(commits.begin(), commits.end(), header_base.begin() + offset); offset += 32;

    write_le32(header_base.data() + offset, time); offset += 4;
    write_le32(header_base.data() + offset, bits); offset += 4;

    if (offset != 108) return false;
    std::fill(header_base.begin() + 108, header_base.end(), 0);

    return true;
#endif
}
