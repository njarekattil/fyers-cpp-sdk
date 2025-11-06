#include "fyers_optimized.h"
#include <sstream>
#pragma comment(lib, "winhttp.lib")

// Define decompression flags if not available in older SDKs
#ifndef WINHTTP_DECOMPRESSION_FLAG_GZIP
#define WINHTTP_DECOMPRESSION_FLAG_GZIP    0x00000001
#define WINHTTP_DECOMPRESSION_FLAG_DEFLATE 0x00000002
#define WINHTTP_DECOMPRESSION_FLAG_ALL     (WINHTTP_DECOMPRESSION_FLAG_GZIP | WINHTTP_DECOMPRESSION_FLAG_DEFLATE)
#endif

// API Credentials
namespace FyersCredentials {
    const std::string APP_ID = "ZYXABCDEF-100";
    const std::string ACCESS_TOKEN = "eyJhbGc******B7ayZGPyuIqRb***KJqaNg";
}

// WinHttpHandle implementation
WinHttpHandle::WinHttpHandle(HINTERNET h) : handle(h) {}

WinHttpHandle::~WinHttpHandle() {
    if (handle) {
        WinHttpCloseHandle(handle);
        handle = nullptr;
    }
}

WinHttpHandle::operator HINTERNET() const { return handle; }
HINTERNET* WinHttpHandle::operator&() { return &handle; }

void WinHttpHandle::reset(HINTERNET h) {
    if (handle) WinHttpCloseHandle(handle);
    handle = h;
}

HINTERNET WinHttpHandle::release() {
    HINTERNET temp = handle;
    handle = nullptr;
    return temp;
}

// FyersClient implementation
FyersClient::FyersClient(const std::string& app_id_, const std::string& access_token_)
    : user_agent(L"Fyers/3.0"),
    host(L"api-t1.fyers.in"),
    app_id(app_id_),
    access_token(access_token_) {

    // Pre-build auth header (done once)
    auth_header = toWString("Authorization: " + app_id + ":" + access_token);

    // Initialize persistent connection
    initializeConnection();
    configureOptimalSettings();
}

FyersClient::~FyersClient() {
    // Handles cleaned up automatically by RAII
}

void FyersClient::initializeConnection() {
    // Open session once with async flag for better performance
    hSession.reset(WinHttpOpen(user_agent.c_str(),
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS,
        0));

    if (hSession) {
        // Connect once (reused for all requests)
        hConnect.reset(WinHttpConnect(hSession,
            host.c_str(),
            INTERNET_DEFAULT_HTTPS_PORT,
            0));
    }
}

void FyersClient::configureOptimalSettings() {
    if (!hSession) return;

    // Aggressive timeout settings for low latency
    DWORD timeout_resolve = 2000;    // 2 seconds for DNS
    DWORD timeout_connect = 3000;    // 3 seconds for connection
    DWORD timeout_send = 5000;       // 5 seconds for send
    DWORD timeout_receive = 5000;    // 5 seconds for receive

    WinHttpSetOption(hSession, WINHTTP_OPTION_RESOLVE_TIMEOUT, &timeout_resolve, sizeof(timeout_resolve));
    WinHttpSetOption(hSession, WINHTTP_OPTION_CONNECT_TIMEOUT, &timeout_connect, sizeof(timeout_connect));
    WinHttpSetOption(hSession, WINHTTP_OPTION_SEND_TIMEOUT, &timeout_send, sizeof(timeout_send));
    WinHttpSetOption(hSession, WINHTTP_OPTION_RECEIVE_TIMEOUT, &timeout_receive, sizeof(timeout_receive));

    // Enable HTTP/2 for better performance (Windows 10+)
    // Comment out if using Windows 7/8
#if defined(WINHTTP_PROTOCOL_FLAG_HTTP2)
    DWORD http2 = WINHTTP_PROTOCOL_FLAG_HTTP2;
    WinHttpSetOption(hSession, WINHTTP_OPTION_ENABLE_HTTP_PROTOCOL, &http2, sizeof(http2));
#endif

    // Optional: Disable certificate revocation check for lower latency
    // This trades security for speed - use carefully in production!
    // The revocation check can add 50-100ms per request

    // Method 1: Skip revocation check only (safest performance boost)
    // Uncomment the next 2 lines to enable this optimization:
    // DWORD securityFlags = 0x00000080;  // SECURITY_FLAG_IGNORE_REVOCATION
    // WinHttpSetOption(hSession, WINHTTP_OPTION_SECURITY_FLAGS, &securityFlags, sizeof(securityFlags));
}

std::wstring FyersClient::toWString(const std::string& str) {
    if (str.empty()) return L"";
    int size = MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), nullptr, 0);
    std::wstring result(size, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), &result[0], size);
    return result;
}

std::string FyersClient::get_quote(const std::vector<std::string>& symbols) const {
    // Build path
    std::ostringstream path;
    path << "/data/quotes?symbols=";
    for (size_t i = 0; i < symbols.size(); ++i) {
        if (i > 0) path << ",";
        path << symbols[i];
    }

    std::wstring wpath = toWString(path.str());

    // Lock for thread safety
    std::lock_guard<std::mutex> lock(connection_mutex);

    if (!hConnect.valid()) return "";

    // Create request with optimal flags
    WinHttpHandle hRequest(WinHttpOpenRequest(hConnect,
        L"GET",
        wpath.c_str(),
        nullptr,
        WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        WINHTTP_FLAG_SECURE));

    if (!hRequest.valid()) return "";

    // Try to enable automatic decompression (Windows 8.1+)
    // If this fails, we'll just get uncompressed responses
#if defined(WINHTTP_OPTION_DECOMPRESSION)
    DWORD dwAutoDecompression = WINHTTP_DECOMPRESSION_FLAG_ALL;
    WinHttpSetOption(hRequest, WINHTTP_OPTION_DECOMPRESSION, &dwAutoDecompression, sizeof(dwAutoDecompression));
#endif

    // Set request-specific options for speed
    DWORD disableFeature = WINHTTP_DISABLE_REDIRECTS;
    WinHttpSetOption(hRequest, WINHTTP_OPTION_DISABLE_FEATURE, &disableFeature, sizeof(disableFeature));

    // Add minimal headers - don't request compression if decompression isn't supported
    std::wstring headers = auth_header + L"\r\nAccept: application/json";

    // Only request compression if we have decompression support
#if defined(WINHTTP_OPTION_DECOMPRESSION)
    headers += L"\r\nAccept-Encoding: gzip, deflate";
#endif

    if (!WinHttpAddRequestHeaders(hRequest, headers.c_str(), -1L, WINHTTP_ADDREQ_FLAG_ADD))
        return "";

    // Send request
    if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
        WINHTTP_NO_REQUEST_DATA, 0, 0, 0))
        return "";

    if (!WinHttpReceiveResponse(hRequest, nullptr))
        return "";

    // Read response efficiently
    std::string responseData;
    responseData.reserve(BUFFER_SIZE);

    DWORD bytesRead = 0;
    while (WinHttpReadData(hRequest, response_buffer, BUFFER_SIZE, &bytesRead)) {
        if (bytesRead == 0) break;
        responseData.append(response_buffer, bytesRead);
    }

    return responseData;
}