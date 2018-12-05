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

struct ring_buffer_ctx
{
    uint8_t* m_data;
    uint32_t m_rPos;
    uint32_t m_wPos;
    uint32_t m_length;
    uint32_t threshold;
};

uint32_t ring_buffer_len(struct ring_buffer_ctx* ctx);
void ring_buffer_put(struct ring_buffer_ctx* ctx, uint8_t* buffer, uint32_t len);
uint32_t ring_buffer_get(struct ring_buffer_ctx* ctx, uint8_t* buffer, uint32_t len);
void ring_buffer_init(struct ring_buffer_ctx* ctx, uint8_t* buffer, uint32_t len);

#ifdef __cplusplus
}
#endif

