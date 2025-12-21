
#include <time/time.h>

namespace gamelib
{

	uint64_t GetTimeMs()
        {
            using namespace std::chrono;
            return duration_cast<milliseconds>(
                steady_clock::now().time_since_epoch()
            ).count();
        }

        std::tm localtime_safe(std::time_t t)
        {
            std::tm tm{};
#if defined(_WIN32)
            localtime_s(&tm, &t);   // Windows
#else
            localtime_r(&t, &tm);   // Linux / macOS
#endif
            return tm;
        }
}
