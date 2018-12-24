#ifndef CXMLWRITER
#define CXMLWRITER

#include "IArchive.h"
#include "Common/CFourCC.h"
#include <iostream>
#include <tinyxml2.h>

class CXMLWriter : public IArchive
{
    tinyxml2::XMLDocument mDoc;
    tinyxml2::XMLElement *mpCurElem;
    TString mOutFilename;
    const char* mpAttributeName;
    bool mSaved;

public:
    CXMLWriter(const TString& rkFileName, const TString& rkRootName, uint16 FileVersion = 0, EGame Game = EGame::Invalid)
        : IArchive()
        , mOutFilename(rkFileName)
        , mpAttributeName(nullptr)
        , mSaved(false)
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

    ~CXMLWriter()
    {
        if (!mSaved)
        {
            bool SaveSuccess = Save();
            ASSERT(SaveSuccess);
        }
    }

    inline bool Save()
    {
        if (mSaved)
        {
            errorf("Attempted to save XML twice!");
            return false;
        }

        tinyxml2::XMLError Error = mDoc.SaveFile(*mOutFilename);
        mSaved = true;

        if (Error != tinyxml2::XML_SUCCESS)
        {
            errorf("Failed to save XML file: %s", *mOutFilename);
            return false;
        }
        else
            return true;
    }

    inline bool IsValid() const
    {
        return mpCurElem != nullptr && !mSaved;
    }

    // Interface
    virtual bool ParamBegin(const char *pkName, uint32 Flags)
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

    virtual void ParamEnd()
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
    virtual bool PreSerializePointer(void*& Pointer, uint32 Flags)
    {
        if (!Pointer)
        {
            mpCurElem->SetText("NULL");
            return false;
        }
        return true;
    }

    virtual void SerializeArraySize(uint32& Value)
    {
        // Do nothing. Reader obtains container size from number of child elements
    }

    virtual void SerializePrimitive(bool& rValue, uint32 Flags)         { WriteParam(rValue ? "true" : "false"); }
    virtual void SerializePrimitive(char& rValue, uint32 Flags)         { WriteParam(*TString(rValue)); }
    virtual void SerializePrimitive(int8& rValue, uint32 Flags)         { WriteParam( (Flags & SH_HexDisplay) ? *TString::HexString((uint8&)  rValue, 0) : *TString::FromInt32(rValue, 0, 10) ); }
    virtual void SerializePrimitive(uint8& rValue, uint32 Flags)        { WriteParam( (Flags & SH_HexDisplay) ? *TString::HexString((uint8&)  rValue, 0) : *TString::FromInt32(rValue, 0, 10) ); }
    virtual void SerializePrimitive(int16& rValue, uint32 Flags)        { WriteParam( (Flags & SH_HexDisplay) ? *TString::HexString((uint16&) rValue, 0) : *TString::FromInt32(rValue, 0, 10) ); }
    virtual void SerializePrimitive(uint16& rValue, uint32 Flags)       { WriteParam( (Flags & SH_HexDisplay) ? *TString::HexString((uint16&) rValue, 0) : *TString::FromInt32(rValue, 0, 10) ); }
    virtual void SerializePrimitive(int32& rValue, uint32 Flags)        { WriteParam( (Flags & SH_HexDisplay) ? *TString::HexString((uint32&) rValue, 0) : *TString::FromInt32(rValue, 0, 10) ); }
    virtual void SerializePrimitive(uint32& rValue, uint32 Flags)       { WriteParam( (Flags & SH_HexDisplay) ? *TString::HexString((uint32&) rValue, 0) : *TString::FromInt32(rValue, 0, 10) ); }
    virtual void SerializePrimitive(int64& rValue, uint32 Flags)        { WriteParam( *TString::FromInt64(rValue, 0, (Flags & SH_HexDisplay) ? 16 : 10) ); }
    virtual void SerializePrimitive(uint64& rValue, uint32 Flags)       { WriteParam( *TString::FromInt64(rValue, 0, (Flags & SH_HexDisplay) ? 16 : 10) ); }
    virtual void SerializePrimitive(float& rValue, uint32 Flags)        { WriteParam( *TString::FromFloat(rValue, 1, true) ); }
    virtual void SerializePrimitive(double& rValue, uint32 Flags)       { WriteParam( *TString::FromFloat((float) rValue, 1, true) ); }
    virtual void SerializePrimitive(TString& rValue, uint32 Flags)      { WriteParam( *rValue ); }
    virtual void SerializePrimitive(CFourCC& rValue, uint32 Flags)      { WriteParam( *rValue.ToString() ); }
    virtual void SerializePrimitive(CAssetID& rValue, uint32 Flags)     { WriteParam( *rValue.ToString( CAssetID::GameIDLength(Game()) ) ); }

    virtual void SerializeBulkData(void* pData, uint32 Size, uint32 Flags)
    {
        char* pCharData = (char*) pData;
        TString OutString(Size*2);

        for (uint32 ByteIdx = 0; ByteIdx < Size; ByteIdx++)
        {
            //@todo: sloooooow. maybe replace with a LUT?
            sprintf(&OutString[ByteIdx*2], "%02X", pCharData[ByteIdx]);
        }

        WriteParam(*OutString);
    }
};

#endif // CXMLWRITER
