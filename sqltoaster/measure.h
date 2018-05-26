/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */


#ifndef SQLTOASTER_MEASURE_H
#define SQLTOASTER_MEASURE_H

#include <chrono>

template<typename TimeT = std::chrono::milliseconds>
struct measure
{
    template<typename F, typename ...Args>
    static typename TimeT::rep execution(F&& func, Args&&... args)
    {
        auto start = std::chrono::steady_clock::now();
        std::forward<decltype(func)>(func)(std::forward<Args>(args)...);
        auto duration = std::chrono::duration_cast<TimeT>
                            (std::chrono::steady_clock::now() - start);
        return duration.count();
    }
};

#endif /* SQLTOASTER_MEASURE_H */
