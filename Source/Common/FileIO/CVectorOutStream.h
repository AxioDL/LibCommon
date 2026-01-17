#ifndef AXIO_CVECTOROUTSTREAM_H
#define AXIO_CVECTOROUTSTREAM_H

#include "IOutputStream.h"
#include <vector>

class CVectorOutStream : public IOutputStream
{
    static constexpr uint32_t skAllocSize = 1; // must be 1 or a power of 2

    std::vector<char> *mpVector;
    bool mOwnsVector = true;
    uint32_t mPos = 0;

public:
    CVectorOutStream();
    explicit CVectorOutStream(std::endian DataEndianness);
    explicit CVectorOutStream(uint32_t InitialSize, std::endian DataEndianness);
    explicit CVectorOutStream(std::vector<char> *pVector, std::endian DataEndianness);
    ~CVectorOutStream() override;

    void WriteBytes(const void* pkSrc, uint32_t Count) override;
    bool Seek(int32_t Offset, uint32_t Origin) override;
    uint32_t Tell() const override;
    bool EoF() const override;
    bool IsValid() const override;
    uint32_t Size() const override;
    void SetVector(std::vector<char> *pVector);
    void* Data();
    const void* Data() const;
    void* DataAtPosition();
    const void* DataAtPosition() const;
    void Clear();
};

#endif // AXIO_CVECTOROUTSTREAM_H
