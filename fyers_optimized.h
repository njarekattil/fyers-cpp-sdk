#pragma once
#include <string>
#include <vector>
#include <windows.h>
#include <winhttp.h>
#include <mutex>
#include <memory>

namespace FyersCredentials {
    extern const std::string APP_ID;
    extern const std::string ACCESS_TOKEN;
}

// RAII wrapper for WinHTTP handles
class WinHttpHandle {
    HINTERNET handle;
public:
    WinHttpHandle(HINTERNET h = nullptr);
    ~WinHttpHandle();
    operator HINTERNET() const;
    HINTERNET* operator&();
    void reset(HINTERNET h = nullptr);
    HINTERNET release();
    bool valid() const { return handle != nullptr; }
    WinHttpHandle(const WinHttpHandle&) = delete;
    WinHttpHandle& operator=(const WinHttpHandle&) = delete;
};

class FyersClient {
private:
    const std::wstring user_agent;
    const std::wstring host;
    std::string app_id;
    std::string access_token;
    std::wstring auth_header;

    // Persistent connection handles
    mutable WinHttpHandle hSession;
    mutable WinHttpHandle hConnect;
    mutable std::mutex connection_mutex;

    // Pre-allocated response buffer
    static constexpr size_t BUFFER_SIZE = 32768;  // Increased buffer size
    mutable char response_buffer[BUFFER_SIZE];

    static std::wstring toWString(const std::string& str);
    void initializeConnection();
    void configureOptimalSettings();

public:
    FyersClient(const std::string& app_id_, const std::string& access_token_);
    ~FyersClient();

    // Fast quote retrieval
    std::string get_quote(const std::vector<std::string>& symbols) const;
};
