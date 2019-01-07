#include <stdint.h>


/*****************************************************************************/
/*                                                                           */
/*                            class TinyRingBuffer                           */
/*                           Original Implemention.                          */
/*                                                                           */
/*****************************************************************************

template< class T, uint32_t SIZE >
class TinyRingBuffer
{
protected:
    T m_data[SIZE];
    uint32_t m_rPos;
    uint32_t m_wPos;
    static const uint32_t threshold = 10 * SIZE;

public:
    TinyRingBuffer() : m_rPos(0), m_wPos(0) {
        for (uint32_t i = 0; i < SIZE; i++) m_data[i] = T();
    }
    ~TinyRingBuffer() { };

    uint32_t len() { return m_wPos - m_rPos; };
    void put(const T& val) {
        access(m_wPos++) = val;
        if (m_wPos - m_rPos > SIZE) { m_rPos = m_wPos - SIZE; }
        if (m_rPos > threshold) { m_wPos -= threshold; m_rPos -= threshold; }
    }
    T get() { return readable(m_rPos) ? access(m_rPos) : T(); }
    T peek(uint32_t offset) { uint32_t pos(m_rPos + offset); return readable(pos) ? access(pos) : T(); };
protected:
    T& access(uint32_t pos) { return m_data[pos % SIZE]; };
    bool readable(uint32_t pos) { return (pos < m_wPos); };
};
******************************************************************************/


/*****************************************************************************/
/*                                                                           */
/*                           class RingBufferShell                           */
/*         You can map your memory as a ring buffer with this class.         */
/*                                                                           */
/*****************************************************************************/

template< class T >
class TinyRingBufferShell
{
protected:
    T* m_buffer;
    uint32_t m_length;
    uint64_t* m_readPos;
    uint64_t* m_writePos;
    uint32_t m_threshold;

public:
    TinyRingBufferShell() : m_buffer(NULL), m_length(0), m_readPos(NULL), m_writePos(NULL) { }
    virtual ~TinyRingBufferShell() { };

    void init(T* buffer, uint32_t length, uint64_t* readPos, uint64_t* writePos, uint32_t threshold = 0) {
        m_buffer = buffer; m_length = length; m_readPos = readPos; m_writePos = writePos; m_threshold = threshold;
    }
    bool inited() const { return (m_buffer != NULL) && (m_length > 0) && (m_readPos != NULL) && (m_writePos != NULL); };

    uint32_t length() { adjust();  return (uint32_t)((*m_writePos) - (*m_readPos)); };
    uint32_t capacity() const { return m_length; };

    bool end() const { return (*m_readPos) >= (*m_writePos); }
    void put(const T& val) { m_buffer[((*m_writePos)++) % m_length] = val; }
    void poke(int32_t offset, const T& val) { access((*m_writePos) + offset) = val; }
    T get() { return adjust() && readable(*m_readPos) ? access((*m_readPos)++) : T(); }
    T peek(int32_t offset) { adjust();  uint64_t pos((*m_readPos) + offset); return readable(pos) ? access(pos) : T(); };

protected:
    T& access(uint64_t pos) { return m_buffer[pos % m_length]; };
    bool readable(uint64_t pos) { return (pos < (*m_writePos)); };
    bool adjust() {
        if (((*m_writePos) - (*m_readPos) > m_length)) { (*m_readPos) = (*m_writePos) - m_length; }
        if ((m_threshold > 0) && ((*m_readPos) > m_threshold)) { (*m_writePos) -= m_threshold; (*m_readPos) -= m_threshold; };
        return true;
    }
};


/*****************************************************************************/
/*                                                                           */
/*                            class TinyRingBuffer                           */
/*                                                                           */
/*****************************************************************************/

template< class T, uint32_t SIZE >
class TinyRingBuffer : public TinyRingBufferShell< T >
{
protected:
    T m_data[SIZE];
    uint64_t m_rPos;
    uint64_t m_wPos;

public:
    TinyRingBuffer() : m_rPos(0), m_wPos(0) {
        for (uint32_t i = 0; i < SIZE; i++) { m_data[i] = T(); }
        init(m_data, SIZE, &m_rPos, &m_wPos);
    }
    virtual ~TinyRingBuffer() { };
};

class TinyCircularBuffer : public TinyRingBufferShell< uint8_t >
{
protected:
    uint8_t* m_data;
    uint32_t m_size;
    uint64_t m_rPos;
    uint64_t m_wPos;

public:
    TinyCircularBuffer(uint32_t size) : m_rPos(0), m_wPos(0) {
        m_size = size;
        m_data = new uint8_t[m_size];
        for (uint32_t i = 0; i < m_size; i++) m_data[i] = 0;
        init(m_data, m_size, &m_rPos, &m_wPos);
    }
    virtual ~TinyCircularBuffer() { delete[] m_data; m_data = NULL; };

    uint32_t write(const uint8_t* buffer, uint32_t len) {
        for (uint32_t i = 0; i < len; ++i) { put(buffer[i]); }
        return len;
    }
    uint32_t read(uint8_t* buffer, uint32_t size) {
        uint32_t readed = 0;
        for ( ; (readed < size) && !end(); ++readed) { buffer[readed] = get(); }
        return   readed;
    }
};


template< class T, uint32_t SIZE >
class TinySmooth
{
protected:
    TinyRingBuffer< T, SIZE > m_ringBuffer;
public:
    TinySmooth() { };
    ~TinySmooth() { };

    void appendData(const T& val) {
        m_ringBuffer.put(val);
    };
    T smoothedData() {
        T val = T();
        uint32_t len = m_ringBuffer.length();
        for (uint32_t i = 0; i < len; i++) {
            val += m_ringBuffer.peek(i);
        }
        if (len > 1) {
            val /= len;
        }
        return val;
    }
};


struct TinyMask
{
    uint32_t m_mask;

    TinyMask() { m_mask = 0; }
    TinyMask(uint32_t rhs) { m_mask = rhs; }
    TinyMask(const TinyMask& rhs) { m_mask = rhs.maskGet(); }

    TinyMask& operator=(uint32_t rhs) { m_mask = rhs; return *this; }
    TinyMask& operator=(const TinyMask& rhs) { m_mask = rhs.maskGet(); return *this; }

    bool operator==(uint32_t rhs) { return m_mask == rhs; }
    bool operator==(const TinyMask& rhs) { return m_mask == rhs.maskGet(); }

    uint32_t maskGet() const { return m_mask; }
    operator uint32_t() const { return m_mask; }

    void maskReset() { m_mask = 0; }
    void maskAdd(uint32_t mask) { m_mask |= mask; }
    void maskRemove(uint32_t mask) { m_mask &= ~mask; }
    bool maskCheck(uint32_t mask) { return (m_mask & mask) != 0; }
};

