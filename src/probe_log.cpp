#include "probe_log.hpp"

#include <cstdio>

namespace peacock
{
void append_log(const char *message) noexcept
{
    FILE *file = std::fopen("/download0/peacock_probe.log", "ab");
    if (file == nullptr)
        return;
    std::fputs(message, file);
    std::fputc('\n', file);
    std::fclose(file);
}
} // namespace peacock
