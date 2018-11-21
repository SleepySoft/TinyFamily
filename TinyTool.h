#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct string_queue_context
{
    char* buffer;
    uint32_t buffer_len;
    uint32_t rpos;
    uint32_t wpos;
};

uint32_t string_queue_put(struct string_queue_context* ctx, const char* data);
uint32_t string_queue_get(struct string_queue_context* ctx, char* data, uint32_t len);

#define STRING_INDEX_BUFFER_LEN 64
int32_t string_to_index(const char* str);

#ifdef __cplusplus
}
#endif

