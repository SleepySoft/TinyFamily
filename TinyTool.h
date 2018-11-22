/************************************************************/
/*           TinyTool - For MCU or SCM C compiler           */
/*                                    Sleepy                */
/************************************************************/

#ifndef _TINY_TOOL_SLEEPY_H_
#define _TINY_TOOL_SLEEPY_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


/*----------------------------------------------------------*/
/*      String Queue - A string queue with ring buffer      */
/*----------------------------------------------------------*/

struct string_queue_context
{
    char* buffer;
    uint32_t buffer_len;
    uint32_t rpos;
    uint32_t wpos;
};

uint32_t string_queue_put(struct string_queue_context* ctx, const char* data);
uint32_t string_queue_get(struct string_queue_context* ctx, char* data, uint32_t len);


/*---------------------------------------------------------*/
/*  String to Index - Cache and Mapping a String to Index  */
/*   - Return the index of cached string                   */
/*   - If string not cached before, it will cache          */
/*     this string and return its index                    */
/*   - If out of buffer memory, it will return -1          */
/*---------------------------------------------------------*/

#define STRING_INDEX_BUFFER_LEN 64
int32_t string_to_index(const char* str);

#ifdef __cplusplus
}
#endif

#endif // _TINY_TOOL_SLEEPY_H_

