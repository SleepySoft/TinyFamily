#include "TinyTool.h"
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

uint32_t string_queue_put(struct string_queue_context* ctx, const char* data)
{
    uint32_t iter = 0;
    uint32_t write = 0;
    while (1)
    {
        iter = ctx->wpos % ctx->buffer_len;
        ctx->buffer[iter] = ((unsigned char*)data)[write];
        ctx->wpos++;
        write++;
        if (ctx->buffer[iter] == '\0')
        {
            break;
        }
    }
    if (ctx->wpos - ctx->rpos > ctx->buffer_len)
    {
        ctx->rpos = ctx->wpos - ctx->buffer_len;
    }
    if (ctx->rpos > ctx->buffer_len * 10)
    {
        ctx->rpos -= ctx->buffer_len * 10;
        ctx->wpos -= ctx->buffer_len * 10;
    }
    return write;
}
uint32_t string_queue_get(struct string_queue_context* ctx, char* data, uint32_t len)
{
    uint32_t iter = 0;
    uint32_t read = 0;
    while (1)
    {
        if (ctx->rpos >= ctx->wpos)
        {
            break;
        }

        iter = ctx->rpos % ctx->buffer_len;
        ((unsigned char*)data)[read] = ctx->buffer[iter];
        ctx->rpos++;
        read++;

        if (ctx->buffer[iter] == '\0')
        {
            break;
        }
    }
    return read;
}


int32_t move_compare(const char* str, uint32_t* offset, uint32_t length, const char* compare)
{
    int32_t result = 1;
    do {
        if (((*offset) >= length) || ((*str) == 0))
        {
            result = 0;
            break;
        }
        if (result)
        {
            if (str[*offset] != (*compare))
            {
                result = 0;
            }
            ++compare;
        }
    } while (str[(*offset)++] != '\0');
    return result;
}

static char string_index_buffer[STRING_INDEX_BUFFER_LEN] = { 0 };

static const uint32_t STAT_COMPARE = 0;
static const uint32_t STAT_GONEXT = 1;
static const uint32_t STAT_COPY = 2;
static const uint32_t STAT_DONE = 3;

int32_t string_to_index(const char* str)
{
    int32_t index = ((str && str[0]) ? 0 : -1);
    uint32_t state = ((string_index_buffer[0] == '\0') ? STAT_COPY : STAT_COMPARE);
    uint32_t offset = 0, compare = 0;
    while (str && str[0])
    {
        if (offset >= STRING_INDEX_BUFFER_LEN - 1)
        {
            index = -1;
            break;
        }
        if (state == STAT_COMPARE)
        {
            if (string_index_buffer[offset] != str[compare])
            {
                compare = 0;
                state = STAT_GONEXT;
            }
            else if (str[compare] == '\0')
            {
                break;
            }
            else
            {
                ++compare;
            }
        }
        if (state == STAT_GONEXT)
        {
            if (string_index_buffer[offset] == '\0')
            {
                if (string_index_buffer[offset + 1] == '\0')
                {
                    if (offset + 1 + strlen(str) >= STRING_INDEX_BUFFER_LEN - 1)
                    {
                        index = -1;
                        break;
                    }
                    state = STAT_COPY;
                }
                else
                {
                    state = STAT_COMPARE;
                }
                ++index;
            }
        }
        else if (state == STAT_COPY)
        {
            string_index_buffer[offset] = str[compare];
            if (str[compare] == '\0')
            {
                string_index_buffer[offset + 1] = 0;
                break;
            }
            ++compare;
        }
        ++offset;
    }
    return index;
}

#ifdef __cplusplus
}
#endif





