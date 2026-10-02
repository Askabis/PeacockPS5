#include "network_probe.hpp"

#include "probe_log.hpp"

#include <array>
#include <cstdio>
#include <cstring>

namespace
{
struct NetSockaddrIn
{
    std::uint8_t length;
    std::uint8_t family;
    std::uint16_t port;
    std::uint32_t address;
    std::uint16_t virtual_port;
    std::uint8_t zero[6];
};

extern "C"
{
    int sceNetConnect(int socket, const void *address, std::uint32_t address_length);
    int sceNetSend(int socket, const void *data, std::size_t length, int flags);
    int sceNetRecv(int socket, void *data, std::size_t length, int flags);
    int sceNetSetsockopt(int socket, int level, int option, const void *value, std::uint32_t size);
    int sceNetSocket(const char *name, int domain, int type, int protocol);
    int sceNetSocketClose(int socket);
}

std::uint16_t to_network16(std::uint16_t value) noexcept
{
    return static_cast<std::uint16_t>((value << 8U) | (value >> 8U));
}

std::uint32_t to_ps5_ipv4(std::uint32_t network_order_address) noexcept
{
    return ((network_order_address & 0x000000ffU) << 24U) | ((network_order_address & 0x0000ff00U) << 8U) |
           ((network_order_address & 0x00ff0000U) >> 8U) | ((network_order_address & 0xff000000U) >> 24U);
}

bool send_all(int socket, const char *data, std::size_t size) noexcept
{
    std::size_t sent = 0;
    while (sent < size)
    {
        const int count = sceNetSend(socket, data + sent, size - sent, 0);
        if (count <= 0)
            return false;
        sent += static_cast<std::size_t>(count);
    }
    return true;
}

void set_summary(peacock::ProbeResult &result, const char *text, int code = 0) noexcept
{
    result.transport_code = code;
    std::snprintf(result.summary, sizeof(result.summary), "%s", text);
}
} // namespace

namespace peacock
{
ProbeResult run_probe(const Config &config) noexcept
{
    ProbeResult result{};
    std::uint32_t address = 0;
    if (!parse_ipv4(config.host, address))
    {
        set_summary(result, "PEACOCK_HOST must be an IPv4 address");
        append_log(result.summary);
        return result;
    }

    const int socket = sceNetSocket("peacock_probe", 2, 1, 6);
    if (socket < 0)
    {
        set_summary(result, "socket creation failed", socket);
        append_log(result.summary);
        return result;
    }

    constexpr int socket_level = 0xffff;
    const int timeout_us = config.timeout_ms * 1000;
    for (const int option : {0x1105, 0x1106, 0x1109})
        (void)sceNetSetsockopt(socket, socket_level, option, &timeout_us, sizeof(timeout_us));

    const NetSockaddrIn target{sizeof(NetSockaddrIn), 2, to_network16(config.port), to_ps5_ipv4(address), 0, {0}};
    if (const int connect_result = sceNetConnect(socket, &target, sizeof(target)); connect_result < 0)
    {
        set_summary(result, "connect failed", connect_result);
        (void)sceNetSocketClose(socket);
        append_log(result.summary);
        return result;
    }

    std::array<char, 512> request{};
    const int request_size = std::snprintf(request.data(), request.size(),
                                          "GET %s HTTP/1.1\r\nHost: %s\r\nUser-Agent: PeacockPS5/0.1\r\n"
                                          "Accept: application/json,*/*\r\nConnection: close\r\n\r\n",
                                          config.path, config.host);
    if (request_size <= 0 || static_cast<std::size_t>(request_size) >= request.size() ||
        !send_all(socket, request.data(), static_cast<std::size_t>(request_size)))
    {
        set_summary(result, "request send failed");
        (void)sceNetSocketClose(socket);
        append_log(result.summary);
        return result;
    }

    std::array<char, 512> response{};
    const int received = sceNetRecv(socket, response.data(), response.size() - 1, 0);
    (void)sceNetSocketClose(socket);
    if (received <= 0)
    {
        set_summary(result, "no HTTP response", received);
        append_log(result.summary);
        return result;
    }

    response[static_cast<std::size_t>(received)] = '\0';
    char *line_end = std::strstr(response.data(), "\r\n");
    if (line_end != nullptr)
        *line_end = '\0';
    std::snprintf(result.first_line, sizeof(result.first_line), "%s", response.data());
    if (std::sscanf(result.first_line, "HTTP/%*s %d", &result.status_code) == 1)
    {
        result.ok = result.status_code >= 200 && result.status_code < 500;
        std::snprintf(result.summary, sizeof(result.summary), "HTTP probe received status %d", result.status_code);
    }
    else
    {
        set_summary(result, "response was not HTTP");
    }

    char log_line[256]{};
    std::snprintf(log_line, sizeof(log_line), "%s | %s:%u%s | %s", result.ok ? "OK" : "FAIL", config.host,
                  static_cast<unsigned>(config.port), config.path, result.first_line);
    append_log(log_line);
    return result;
}
} // namespace peacock
