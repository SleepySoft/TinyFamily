#include "TinyFamily.h"
#include "TinyTool.h"
#include <limits>
#include <time.h>
#include <stdlib.h>
#include <assert.h>
#include <assert.h>

void Test_StringToIndex()
{
    int32_t index = 0;

    index = string_to_index("index_xxx_01");
    assert(index == 0);
    index = string_to_index("index_xxxx_02");
    assert(index == 1);
    index = string_to_index("index_xxxxxxx_03");
    assert(index == 2);
    index = string_to_index("index_xxxxx_04");
    assert(index == 3);
    index = string_to_index("index_xxxxxxx_05");
    assert(index == -1);
    index = string_to_index("index_xxxxxxxxxx_06");
    assert(index == -1);

    index = string_to_index("index_xxx_01");
    assert(index == 0);
    index = string_to_index("index_xxxx_02");
    assert(index == 1);
    index = string_to_index("index_xxxxxxx_03");
    assert(index == 2);
    index = string_to_index("index_xxxxx_04");
    assert(index == 3);
    index = string_to_index("index_xxxxxxx_05");
    assert(index == -1);
    index = string_to_index("in");
    assert(index == 4);
    index = string_to_index("in");
    assert(index == 4);
}

void Test_TinySmooth()
{
    {
        TinySmooth< int, 5 > ts_int_5;
        ts_int_5.appendData(2);
        ts_int_5.appendData(2);
        ts_int_5.appendData(2);
        ts_int_5.appendData(2);
        ts_int_5.appendData(2);
        ts_int_5.appendData(2);
        ts_int_5.appendData(2);
        ts_int_5.appendData(2);
        int smoothed_int = ts_int_5.smoothedData();
        assert(smoothed_int == 2);
    }

    {
        int smoothed_int = 0;
        TinySmooth< int, 5 > ts_int_5;

        ts_int_5.appendData(1);
        smoothed_int = ts_int_5.smoothedData();
        assert(smoothed_int == 1 / 1);

        ts_int_5.appendData(2);
        smoothed_int = ts_int_5.smoothedData();
        assert(smoothed_int == (1 + 2) / 2);

        ts_int_5.appendData(3);
        smoothed_int = ts_int_5.smoothedData();
        assert(smoothed_int == (1 + 2 + 3) / 3);

        ts_int_5.appendData(4);
        smoothed_int = ts_int_5.smoothedData();
        assert(smoothed_int == (1 + 2 + 3 + 4) / 4);

        ts_int_5.appendData(5);
        smoothed_int = ts_int_5.smoothedData();
        assert(smoothed_int == (1 + 2 + 3 + 4 + 5) / 5);

        ts_int_5.appendData(6);
        smoothed_int = ts_int_5.smoothedData();
        assert(smoothed_int == (2 + 3 + 4 + 5 + 6) / 5);

        ts_int_5.appendData(7);
        smoothed_int = ts_int_5.smoothedData();
        assert(smoothed_int == (3 + 4 + 5 + 6 + 7) / 5);
    }

    {
        float smoothed_float = 0;
        TinySmooth< float, 4 > ts_float_4;

        ts_float_4.appendData(11.0f);
        smoothed_float = ts_float_4.smoothedData();
        assert(smoothed_float == 11.0f / 1);

        ts_float_4.appendData(22.0f);
        smoothed_float = ts_float_4.smoothedData();
        assert(smoothed_float == (11.0f + 22.0f) / 2);

        ts_float_4.appendData(33.0f);
        smoothed_float = ts_float_4.smoothedData();
        assert(smoothed_float == (11.0f + 22.0f + 33.0f) / 3);

        ts_float_4.appendData(44.0f);
        smoothed_float = ts_float_4.smoothedData();
        assert(smoothed_float == (11.0f + 22.0f + 33.0f + 44.0f) / 4);

        ts_float_4.appendData(55.0f);
        smoothed_float = ts_float_4.smoothedData();
        assert(smoothed_float == (55.0f + 22.0f + 33.0f + 44.0f) / 4);

        ts_float_4.appendData(66.0f);
        smoothed_float = ts_float_4.smoothedData();
        assert(smoothed_float == (55.0f + 66.0f + 33.0f + 44.0f) / 4);

        ts_float_4.appendData(77.0f);
        smoothed_float = ts_float_4.smoothedData();
        assert(smoothed_float == (55.0f + 66.0f + 77.0f + 44.0f) / 4);

        ts_float_4.appendData(88.0f);
        smoothed_float = ts_float_4.smoothedData();
        assert(smoothed_float == (55.0f + 66.0f + 77.0f + 88.0f) / 4);

        ts_float_4.appendData(99.0f);
        smoothed_float = ts_float_4.smoothedData();
        assert(smoothed_float == (99.0f + 66.0f + 77.0f + 88.0f) / 4);
    }

    {
        static const double lower = -1000000.0;
        static const double upper = 1000000.0;
        static const uint32_t loops = 10000000;
        static const uint32_t points = 11;

        srand((unsigned)time(0));

        double smoothed_double = 0;
        TinySmooth< double, points > ts_double_x;

        double* arr = new double[loops];
        for (uint32_t i = 0; i < loops; i++)
        {
            arr[i] = lower + (upper - lower) * rand() / (RAND_MAX + 1);
        }

        for (uint32_t i = 0; i < loops; i++)
        {
            ts_double_x.appendData(arr[i]);
            smoothed_double = ts_double_x.smoothedData();

            double sum = 0.0;
            uint32_t count = 0;

            for (uint32_t j = i; ; )
            {
                sum += arr[j];
                count++;
                if (j == 0 || count >= points)
                {
                    break;
                }
                j--;
            }

            if (count != 0)
            {
                assert(smoothed_double == (sum / count));
                //assert(abs(smoothed_double - (sum / count)) < std::numeric_limits<double>::epsilon());
            }
        }

        delete[] arr;
        arr = NULL;
    }
}

void Test_Ringbuffer_C()
{
    uint32_t testDataLen = 10000000;
    uint32_t testBufferLen = 99;

    uint8_t* mainBuffer = new uint8_t[testBufferLen];
    uint8_t* randomBuffer = new uint8_t[testDataLen];
    uint8_t* compareBuffer = new uint8_t[testDataLen];

    ring_buffer_ctx rb_ctx;
    ring_buffer_init(&rb_ctx, mainBuffer, testBufferLen);

    srand((unsigned)time(NULL));

    for (uint32_t i = 0; i < testDataLen; ++i)
    {
        randomBuffer[i] = (uint8_t)(UINT8_MAX * rand() / RAND_MAX);
    }

    uint32_t posW = 0;
    uint32_t posR = 0;
    uint32_t posC = 0;
    uint32_t dataLen = 0;
    uint32_t freeLen = 0;
    uint32_t randomWLen = 0;
    uint32_t randomRLen = 0;

    while ((posW < testDataLen) || (posR < testDataLen))
    {
        freeLen = testBufferLen - ring_buffer_len(&rb_ctx);
        if ((freeLen > 0) && (posW < testDataLen))
        {
            randomWLen = (uint32_t)((rand() % (freeLen - 1 + 1)) + 1);
            if (randomWLen + posW >= testDataLen)
            {
                randomWLen = testDataLen - posW;
            }
            ring_buffer_put(&rb_ctx, randomBuffer + posW, randomWLen);
            posW += randomWLen;
        }

        dataLen = ring_buffer_len(&rb_ctx);
        if ((dataLen > 0) && (posR < testDataLen))
        {
            randomRLen = (uint32_t)((rand() % (dataLen - 1 + 1)) + 1);
            if (randomRLen + posR >= testDataLen)
            {
                randomRLen = testDataLen - posR;
            }
            ring_buffer_get(&rb_ctx, compareBuffer + posR, randomRLen);
            posR += randomRLen;
        }
        for ( ; posC < posR; ++posC)
        {
            assert(randomBuffer[posC] == compareBuffer[posC]);
        }
    }

    delete[] mainBuffer; mainBuffer = NULL;
    delete[] randomBuffer; randomBuffer = NULL;
    delete[] compareBuffer; compareBuffer = NULL;
}

int main()
{
    Test_TinySmooth();
    Test_StringToIndex();
    Test_Ringbuffer_C();
    return 0;
}