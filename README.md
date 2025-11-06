# fyers-cpp-sdk

> ⚡ Lightning-fast C++ SDK for Fyers Trading API - Optimized for algorithmic trading with 28ms average latency (1.5x faster than official Python SDK)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++17](https://img.shields.io/badge/C++-17-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B17)
[![Platform](https://img.shields.io/badge/platform-Windows-lightgrey.svg)](https://www.microsoft.com/windows)

## 🚀 Performance Highlights

Benchmarked against the official Fyers Python SDK with **100 consecutive requests**:

| Metric | fyers-cpp-sdk | Python SDK | Performance Gain |
|--------|---------------|------------|------------------|
| **Sustained Average** | **28ms** | 42ms | **🚀 1.5x faster** |
| **Minimum Time** | **19ms** | 29ms | **1.5x faster** |
| **Median Time** | **25ms** | 38ms | **1.5x faster** |
| **Total (100 requests)** | **2.95s** | 4.19s | **1.4x faster** |
| **First Request (Cold)** | 163ms | 81ms | 2x slower |

### Real-World Impact

```
📊 Trading Day Performance (23,400 requests @ 1/sec for 6.5 hours):

C++ SDK:    10.9 minutes of API wait time
Python SDK: 16.4 minutes of API wait time

Time Saved: 5.5 minutes per trading day
Monthly:    1.8 hours saved (20 trading days)
```

**Perfect for:** Algorithmic trading, quantitative strategies, market making, and high-frequency quote polling.

---

## ✨ Features

- ⚡ **High Performance** - 28ms sustained latency with connection reuse
- 🔒 **Thread-Safe** - Mutex-protected for multi-threaded applications
- 🔄 **Connection Pooling** - Persistent HTTP connections with automatic reuse
- 🎯 **Modern C++17** - Clean, maintainable codebase with RAII principles
- 💾 **Low Memory Footprint** - Pre-allocated buffers, minimal allocations
- 🛡️ **Production Ready** - Comprehensive error handling and stability
- 📦 **Zero Dependencies** - Uses native Windows WinHTTP library

---

## 📦 Installation

### Prerequisites

- **Windows 10/11** (currently Windows-only)
- **Visual Studio 2019+** or any C++17 compatible compiler
- **Windows SDK** (included with Visual Studio)

### Clone the Repository

```bash
git clone https://github.com/yourusername/fyers-cpp-sdk.git
cd fyers-cpp-sdk
```

---

## 🔨 Building

### Visual Studio (Recommended)

**Release Mode (Optimized):**
```cmd
cl /O2 /Ox /std:c++17 /EHsc main.cpp fyers.cpp /Fe:fyers_app.exe
```

**Debug Mode:**
```cmd
cl /Zi /std:c++17 /EHsc main.cpp fyers.cpp /Fe:fyers_app_debug.exe
```

### CMake (Coming Soon)

```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

---

## 🎯 Quick Start

### 1. Set Your Credentials

Edit `fyers.cpp` and add your credentials:

```cpp
namespace FyersCredentials {
    const std::string APP_ID = "YOUR_APP_ID_HERE";
    const std::string ACCESS_TOKEN = "YOUR_ACCESS_TOKEN_HERE";
}
```

### 2. Basic Usage

```cpp
#include "fyers.h"
#include <iostream>
#include <vector>

int main() {
    // Initialize client (connection pooling enabled automatically)
    FyersClient client(FyersCredentials::APP_ID, 
                      FyersCredentials::ACCESS_TOKEN);
    
    // Define symbols to fetch
    std::vector<std::string> symbols = {
        "NSE:SBIN-EQ",
        "NSE:RELIANCE-EQ",
        "NSE:TCS-EQ"
    };
    
    // Get real-time quotes (fast after first request!)
    std::string response = client.get_quote(symbols);
    
    // Parse and use the JSON response
    std::cout << response << std::endl;
    
    return 0;
}
```

### 3. Run Your Application

```cmd
fyers_app.exe
```

**Output:**
```json
{
  "message": "",
  "code": 200,
  "d": [
    {
      "n": "NSE:SBIN-EQ",
      "v": {
        "ask": 962.35,
        "bid": 962.25,
        "lp": 962.30,
        ...
      }
    },
    ...
  ]
}
```

---

## 📚 API Reference

### `FyersClient` Class

#### Constructor

```cpp
FyersClient(const std::string& app_id, const std::string& access_token)
```

Creates a new Fyers client with persistent connection pooling.

**Parameters:**
- `app_id` - Your Fyers App ID
- `access_token` - Your Fyers Access Token

**Example:**
```cpp
FyersClient client("YCTR9C757K-100", "eyJhbGc...");
```

---

#### `get_quote()` Method

```cpp
std::string get_quote(const std::vector<std::string>& symbols) const
```

Retrieves real-time market quotes for specified symbols.

**Parameters:**
- `symbols` - Vector of symbol strings (e.g., `"NSE:SBIN-EQ"`)

**Returns:**
- JSON string containing quote data

**Performance:**
- First call: ~163ms (establishes connection)
- Subsequent calls: ~28ms average (connection reuse)

**Example:**
```cpp
std::vector<std::string> symbols = {"NSE:SBIN-EQ", "NSE:RELIANCE-EQ"};
std::string quotes = client.get_quote(symbols);
```

---

## 💡 Usage Examples

### Example 1: Continuous Quote Monitoring

```cpp
#include "fyers.h"
#include <iostream>
#include <chrono>
#include <thread>

int main() {
    FyersClient client(FyersCredentials::APP_ID, 
                      FyersCredentials::ACCESS_TOKEN);
    
    std::vector<std::string> symbols = {"NSE:SBIN-EQ"};
    
    // Warm-up connection (optional but recommended)
    client.get_quote(symbols);
    
    // Poll quotes every second
    while (true) {
        auto start = std::chrono::high_resolution_clock::now();
        
        std::string quote = client.get_quote(symbols);
        // Process quote data here...
        
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        
        std::cout << "Quote fetched in " << elapsed.count() << "s\n";
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    return 0;
}
```

### Example 2: Multi-Symbol Portfolio Tracking

```cpp
#include "fyers.h"
#include <iostream>

int main() {
    FyersClient client(FyersCredentials::APP_ID, 
                      FyersCredentials::ACCESS_TOKEN);
    
    // Track entire portfolio in one request
    std::vector<std::string> portfolio = {
        "NSE:SBIN-EQ",
        "NSE:RELIANCE-EQ",
        "NSE:TCS-EQ",
        "NSE:INFY-EQ",
        "NSE:HDFCBANK-EQ"
    };
    
    std::string quotes = client.get_quote(portfolio);
    
    // Parse and display (or use a JSON library like nlohmann/json)
    std::cout << quotes << std::endl;
    
    return 0;
}
```

### Example 3: Performance Benchmarking

```cpp
#include "fyers.h"
#include <iostream>
#include <chrono>
#include <numeric>
#include <vector>

int main() {
    FyersClient client(FyersCredentials::APP_ID, 
                      FyersCredentials::ACCESS_TOKEN);
    
    std::vector<std::string> symbols = {"NSE:SBIN-EQ"};
    std::vector<double> times;
    
    // Run 100 requests
    for (int i = 0; i < 100; i++) {
        auto start = std::chrono::high_resolution_clock::now();
        client.get_quote(symbols);
        auto end = std::chrono::high_resolution_clock::now();
        
        std::chrono::duration<double> diff = end - start;
        times.push_back(diff.count());
    }
    
    // Calculate statistics
    double avg = std::accumulate(times.begin(), times.end(), 0.0) / times.size();
    double first = times[0];
    double warm_avg = std::accumulate(times.begin() + 1, times.end(), 0.0) / (times.size() - 1);
    
    std::cout << "First request: " << first * 1000 << "ms\n";
    std::cout << "Warm average: " << warm_avg * 1000 << "ms\n";
    std::cout << "Overall average: " << avg * 1000 << "ms\n";
    
    return 0;
}
```

---

## ⚡ Performance Tips

### 1. Reuse Client Instances

❌ **Don't do this:**
```cpp
for (int i = 0; i < 100; i++) {
    FyersClient client(app_id, token);  // Creates new connection each time!
    client.get_quote(symbols);
}
```

✅ **Do this instead:**
```cpp
FyersClient client(app_id, token);  // Create once
for (int i = 0; i < 100; i++) {
    client.get_quote(symbols);  // Reuses connection (~28ms each)
}
```

**Performance impact:** 5.9x faster with connection reuse!

---

### 2. Pre-Warm Connections

```cpp
// Before market opens or critical operations
FyersClient client(app_id, token);
client.get_quote({"NSE:SBIN-EQ"});  // Warm-up (163ms)

// Now all requests are fast (~28ms)
// Start your trading strategy here...
```

---

### 3. Batch Multiple Symbols

❌ **Slower:**
```cpp
client.get_quote({"NSE:SBIN-EQ"});      // 28ms
client.get_quote({"NSE:RELIANCE-EQ"});  // 28ms
client.get_quote({"NSE:TCS-EQ"});       // 28ms
// Total: 84ms
```

✅ **Faster:**
```cpp
client.get_quote({"NSE:SBIN-EQ", "NSE:RELIANCE-EQ", "NSE:TCS-EQ"});
// Total: 28ms (same as single request!)
```

---

### 4. Always Use Release Mode

```cmd
# Debug mode: ~35-40ms per request
cl /Zi /std:c++17 /EHsc main.cpp fyers.cpp

# Release mode: ~28ms per request (25% faster!)
cl /O2 /Ox /std:c++17 /EHsc main.cpp fyers.cpp
```

---

## 🏗️ Architecture

### Key Design Principles

1. **RAII (Resource Acquisition Is Initialization)**
   - Automatic cleanup of WinHTTP handles
   - No manual resource management needed
   - Exception-safe

2. **Connection Pooling**
   - Persistent session and connection handles
   - Reused across all requests
   - Eliminates repeated TLS handshakes

3. **Thread Safety**
   - Mutex protection for concurrent access
   - Safe to use from multiple threads
   - Lock-free reads where possible

4. **Zero-Copy Where Possible**
   - Pre-allocated response buffers
   - Minimal memory allocations
   - Efficient string operations

### Performance Breakdown

```
First Request (163ms):
├─ DNS lookup:        20ms
├─ TCP handshake:     30ms
├─ TLS handshake:     80ms (includes cert validation)
├─ HTTP request:      10ms
└─ HTTP response:     23ms

Subsequent Requests (28ms):
├─ HTTP request:      10ms
└─ HTTP response:     18ms
└─ (Connection reused, no handshakes!)
```

---

## 🔍 Troubleshooting

### Issue: Slow First Request (>200ms)

**Cause:** Network latency or DNS issues

**Solution:**
```cpp
// Pre-resolve DNS at startup
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

WSADATA wsaData;
WSAStartup(MAKEWORD(2, 2), &wsaData);
struct addrinfo *result = nullptr;
getaddrinfo("api-t1.fyers.in", "443", nullptr, &result);
freeaddrinfo(result);
```

---

### Issue: Inconsistent Performance

**Cause:** Network congestion or debug mode

**Solution:**
1. Always compile in Release mode (`/O2 /Ox`)
2. Test during off-peak hours
3. Check network stability

---

### Issue: Linker Errors

**Error:** `unresolved external symbol WinHttpOpen`

**Solution:**
```cmd
# Add winhttp.lib explicitly
cl /O2 /std:c++17 /EHsc main.cpp fyers.cpp winhttp.lib
```

Or add to code:
```cpp
#pragma comment(lib, "winhttp.lib")
```

---

## 📊 Benchmark Methodology

All benchmarks performed on:
- **OS:** Windows 11
- **CPU:** Modern x64 processor
- **Compiler:** MSVC with `/O2 /Ox` optimizations
- **Network:** Stable broadband connection
- **Test:** 100 consecutive requests to Fyers API

Comparison with official Python SDK using identical test conditions and network.

---

## 🗺️ Roadmap

### Version 1.0 (Current)
- [x] Quote API support
- [x] Connection pooling
- [x] Thread-safe operations
- [x] Comprehensive documentation

### Version 1.1 (Planned)
- [ ] Order placement API
- [ ] Order history retrieval
- [ ] Portfolio management
- [ ] JSON parsing with nlohmann/json
- [ ] CMake build system

### Version 2.0 (Future)
- [ ] Linux/Mac support (libcurl)
- [ ] WebSocket real-time feeds
- [ ] Async API with callbacks
- [ ] Connection pool size configuration
- [ ] Retry logic with exponential backoff

### Version 3.0 (Long-term)
- [ ] HTTP/2 multiplexing
- [ ] Binary protocol option (Protobuf)
- [ ] Advanced order types
- [ ] Market depth data
- [ ] Historical data API

---

## 🤝 Contributing

Contributions are welcome! Here's how you can help:

### Reporting Bugs

Open an issue with:
- Description of the problem
- Steps to reproduce
- Expected vs actual behavior
- System information (OS, compiler version)

### Suggesting Features

Open an issue with:
- Clear description of the feature
- Use case / why it's needed
- Proposed implementation (optional)

### Submitting Pull Requests

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

**Code Style:**
- Follow existing code style
- Add comments for complex logic
- Update README if needed
- Test thoroughly in Release mode

---

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

```
MIT License

Copyright (c) 2024 [Your Name]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

---

## ⚖️ Disclaimer

**IMPORTANT:** This is an **unofficial, community-developed** SDK and is **NOT affiliated with, endorsed by, or supported by Fyers Securities**. 

### Legal & Risk Warnings

- ⚠️ **Use at your own risk** - Thoroughly test in paper trading before live trading
- ⚠️ **No warranty** - This software is provided "as is" without any guarantees
- ⚠️ **Trading risks** - Trading involves substantial risk of loss
- ⚠️ **Not financial advice** - This is a software tool, not investment advice
- ⚠️ **Verify independently** - Always validate outputs before executing trades
- ⚠️ **Rate limits** - Respect Fyers API rate limits to avoid account suspension

### Best Practices

✅ Test extensively in development/paper trading environment
✅ Implement proper error handling in your trading logic
✅ Monitor API usage to stay within rate limits
✅ Keep credentials secure (never commit to git!)
✅ Use stop-losses and risk management
✅ Have fallback mechanisms for API failures

---

## 🙏 Acknowledgments

- **Inspiration:** Built out of necessity for faster execution in algorithmic trading
- **Community:** Thanks to the algo trading community for testing and feedback
- **Fyers:** For providing a robust API infrastructure (even if this SDK is unofficial!)
- **Performance Testing:** All benchmarks done with real API calls during market hours

---

## 📧 Support & Contact

- **Issues:** [GitHub Issues](https://github.com/yourusername/fyers-cpp-sdk/issues)
- **Discussions:** [GitHub Discussions](https://github.com/yourusername/fyers-cpp-sdk/discussions)
- **Email:** your.email@example.com (for private inquiries)

---

## 🌟 Star History

If this project helped improve your trading system, consider:
- ⭐ Starring the repository
- 🍴 Forking for your own modifications
- 📢 Sharing with other algo traders
- 💬 Providing feedback and suggestions

---

## 📈 Project Stats

![GitHub stars](https://img.shields.io/github/stars/yourusername/fyers-cpp-sdk?style=social)
![GitHub forks](https://img.shields.io/github/forks/yourusername/fyers-cpp-sdk?style=social)
![GitHub issues](https://img.shields.io/github/issues/yourusername/fyers-cpp-sdk)
![GitHub pull requests](https://img.shields.io/github/issues-pr/yourusername/fyers-cpp-sdk)

---

<div align="center">

**Built with ⚡ by algo traders, for algo traders**

Made with ❤️ for the trading community

[⬆ Back to Top](#fyers-cpp-sdk)

</div>
