#include <stdint.h>


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
        uint32_t len = m_ringBuffer.len();
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

