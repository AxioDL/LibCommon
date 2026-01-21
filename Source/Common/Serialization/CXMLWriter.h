#ifndef AXIO_CXMLWRITER_H
#define AXIO_CXMLWRITER_H

#include "Common/CFourCC.h"
#include "Common/Log.h"
#include "Common/Serialization/IArchive.h"
#include <tinyxml2.h>

class CXMLWriter : public IArchive
{
    tinyxml2::XMLDocument mDoc;
    tinyxml2::XMLElement *mpCurElem = nullptr;
    TString mOutFilename;
    const char* mpAttributeName = nullptr;
    bool mSaved = false;

public:
    explicit CXMLWriter(const TString& rkFileName, const TString& rkRootName, uint16_t FileVersion = 0, EGame Game = EGame::Invalid)
        : mOutFilename(rkFileName)
    {
        mArchiveFlags = AF_Writer | AF_Text;
        SetVersion(skCurrentArchiveVersion, FileVersion, Game);

        // Create declaration and root node
        tinyxml2::XMLDeclaration *pDecl = mDoc.NewDeclaration();
        mDoc.LinkEndChild(pDecl);

        mpCurElem = mDoc.NewElement(*rkRootName);
        mDoc.LinkEndChild(mpCurElem);

        // Write version data
        SerializeVersion();
    }

    ~CXMLWriter() override
    {
        if (!mSaved)
        {
            [[maybe_unused]] bool SaveSuccess = Save();
            ASSERT(SaveSuccess);
        }
    }

    bool Save()
    {
        if (mSaved)
        {
            NLog::Error("Attempted to save XML twice!");
            return false;
        }

        tinyxml2::XMLError Error = mDoc.SaveFile(*mOutFilename);
        mSaved = true;

        if (Error != tinyxml2::XML_SUCCESS)
        {
            NLog::Error("Failed to save XML file: {}", mOutFilename.ToStdString());
            return false;
        }

        return true;
    }

    bool IsValid() const
    {
        return mpCurElem != nullptr && !mSaved;
    }

    // Interface
    bool ParamBegin(const char *pkName, uint32_t Flags) override
    {
        ASSERT(IsValid());
        ASSERT(!mpAttributeName); // Attributes cannot have sub-children

        // Read as attribute if needed
        if (Flags & SH_Attribute)
        {
            mpAttributeName = pkName;
        }
        else
        {
            tinyxml2::XMLElement *pElem = mDoc.NewElement(pkName);
            mpCurElem->LinkEndChild(pElem);
            mpCurElem = pElem;
        }

        return true;
    }

    void ParamEnd() override
    {
        if (mpAttributeName)
        {
            mpAttributeName = nullptr;
        }
        else
        {
            // If we didn't save any sub parameters, remove the element.
            tinyxml2::XMLElement* pParent = mpCurElem->Parent()->ToElement();

            if ( mpCurElem->FirstAttribute() == nullptr
                 && mpCurElem->FirstChild() == nullptr
                 && mpCurElem->GetText() == nullptr )
            {
                pParent->DeleteChild(mpCurElem);
            }

            mpCurElem = pParent;
        }
    }

protected:
    void WriteParam(const char *pkValue)
    {
        if (mpAttributeName)
            mpCurElem->SetAttribute(mpAttributeName, pkValue);
        else
            mpCurElem->SetText(pkValue);
    }

public:
    bool PreSerializePointer(void*& Pointer, uint32_t Flags) override
    {
        if (!Pointer)
        {
            mpCurElem->SetText("NULL");
            return false;
        }
        return true;
    }

    void SerializeArraySize(uint32_t& Value) override
    {
        // Do nothing. Reader obtains container size from number of child elements
    }

    void SerializePrimitive(bool& rValue, uint32_t Flags) override     { WriteParam(rValue ? "true" : "false"); }
    void SerializePrimitive(char& rValue, uint32_t Flags) override     { WriteParam(*TString(rValue)); }
    void SerializePrimitive(int8_t& rValue, uint32_t Flags) override   { WriteParam((Flags & SH_HexDisplay) ? *TString::HexString((uint8_t)rValue, 0) : *TString::FromInt32(rValue, 0, 10)); }
    void SerializePrimitive(uint8_t& rValue, uint32_t Flags) override  { WriteParam((Flags & SH_HexDisplay) ? *TString::HexString(rValue, 0) : *TString::FromInt32(rValue, 0, 10)); }
    void SerializePrimitive(int16_t& rValue, uint32_t Flags) override  { WriteParam((Flags & SH_HexDisplay) ? *TString::HexString((uint16_t)rValue, 0) : *TString::FromInt32(rValue, 0, 10)); }
    void SerializePrimitive(uint16_t& rValue, uint32_t Flags) override { WriteParam((Flags & SH_HexDisplay) ? *TString::HexString(rValue, 0) : *TString::FromInt32(rValue, 0, 10)); }
    void SerializePrimitive(int32_t& rValue, uint32_t Flags) override  { WriteParam((Flags & SH_HexDisplay) ? *TString::HexString((uint32_t)rValue, 0) : *TString::FromInt32(rValue, 0, 10)); }
    void SerializePrimitive(uint32_t& rValue, uint32_t Flags) override { WriteParam((Flags & SH_HexDisplay) ? *TString::HexString(rValue, 0) : *TString::FromInt32(rValue, 0, 10)); }
    void SerializePrimitive(int64_t& rValue, uint32_t Flags) override  { WriteParam(*TString::FromInt64(rValue, 0, (Flags & SH_HexDisplay) ? 16 : 10)); }
    void SerializePrimitive(uint64_t& rValue, uint32_t Flags) override { WriteParam(*TString::FromInt64(rValue, 0, (Flags & SH_HexDisplay) ? 16 : 10)); }
    void SerializePrimitive(float& rValue, uint32_t Flags) override    { WriteParam(*TString::FromFloat(rValue, 1, true)); }
    void SerializePrimitive(double& rValue, uint32_t Flags) override   { WriteParam(*TString::FromFloat((float)rValue, 1, true)); }
    void SerializePrimitive(TString& rValue, uint32_t Flags) override  { WriteParam(*rValue); }
    void SerializePrimitive(CFourCC& rValue, uint32_t Flags) override  { WriteParam(*rValue.ToString()); }
    void SerializePrimitive(CAssetID& rValue, uint32_t Flags) override { WriteParam(*rValue.ToString(CAssetID::GameIDLength(Game()))); }

    void SerializeBulkData(void* pData, uint32_t Size, uint32_t Flags) override
    {
        char* pCharData = (char*) pData;
        TString OutString(Size*2);

        for (uint32_t ByteIdx = 0; ByteIdx < Size; ByteIdx++)
        {
            //@todo: sloooooow. maybe replace with a LUT?
            sprintf(&OutString[ByteIdx*2], "%02X", pCharData[ByteIdx]);
        }

        WriteParam(*OutString);
    }
};

#endif // AXIO_CXMLWRITER_H
