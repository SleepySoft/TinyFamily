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

int32_t string_to_index(const char* str)
{
    int32_t index = 0;
    uint32_t offset = 0;
    size_t length = 0;
    do 
    {
        if (move_compare(string_index_buffer, &offset, STRING_INDEX_BUFFER_LEN, str))
        {
            break;
        }
        if (offset >= STRING_INDEX_BUFFER_LEN - 1)
        {
            index = -1;
            break;
        }
        ++index;
        if (string_index_buffer[offset] == '\0')
        {
            length = strlen(str);
            if (offset + length + 2 >= STRING_INDEX_BUFFER_LEN)
            {
                index = -1;
                break;
            }
            strcpy(string_index_buffer + offset, str);
            string_index_buffer[offset + length + 1] = '\0';
            break;
        }
    } while (offset < STRING_INDEX_BUFFER_LEN);
    return index;
}

#ifdef __cplusplus
}
#endif





