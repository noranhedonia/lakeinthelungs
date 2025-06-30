#pragma once

/** @file lake/bedrock/time.h
 *  @brief TODO docs
 */

#include <lake/bedrock/types.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define LAKE_MS_PER_SECOND      1000
#define LAKE_US_PER_SECOND      1000000
#define LAKE_NS_PER_SECOND      1000000000LL
#define LAKE_NS_PER_MS          1000000
#define LAKE_NS_PER_US          1000
#define LAKE_SECONDS_TO_NS(S)   (((u64)(S)) * LAKE_NS_PER_SECOND)
#define LAKE_NS_TO_SECONDS(NS)  ((NS) / LAKE_NS_PER_SECOND)
#define LAKE_MS_TO_NS(MS)       (((u64)(MS)) * LAKE_NS_PER_MS)
#define LAKE_NS_TO_MS(NS)       ((NS) / LAKE_NS_PER_MS)
#define LAKE_US_TO_NS(US)       (((u64)(US)) * LAKE_NS_PER_US)
#define LAKE_NS_TO_US(NS)       ((NS) / LAKE_NS_PER_US)

#define LAKE_TIME_OVERFLOW 86400000
#define LAKE_TIME_LESS(a, b)            ((a) - (b) >= LAKE_TIME_OVERFLOW)
#define LAKE_TIME_GREATER(a, b)         ((b) - (a) >= LAKE_TIME_OVERFLOW)
#define LAKE_TIME_LESS_EQUAL(a, b)      (!LAKE_TIME_GREATER(a, b))
#define LAKE_TIME_GREATER_EQUAL(a, b)   (!LAKE_TIME_LESS(a, b))
#define LAKE_TIME_DIFFERENCE(a, b)      ((a) - (b) >= LAKE_TIME_OVERFLOW ? (b) - (a) : (a) - (b))

/** Returns the counter of the real-time clock. */
LAKE_HOT_FN 
LAKEAPI u64 LAKECALL 
lake_rtc_counter(void);

/** Returns the frequency of the real-time clock. */
LAKE_HOT_FN 
LAKEAPI u64 LAKECALL 
lake_rtc_frequency(void);

/** Invoke this function exactly once per frame to record the current frame time.
 *  Only when the other functions defined in this header will be available. */
LAKEAPI void LAKECALL 
lake_frame_time_record(
    u64 time_app_start, 
    u64 time_now, 
    f64 dt_freq_reciprocal);

/** Retrieves the current estimate of the frame time in seconds. It is the median 
 *  of a certain number of previously recorded frame times. */
LAKEAPI f32 LAKECALL 
lake_frame_time_median(void);

/** Prints the current estimate of the total frame time periodically, namely once per 
 *  given time interval, assuming this function is called once per frame. The interval 
 *  must be given in relation to the dt_freq_reciprocal used while recording. */
LAKEAPI void LAKECALL 
lake_frame_time_print(
    f32 interval_from_dt_freq);

#ifdef __cplusplus
}
#endif /* __cplusplus */
