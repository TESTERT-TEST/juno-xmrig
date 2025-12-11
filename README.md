# Juno XMRig
XMRig fork with native Junocash mining support.

## Download
Pre-built binaries are available from the [Releases](https://github.com/juno-cash/juno-xmrig/releases) page:
- **Linux x64**: `xmrig-vX.X.X-linux-x64.zip`
- **macOS x64**: `xmrig-vX.X.X-macos-x64.zip` (requires macOS 14+ Sonoma)
- **macOS ARM64** (Apple Silicon): `xmrig-vX.X.X-macos-arm64.zip` (requires macOS 14+ Sonoma)

## Features
- **Junocash Pool Mining**: Connect to Junocash stratum pools (e.g., pool.juno.ad)
- **280-byte Protocol**: Full support for Junocash's 140-byte header (280 hex chars)
- **32-byte Nonce**: Full 256-bit nonce support (vs standard 4-byte)
- **rx/0 Compatible**: Uses standard RandomX algorithm (same as Monero)

## Quick Start

### Pool Mining
```bash
./xmrig -o pool.example.com:3333 -u j1_YOUR_JUNO_ADDRESS -p x --algo rx/juno
```

### Solo Mining
Connect directly to your Junocash node's RPC port:
```bash
./xmrig -o 127.0.0.1:8232 -u rpcuser -p rpcpassword --coin junocash --daemon
```

Replace `127.0.0.1:8232` with your node's RPC address/port, and `rpcuser`/`rpcpassword` with your node's RPC credentials (configured in `junocash.conf`).

## Protocol Differences from Standard XMRig
| Aspect | Standard (Monero) | Junocash |
|--------|-------------------|----------|
| Blob Size | 76 bytes (152 hex) | 140 bytes (280 hex) |
| Nonce Size | 4 bytes | 32 bytes |
| Nonce Offset | Byte 39 | Bytes 108-139 |
| Algorithm | rx/0 | rx/0 (same) |
| Target | 8-byte LE | 8-byte LE (expanded to 32-byte BE) |

## Building
See [BUILDING.md](BUILDING.md) for detailed build instructions for all platforms.

### Quick Build (Linux/macOS)
```bash
git clone https://github.com/juno-cash/juno-xmrig.git
cd juno-xmrig
mkdir build && cd build
cmake ..
make -j$(nproc)
```

## Documentation
- [BUILDING.md](BUILDING.md) - Detailed build instructions
- [PROTOCOL.md](PROTOCOL.md) - Junocash stratum protocol specification
- [CHANGELOG.md](CHANGELOG.md) - Version history and changes

## Mining Backends
- **CPU** (x86/x64/ARMv7/ARMv8)
- **OpenCL** for AMD GPUs
- **CUDA** for NVIDIA GPUs via external [CUDA plugin](https://github.com/xmrig/xmrig-cuda)

## Based On
- XMRig v6.24.0 (https://github.com/xmrig/xmrig)
- Licensed under GPLv3

## XMRig Original Developers
* **[xmrig](https://github.com/xmrig)**
* **[sech1](https://github.com/SChernykh)**

## License
This project is licensed under the GNU General Public License v3.0 - see the [LICENSE](LICENSE) file for details.
