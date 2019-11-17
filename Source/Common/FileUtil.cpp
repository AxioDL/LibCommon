#include "FileUtil.h"
#include "Macros.h"
#include "Common/FileIO/CFileInStream.h"

#include <chrono>
#include <filesystem>
#include <system_error>

#ifdef WIN32
#include <Windows.h>

// Windows why
#undef CopyFile
#undef DeleteFile
#undef MoveFile

#include <io.h>
#define check_dir_access(path) _access(path, 0x6)

#else

#include <unistd.h>
#define check_dir_access(path) access(path, R_OK | W_OK | X_OK)

#endif

// These are mostly just wrappers around std::filesystem functions.
using namespace std::filesystem;

namespace FileUtil
{

#define ToPath(Path) u8path(Path)

bool Exists(const TString &rkFilePath)
{
    return exists(ToPath(*rkFilePath));
}

bool IsRoot(const TString& rkPath)
{
    // todo: is this actually a good/multiplatform way of checking for root?
    TString AbsPath = MakeAbsolute(rkPath);
    TStringList Split = AbsPath.Split("\\/");
    return (Split.size() <= 1);
}

bool IsFile(const TString& rkFilePath)
{
    return is_regular_file(ToPath(*rkFilePath));
}

bool IsDirectory(const TString& rkDirPath)
{
    return is_directory(ToPath(*rkDirPath));
}

bool IsDirectoryWritable(const TString& rkDirPath)
{
    return check_dir_access(*rkDirPath) == 0;
}

bool IsAbsolute(const TString& rkDirPath)
{
    return ToPath(*rkDirPath).is_absolute();
}

bool IsRelative(const TString& rkDirPath)
{
    return ToPath(*rkDirPath).is_relative();
}

bool IsEmpty(const TString& rkDirPath)
{
    if (!IsDirectory(rkDirPath))
    {
        errorf("Non-directory path passed to IsEmpty(): %s", *rkDirPath);
        return false;
    }

    return is_empty(ToPath(*rkDirPath));
}

bool MakeDirectory(const TString& rkNewDir)
{
    if (!IsValidPath(rkNewDir, true))
    {
        errorf("Unable to create directory because name contains illegal characters: %s", *rkNewDir);
        return false;
    }

    // Sometimes the MS implementation returns false with a zero-error for some reason
    std::error_code err;
    return create_directories(ToPath(*rkNewDir), err) || !err;
}

bool CopyFile(const TString& rkOrigPath, const TString& rkNewPath)
{
    if (!IsValidPath(rkNewPath, false))
    {
        errorf("Unable to copy file because destination name contains illegal characters: %s", *rkNewPath);
        return false;
    }

    MakeDirectory(rkNewPath.GetFileDirectory());
    std::error_code Error;
    // call std::filesystem::copy, not std::copy
    std::filesystem::copy(ToPath(*rkOrigPath), ToPath(*rkNewPath), Error);
    return (Error.value() == 0);
}

bool CopyDirectory(const TString& rkOrigPath, const TString& rkNewPath)
{
    if (!IsValidPath(rkNewPath, true))
    {
        errorf("Unable to copy directory because destination name contains illegal characters: %s", *rkNewPath);
        return false;
    }

    MakeDirectory(rkNewPath.GetFileDirectory());
    std::error_code Error;
    // call std::filesystem::copy, not std::copy
    std::filesystem::copy(ToPath(*rkOrigPath), ToPath(*rkNewPath), Error);
    return (Error.value() == 0);
}

bool MoveFile(const TString& rkOldPath, const TString& rkNewPath)
{
    if (!IsValidPath(rkNewPath, false))
    {
        errorf("Unable to move file because destination name contains illegal characters: %s", *rkNewPath);
        return false;
    }

    if (Exists(rkNewPath))
    {
        errorf("Unable to move file because there is an existing file at the destination path: %s", *rkNewPath);
        return false;
    }

    std::error_code Error;
    rename(ToPath(*rkOldPath), ToPath(*rkNewPath), Error);
    return Error.value() == 0;
}

bool MoveDirectory(const TString& rkOldPath, const TString& rkNewPath)
{
    if (!IsValidPath(rkNewPath, true))
    {
        errorf("Unable to move directory because destination name contains illegal characters: %s", *rkNewPath);
        return false;
    }

    if (Exists(rkNewPath))
    {
        errorf("Unable to move directory because there is an existing directory at the destination path: %s", *rkNewPath);
        return false;
    }

    std::error_code Error;
    rename(ToPath(*rkOldPath), ToPath(*rkNewPath), Error);
    return Error.value() == 0;
}

bool DeleteFile(const TString& rkFilePath)
{
    if (!IsFile(rkFilePath)) return false;
    return remove(ToPath(*rkFilePath)) == 1;
}

bool DeleteDirectory(const TString& rkDirPath, bool FailIfNotEmpty)
{
    // This is an extremely destructive function, be careful using it!
    if (!IsDirectory(rkDirPath)) return false;

    // Sanity check - don't delete root
    bool Root = IsRoot(rkDirPath);

    if (Root)
    {
        ASSERT(false);
        fatalf("Attempted to delete root directory!");
        return false;
    }

    // Check if directory is empty
    if (FailIfNotEmpty && !IsEmpty(rkDirPath))
        return false;

    // Delete directory
    std::error_code Error;
    remove_all(ToPath(*rkDirPath), Error);
    return (Error.value() == 0);
}

bool ClearDirectory(const TString& rkDirPath)
{
    // This is an extremely destructive function, be careful using it!
    if (!IsDirectory(rkDirPath)) return false;

    // Sanity check - don't clear root
    bool Root = IsRoot(rkDirPath);

    if (Root)
    {
        ASSERT(false);
        fatalf("Attempted to clear root directory!");
        return false;
    }

    // Delete directory contents
    TStringList DirContents;
    GetDirectoryContents(rkDirPath, DirContents, false);

    for (auto It = DirContents.begin(); It != DirContents.end(); It++)
    {
        bool Success = false;

        if (IsFile(*It))
            Success = DeleteFile(*It);
        else if (IsDirectory(*It))
            Success = DeleteDirectory(*It, false);

        if (!Success)
            errorf("Failed to delete filesystem object: %s", *TString(*It));
    }

    return true;
}

void MarkHidden(const TString& rkFilePath, bool Hidden)
{
#ifdef WIN32
    T16String FilePath16 = rkFilePath.ToUTF16();
    DWORD Attrs = GetFileAttributesW( ToWChar(FilePath16) );

    if (Hidden)
        Attrs |= FILE_ATTRIBUTE_HIDDEN;
    else
        Attrs &= ~FILE_ATTRIBUTE_HIDDEN;

    SetFileAttributesW( ToWChar(FilePath16), Attrs );
#else
    errorf("MarkHidden unimplemented: %s", *rkFilePath);
#endif
}

void UpdateLastModifiedTime(const TString& rkFilePath)
{
    last_write_time( ToPath(*rkFilePath), file_time_type::clock::now() );
}

uint64 FileSize(const TString &rkFilePath)
{
    return (uint64) (Exists(rkFilePath) ? file_size(ToPath(*rkFilePath)) : -1);
}

uint64 LastModifiedTime(const TString& rkFilePath)
{
    return (uint64) last_write_time(ToPath(*rkFilePath)).time_since_epoch().count();
}

TString WorkingDirectory()
{
    return current_path().string();
}

TString MakeAbsolute(TString Path)
{
    if (!ToPath(*Path).has_root_path())
        Path = WorkingDirectory() + "/" + Path;

    TStringList Components = Path.Split("/\\");
    TStringList::iterator Prev;

    for (TStringList::iterator Iter = Components.begin(); Iter != Components.end(); Iter++)
    {
        if (*Iter == ".")
            Iter = Components.erase(Iter);
        else if (*Iter == "..")
            Iter = std::prev(Components.erase(Prev, std::next(Iter)));

        Prev = Iter;
    }

    TString Out;
    for (auto I = Path.Begin(), E = Path.End(); I != E && (*I == '/' || *I == '\\'); ++I)
        Out += *I;
    for (auto it = Components.begin(); it != Components.end(); it++)
        Out += *it + "/";

    return Out;
}

TString MakeRelative(const TString& rkPath, const TString& rkRelativeTo /*= WorkingDirectory()*/)
{
    TString AbsPath = MakeAbsolute(rkPath);
    TString AbsRelTo = MakeAbsolute(rkRelativeTo);
    TStringList PathComponents = AbsPath.Split("/\\");
    TStringList RelToComponents = AbsRelTo.Split("/\\");

    // Find furthest common parent
    TStringList::iterator PathIter = PathComponents.begin();
    TStringList::iterator RelToIter = RelToComponents.begin();

    for (; PathIter != PathComponents.end() && RelToIter != RelToComponents.end(); PathIter++, RelToIter++)
    {
        if (*PathIter != *RelToIter)
            break;
    }

    // If there's no common components, then we can't construct a relative path...
    if (PathIter == PathComponents.begin())
        return AbsPath;

    // Construct output path
    TString Out;

    for (; RelToIter != RelToComponents.end(); RelToIter++)
        Out += "../";

    for (; PathIter != PathComponents.end(); PathIter++)
        Out += *PathIter + "/";

    // Attempt to detect if this path is a file as opposed to a directory; if so, remove trailing backslash
    if (PathComponents.back().Contains('.') && !rkPath.EndsWith('/') && !rkPath.EndsWith('\\'))
        Out = Out.ChopBack(1);

    return Out;
}

TString SimplifyRelativePath(const TString& rkPath)
{
    TStringList PathComponents = rkPath.Split("/\\");

    TStringList::iterator Iter = PathComponents.begin();
    TStringList::iterator PrevIter = Iter;

    for (auto Iter = PathComponents.begin(); Iter != PathComponents.end(); PrevIter = Iter, Iter++)
    {
        if (*Iter == ".." && *PrevIter != "..")
        {
            PrevIter = PathComponents.erase(PrevIter);
            PrevIter = PathComponents.erase(PrevIter);
            Iter = PrevIter;
            Iter--;
        }
    }

    TString Out;

    for (auto Iter = PathComponents.begin(); Iter != PathComponents.end(); Iter++)
        Out += *Iter + '/';

    return Out;
}

uint32 MaxFileNameLength()
{
    return 255;
}

static const char gskIllegalNameChars[] = {
    '<', '>', '\"', '/', '\\', '|', '?', '*', ':'
};

TString SanitizeName(TString Name, bool Directory, bool RootDir /*= false*/)
{
    // Windows only atm
    if (Directory && (Name == "." || Name == ".."))
        return Name;

    // Remove illegal characters from path
    for (uint32 iChr = 0; iChr < Name.Size(); iChr++)
    {
        char Chr = Name[iChr];
        bool Remove = false;

        if (Chr >= 0 && Chr <= 31)
            Remove = true;

        // Allow colon only as the last character of root
        bool IsLegalColon = (Chr == ':' && RootDir && iChr == Name.Size() - 1);

        if (!IsLegalColon && !IsValidFileNameCharacter(Chr))
            Remove = true;

        if (Remove)
        {
            Name.Remove(iChr, 1);
            iChr--;
        }
    }

    // For directories, space and dot are not allowed at the end of the path
    if (Directory)
    {
        int ChopNum = 0;

        for (int iChr = (int) Name.Size() - 1; iChr >= 0; iChr--)
        {
            char Chr = Name[iChr];

            if (Chr == ' ' || Chr == '.')
                ChopNum++;
            else
                break;
        }

        if (ChopNum > 0) Name = Name.ChopBack(ChopNum);
    }

    // Remove spaces from beginning of path
    uint NumLeadingSpaces = 0;
    while (NumLeadingSpaces < Name.Size() && Name[NumLeadingSpaces] == ' ')
        NumLeadingSpaces++;

    if (NumLeadingSpaces > 0)
        Name = Name.ChopFront(NumLeadingSpaces);

    // Ensure the name is below the character limit
    if (Name.Size() > MaxFileNameLength())
    {
        int ChopNum = Name.Size() - MaxFileNameLength();
        Name = Name.ChopBack(ChopNum);
    }

    return Name;
}

TString SanitizePath(TString Path, bool Directory)
{
    TStringList Components = Path.Split("\\/");
    uint32 CompIdx = 0;
    Path = "";

    for (auto It = Components.begin(); It != Components.end(); It++)
    {
        TString Comp = *It;
        bool IsDir = Directory || CompIdx < Components.size() - 1;
        bool IsRoot = CompIdx == 0;
        Comp = SanitizeName(Comp, IsDir, IsRoot);

        Path += Comp;
        if (IsDir) Path += '/';
        CompIdx++;
    }

    return Path;
}

bool IsValidFileNameCharacter(char Chr)
{
    static const uint32 skNumIllegalChars = sizeof(gskIllegalNameChars) / sizeof(char);

    if (Chr >= 0 && Chr <= 31)
        return false;

    for (uint32 BanIdx = 0; BanIdx < skNumIllegalChars; BanIdx++)
    {
        if (Chr == gskIllegalNameChars[BanIdx])
            return false;
    }

    return true;
}

bool IsValidName(const TString& rkName, bool Directory, bool RootDir /*= false*/)
{
    // Only accounting for Windows limitations right now. However, this function should
    // ideally return the same output on all platforms to ensure projects are cross platform.
    if (rkName.IsEmpty())
        return false;

    if (rkName.Size() > MaxFileNameLength())
        return false;

    if (Directory && (rkName == "." || rkName == ".."))
        return true;

    // Check for banned characters
    for (uint32 iChr = 0; iChr < rkName.Size(); iChr++)
    {
        char Chr = rkName[iChr];

        // Allow colon only as the last character of root
        bool IsLegalColon = (Chr == ':' && RootDir && iChr == rkName.Size() - 1);

        if (!IsLegalColon && !IsValidFileNameCharacter(Chr))
            return false;
    }

    if (Directory && (rkName.Back() == ' ' || rkName.Back() == '.'))
        return false;

    return true;
}

bool IsValidPath(const TString& rkPath, bool Directory)
{
    // Only accounting for Windows limitations right now. However, this function should
    // ideally return the same output on all platforms to ensure projects are cross platform.
    TStringList Components = rkPath.Split("\\/");

    // Validate other components
    uint32 CompIdx = 0;

    for (auto It = Components.begin(); It != Components.end(); It++)
    {
        bool IsRoot = CompIdx == 0;
        bool IsDir = Directory || CompIdx < (Components.size() - 1);

        if (!IsValidName(*It, IsDir, IsRoot))
            return false;

        CompIdx++;
    }

    return true;
}

void GetDirectoryContents(TString DirPath, TStringList& rOut, bool Recursive /*= true*/, bool IncludeFiles /*= true*/, bool IncludeDirs /*= true*/)
{
    if (IsDirectory(DirPath))
    {
        DirPath.Replace("\\", "/");
        bool IncludeAll = IncludeFiles && IncludeDirs;

        auto AddFileLambda = [IncludeFiles, IncludeDirs, IncludeAll, &rOut](const TString& rkPath) -> void {
            bool ShouldAddFile = IncludeAll || (IncludeFiles && IsFile(rkPath)) || (IncludeDirs && IsDirectory(rkPath));

            if (ShouldAddFile)
                rOut.push_back(rkPath);
        };

        if (Recursive)
        {
            for (recursive_directory_iterator It(ToPath(*DirPath)); It != recursive_directory_iterator(); ++It)
            {
                AddFileLambda(It->path().string());
            }
        }

        else
        {
            for (directory_iterator It(ToPath(*DirPath)); It != directory_iterator(); ++It)
            {
                AddFileLambda(It->path().string());
            }
        }
    }
}

TString FindFileExtension(const TString& rkDir, const TString& rkName)
{
    for (directory_iterator It(ToPath(*rkDir)); It != directory_iterator(); ++It)
    {
        TString Name = It->path().filename().string();
        if (Name.GetFileName(false) == rkName) return Name.GetFileExtension();
    }

    return "";
}

bool LoadFileToString(const TString& rkFilePath, TString& rOut)
{
    CFileInStream File(rkFilePath);

    if (File.IsValid())
    {
        rOut = TString(File.Size());
        File.ReadBytes(&rOut[0], rOut.Size());
        return true;
    }
    else
        return false;
}

bool LoadFileToBuffer(const TString& rkFilePath, std::vector<uint8>& Out)
{
    CFileInStream File(rkFilePath);

    if (File.IsValid())
    {
        Out.resize(File.Size());
        File.ReadBytes(Out.data(), Out.size());
        return true;
    }
    else
        return false;
}

bool SaveStringToFile(const TString& rkFilePath, const TString& kString)
{
    CFileOutStream File(rkFilePath);

    if (File.IsValid())
    {
        File.WriteBytes(&kString[0], kString.Size());
        return true;
    }
    else
        return false;
}

bool SaveBufferToFile(const TString& rkFilePath, const std::vector<uint8>& kBuffer)
{
    CFileOutStream File(rkFilePath);

    if (File.IsValid())
    {
        File.WriteBytes(kBuffer.data(), kBuffer.size());
        return true;
    }
    else
        return false;
}

}
