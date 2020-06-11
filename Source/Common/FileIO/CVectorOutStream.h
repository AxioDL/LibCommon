#ifndef CVECTOROUTSTREAM_H
#define CVECTOROUTSTREAM_H

#include "IOutputStream.h"
#include <vector>

class CVectorOutStream : public IOutputStream
{
    static constexpr uint32 skAllocSize = 1; // must be 1 or a power of 2

    std::vector<char> *mpVector;
    bool mOwnsVector = true;
    uint32 mPos = 0;

public:
    CVectorOutStream();
    CVectorOutStream(EEndian DataEndianness);
    CVectorOutStream(uint32 InitialSize, EEndian DataEndianness);
    CVectorOutStream(std::vector<char> *pVector, EEndian DataEndianness);
    ~CVectorOutStream() override;

    void WriteBytes(const void* pkSrc, uint32 Count) override;
    bool Seek(int32 Offset, uint32 Origin) override;
    uint32 Tell() const override;
    bool EoF() const override;
    bool IsValid() const override;
    uint32 Size() const override;
    void SetVector(std::vector<char> *pVector);
    void *Data();
    void *DataAtPosition();
    void Clear();
};

#endif // CVECTOROUTSTREAM_H
