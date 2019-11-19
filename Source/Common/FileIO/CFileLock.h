#ifndef CFILELOCK_H
#define CFILELOCK_H

#include <cstdio>
#include "Common/TString.h"

#ifndef _WIN32
#include <sys/file.h>
#include <unistd.h>
#endif

// Maintain a file handle to prevent other processes from accessing the file.
class CFileLock
{
#ifdef _WIN32
    FILE* mpFile = nullptr;
#else
    int mpFile = -1;
#endif

public:
    CFileLock() = default;

    ~CFileLock()
    {
        Release();
    }

    void Lock(const TString& rkPath)
    {
        Release();
#ifdef _WIN32
        mpFile = _wfopen(ToWChar(rkPath), L"a+");
#else
        mpFile = open(*rkPath, O_CREAT | O_APPEND | O_RDWR, 0644);
        flock(mpFile, LOCK_EX);
#endif
    }

    void Release()
    {
#ifdef _WIN32
        if (mpFile)
            fclose(mpFile);
#else
        if (mpFile != -1)
            close(mpFile);
#endif
    }
};

#endif // CFILELOCK_H
