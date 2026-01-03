#ifndef CXMLREADER
#define CXMLREADER

#include "Common/Log.h"
#include "Common/Serialization/IArchive.h"
#include "Common/Serialization/XMLCommon.h"
#include <tinyxml2.h>

class CXMLReader : public IArchive
{
    tinyxml2::XMLDocument mDoc;
    tinyxml2::XMLElement *mpCurElem = nullptr; // Points to the next element being read
    const char* mpAttribute = nullptr;         // Name of the parameter we are reading from an attribute
    bool mJustEndedParam = false;              // Indicates we just ended a primitive parameter

public:
    explicit CXMLReader(const TString& rkFileName)
    {
        mArchiveFlags = AF_Reader | AF_Text;

        // Load XML and set current element to the root element; read version
        tinyxml2::XMLError Error = mDoc.LoadFile(*rkFileName);

        if (Error != tinyxml2::XML_SUCCESS)
        {
            NLog::Error("{}: Failed to open XML for read: {}", rkFileName.ToStdString(), XMLErrorString(Error));
        }
        else
        {
            mpCurElem = mDoc.FirstChildElement();
            ASSERT(mpCurElem != nullptr);
            SerializeVersion();
        }
    }

    bool IsValid() const
    {
        return mpCurElem != nullptr;
    }

    // Interface
    bool ParamBegin(const char *pkName, uint32 Flags) override
    {
        ASSERT(IsValid());
        ASSERT(!mpAttribute); // Attributes cannot have sub-children

        // Store as an attribute if requested
        if (Flags & SH_Attribute)
        {
            mpAttribute = mpCurElem->Attribute(pkName);
            return mpAttribute != nullptr;
        }

        // Push new parent if needed
        if (!mJustEndedParam)
        {
            tinyxml2::XMLElement *pChild = mpCurElem->FirstChildElement();
            if (!pChild) return false;
            else mpCurElem = pChild;
        }

        // Verify the current element matches the name of the next serialized parameter.
        if ( (Flags & SH_IgnoreName) || strcmp(mpCurElem->Name(), pkName) == 0 )
        {
            mJustEndedParam = false;
            return true;
        }

        // It didn't match, so we'll try to find a sibling element that does match.
        // Iterate over all sibling elements - if we find a match we will continue
        // reading from that point on. Otherwise we can't load this parameter.
        tinyxml2::XMLElement *pSearchElem = mpCurElem->Parent()->FirstChildElement();

        while (pSearchElem)
        {
            if ( strcmp(pSearchElem->Name(), pkName) == 0 )
            {
                mpCurElem = pSearchElem;
                mJustEndedParam = false;
                return true;
            }

            pSearchElem = pSearchElem->NextSiblingElement();
        }

        // We couldn't find a matching element, so we can't load this parameter.
        // If we pushed a parent earlier, undo it.
        if (!mJustEndedParam)
            mpCurElem = mpCurElem->Parent()->ToElement();

        return false;
    }

    void ParamEnd() override
    {
        if (mpAttribute)
            mpAttribute = nullptr;

        else
        {
            if (mJustEndedParam)
                mpCurElem = mpCurElem->Parent()->ToElement();

            tinyxml2::XMLElement *pElem = mpCurElem->NextSiblingElement();
            if (pElem)
                mpCurElem = pElem;

            mJustEndedParam = true;
        }
    }

protected:
    TString ReadParam()
    {
        return TString(mpAttribute ? mpAttribute : mpCurElem->GetText());
    }

public:
    void SerializeArraySize(uint32& Value) override
    {
        Value = 0;

        for (tinyxml2::XMLElement *pElem = mpCurElem->FirstChildElement(); pElem; pElem = pElem->NextSiblingElement())
            Value++;
    }

    bool PreSerializePointer(void*& InPointer, uint32 Flags) override
    {
        return mpCurElem->GetText() == nullptr || strcmp(mpCurElem->GetText(), "NULL") != 0;
    }

    void SerializePrimitive(bool& rValue, uint32 Flags)  override    { rValue = ReadParam() == "true"; }
    void SerializePrimitive(char& rValue, uint32 Flags) override     { rValue = ReadParam().Front(); }
    void SerializePrimitive(int8& rValue, uint32 Flags) override     { rValue = (int8)ReadParam().ToInt32((Flags & SH_HexDisplay) ? 16 : 0); }
    void SerializePrimitive(uint8& rValue, uint32 Flags) override    { rValue = (uint8)ReadParam().ToInt32((Flags & SH_HexDisplay) ? 16 : 0); }
    void SerializePrimitive(int16& rValue, uint32 Flags) override    { rValue = (int16)ReadParam().ToInt32((Flags & SH_HexDisplay) ? 16 : 0); }
    void SerializePrimitive(uint16& rValue, uint32 Flags) override   { rValue = (uint16)ReadParam().ToInt32((Flags & SH_HexDisplay) ? 16 : 0); }
    void SerializePrimitive(int32& rValue, uint32 Flags) override    { rValue = (int32)ReadParam().ToInt32((Flags & SH_HexDisplay) ? 16 : 0); }
    void SerializePrimitive(uint32& rValue, uint32 Flags) override   { rValue = (uint32)ReadParam().ToInt32((Flags & SH_HexDisplay) ? 16 : 0); }
    void SerializePrimitive(int64& rValue, uint32 Flags) override    { rValue = (int64)ReadParam().ToInt64((Flags & SH_HexDisplay) ? 16 : 0); }
    void SerializePrimitive(uint64& rValue, uint32 Flags) override   { rValue = (uint64)ReadParam().ToInt64((Flags & SH_HexDisplay) ? 16 : 0); }
    void SerializePrimitive(float& rValue, uint32 Flags) override    { rValue = ReadParam().ToFloat(); }
    void SerializePrimitive(double& rValue, uint32 Flags) override   { rValue = (double)ReadParam().ToFloat(); }
    void SerializePrimitive(TString& rValue, uint32 Flags) override  { rValue = ReadParam(); }
    void SerializePrimitive(CFourCC& rValue, uint32 Flags) override  { rValue = CFourCC(ReadParam()); }
    void SerializePrimitive(CAssetID& rValue, uint32 Flags) override { rValue = CAssetID::FromString(ReadParam()); }

    void SerializeBulkData(void* pData, uint32 Size, uint32 Flags) override
    {
        char* pCharData = (char*) pData;
        TString StringData = ReadParam();
        ASSERT(StringData.Size() == Size*2);

        for (uint32 ByteIdx = 0; ByteIdx < Size; ByteIdx++)
        {
            *pCharData = (char) StringData.SubString(ByteIdx*2, 2).ToInt32(16);
            pCharData++;
        }
    }
};

#endif // CXMLREADER

