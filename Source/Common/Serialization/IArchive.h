#ifndef AXIO_IARCHIVE_H
#define AXIO_IARCHIVE_H

#include "CSerialVersion.h"
#include "Common/Macros.h"
#include "Common/CAssetID.h"
#include "Common/CFourCC.h"
#include "Common/EGame.h"
#include "Common/TString.h"

#include <concepts>
#include <cstdint>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <type_traits>
#include <unordered_map>
#include <vector>

/* This is a custom serialization implementation intended for saving game assets out to editor-
 * friendly formats, such as XML. The main goals of the serialization system is to simplify the
 * code for reading and writing editor files and to be able to easily update those files without
 * breaking compatibility with older versions. Support for new output formats can be added by
 * implementing new subclasses of IArchive.
 *
 * To use a class with the serialization system, it must have a Serialize function implemented.
 * There are two ways this function can be defined:
 * 1. As a member function with this signature: void Serialize(IArchive&)
 * 2. As a global/friend function with this signature: void Serialize(IArchive&, YourClass&)
 *
 * Use the << operator to serialize data members to the archive. All data being serialized must be
 * wrapped in a call to SerialParameter(), which allows each value to be associated with a name
 * and other parameters. This primarily helps make the output files more human-readable and
 * assists with backwards compatibility, as well as customizing how each parameter is serialized.
 *
 * Polymorphism is supported. There are two requirements for a polymorphic class to work with the
 * serialization system. First, the base class must contain a virtual Type() function, as well as
 * a virtual Serialize(IArchive&) function. Second, the class must have a static ArchiveConstructor()
 * method that takes the a Type value (which should be the same type as Type() returns), and returns
 * an object of the correct class.
 *
 * SerialParameter() can take flags that provides hints to the serializer. This can either customize
 * how the parameter is displayed in the file, or it can modify how the serialization is done under
 * the hood. For a list of possible hints, check the definition of ESerialHint.
 */

/** ESerialHint - Parameter hint flags */
enum ESerialHint : uint32_t
{
    SH_HexDisplay           = 0x1,      // The parameter should be written in hex in text formats.
    SH_Optional             = 0x2,      // The parameter should not be written to the file if its value matches the default value.
    SH_NeverSave            = 0x4,      // The parameter should not be saved to files.
    SH_AlwaysSave           = 0x8,      // The parameter should always be saved regardless of if it matches the default value.
    SH_Attribute            = 0x10,     // The parameter is an attribute of its parent. Attributes cannot have children.
    SH_IgnoreName           = 0x20,     // The parameter name will not be used to validate file data. May yield incorrect results if used improperly!
    SH_InheritHints         = 0x40,     // The parameter will inherit hints from its parent parameter (except for this flag).
    SH_Proxy                = 0x80,     // The parameter is a proxy of the parent and will display inline instead of as a child parameter.
};

// Hints that can be inherited by SH_InheritHints and SH_Proxy
constexpr uint32_t gkInheritableSerialHints = (SH_HexDisplay | SH_NeverSave | SH_AlwaysSave);

/** EArchiveFlags */
enum EArchiveFlags : uint32_t
{
    AF_Reader               = 0x1,      // Archive reads data.
    AF_Writer               = 0x2,      // Archive writes data.
    AF_Text                 = 0x4,      // Archive reads/writes to a text format.
    AF_Binary               = 0x8,      // Archive reads/writes to a binary format.
    AF_NoSkipping           = 0x10,     // Properties are never skipped.
};

/** Class that determines if the type is a container */
template<typename>
struct TIsContainer : std::false_type {};
template<typename T>
struct TIsContainer<std::vector<T>> : std::true_type {};
template<typename T>
struct TIsContainer<std::list<T>> : std::true_type {};
template<typename T>
struct TIsContainer<std::set<T>> : std::true_type {};
template<typename T, typename V, typename Comp>
struct TIsContainer<std::map<T, V, Comp>> : std::true_type {};
template<typename T, typename V, typename Hash, typename Comp>
struct TIsContainer<std::unordered_map<T, V, Hash, Comp>> : std::true_type {};

/** Class that determines if the type is a smart pointer */
template<typename>   struct TIsSmartPointer : std::false_type {};
template<typename T> struct TIsSmartPointer<std::shared_ptr<T>> : std::true_type {};
template<typename T> struct TIsSmartPointer<std::unique_ptr<T>> : std::true_type {};

/** Helper macro that tells us whether the parameter supports default property values */
#define SUPPORTS_DEFAULT_VALUES (!std::is_pointer_v<ValType> && std::is_copy_assignable_v<ValType> && std::equality_comparable<ValType> && !TIsContainer<ValType>::value && !TIsSmartPointer<ValType>::value)

/** TSerialParameter - name/value pair for generic serial parameters */
template<typename ValType>
struct TSerialParameter
{
    const char*         pkName;
    ValType&            rValue;
    uint32_t            HintFlags;
    const ValType*      pDefaultValue;
};

/** Function that creates a SerialParameter */
template<typename ValType>
std::enable_if_t<SUPPORTS_DEFAULT_VALUES, TSerialParameter<ValType>>
inline SerialParameter(const char* pkName, ValType& rValue, uint32_t HintFlags = 0, const ValType& rkDefaultValue = ValType())
{
    return TSerialParameter<ValType> { pkName, rValue, HintFlags, &rkDefaultValue };
}
template<typename ValType>
std::enable_if_t<!SUPPORTS_DEFAULT_VALUES, TSerialParameter<ValType>>
inline SerialParameter(const char* pkName, ValType& rValue, uint32_t HintFlags = 0)
{
    return TSerialParameter<ValType> { pkName, rValue, HintFlags, nullptr };
}

/** Returns whether the parameter value matches its default value */
template<typename ValType>
std::enable_if_t<SUPPORTS_DEFAULT_VALUES, bool>
inline ParameterMatchesDefault(const TSerialParameter<ValType>& kParameter)
{
    return kParameter.pDefaultValue != nullptr && kParameter.rValue == *kParameter.pDefaultValue;
}

template<typename ValType>
std::enable_if_t<!SUPPORTS_DEFAULT_VALUES && TIsContainer<ValType>::value, bool>
inline ParameterMatchesDefault(const TSerialParameter<ValType>& kParameter)
{
    return kParameter.rValue.size() == 0;
}

template<typename ValType>
std::enable_if_t<!SUPPORTS_DEFAULT_VALUES && !TIsContainer<ValType>::value, bool>
inline ParameterMatchesDefault(const TSerialParameter<ValType>&)
{
    return false;
}

/** Initializes the parameter to its default value */
template<typename ValType>
std::enable_if_t<SUPPORTS_DEFAULT_VALUES, bool>
inline InitParameterToDefault(TSerialParameter<ValType>& Param)
{
    if (Param.pDefaultValue != nullptr)
    {
        Param.rValue = *Param.pDefaultValue;
        return true;
    }

    return false;
}

template<typename ValType>
std::enable_if_t<!SUPPORTS_DEFAULT_VALUES && TIsContainer<ValType>::value, bool>
inline InitParameterToDefault(TSerialParameter<ValType>& Param)
{
    Param.rValue.clear();
    return true;
}

template<typename ValType>
std::enable_if_t<!SUPPORTS_DEFAULT_VALUES && !TIsContainer<ValType>::value, bool>
inline InitParameterToDefault(TSerialParameter<ValType>&)
{
    return false;
}

// Concepts and types used to determine the existence of serializing functions.
enum class SerialType
{
    Primitive,
    Member,
    Global,
    None,
};

/** Check for ArcType::SerializePrimitive(ValType&) method */
template <class Value, typename ArcType>
concept HasPrimitiveSerialize = requires(Value& obj, ArcType arc) {
    { arc.SerializePrimitive(obj, uint32_t{}) } -> std::same_as<void>;
};
/** Check for ValType::Serialize(ArcType&) */
template <typename Value, class ArcType>
concept HasMemberSerialize = requires(Value obj, ArcType& arc) {
    { obj.Serialize(arc) } -> std::same_as<void>;
};
/** Check for global Serialize(ArcType&,ValType&) function */
template <typename Value, class ArcType>
concept HasGlobalSerialize = requires(Value& obj, ArcType& arc) {
    { Serialize(arc, obj) } -> std::same_as<void>;
};

template <typename Value, class ArcType>
constexpr SerialType MakeSerializableType() {
    return HasPrimitiveSerialize<Value, ArcType> ? SerialType::Primitive :
           HasMemberSerialize<Value, ArcType>    ? SerialType::Member    :
           HasGlobalSerialize<Value, ArcType>    ? SerialType::Global    : SerialType::None;
}
template <typename Value, class ArcType>
constexpr inline SerialType SerializeType = MakeSerializableType<Value, ArcType>();

// Archive constructor handling for abstract types
enum class ArcConstructorType
{
    Basic,
    Advanced,
    None,
};

template <typename T>
concept HasTypeMethod = requires(T obj) {
    { obj.Type() };
};

/**
 * Figure out the type being used to represent the object type.
 * If there isn't a type function, then it doesn't matter; just substitute int.
 */
template<typename T>
using TypeMethodReturn = std::conditional_t<HasTypeMethod<T>, decltype(std::declval<T>().Type()), int>;

/** Check for ValType::ArchiveConstructor(ObjectType) */
template <typename Value>
concept HasBasicArchiveConstructor = requires(Value obj) {
    { obj.ArchiveConstructor(TypeMethodReturn<Value>{}) };
};
/** Check for ValType::ArchiveConstructor(ObjectType, IArchive&) */
template <typename Value>
concept HasAdvancedArchiveConstructor = requires(Value obj) {
    { obj.ArchiveConstructor(TypeMethodReturn<Value>{}, std::declval<IArchive&>()) };
};
template <typename Value>
constexpr ArcConstructorType MakeArcConstructorType() {
    return HasAdvancedArchiveConstructor<Value> ? ArcConstructorType::Advanced :
           HasBasicArchiveConstructor<Value>    ? ArcConstructorType::Basic    : ArcConstructorType::None;
}
template <typename Value>
constexpr inline ArcConstructorType ArchiveConstructorType = MakeArcConstructorType<Value>();

/** Helper that turns functions on or off depending on their serialize type */
#define IS_SERIAL_TYPE(SType) (SerializeType<ValType, IArchive> == SerialType::SType)

/** Helper that turns functions on or off depending on their StaticConstructor type */
#define IS_ARCHIVE_CONSTRUCTOR_TYPE(CType) (ArchiveConstructorType<ValType> == ArcConstructorType::CType)

/** Helper that turns functions on or off depending on if the parameter type is abstract */
#define IS_ABSTRACT (std::is_abstract_v<ValType> || (std::is_polymorphic_v<ValType> && ArchiveConstructorType<ValType> != ArcConstructorType::None))

/** Helper that fetches the type used to represent an abstract object type */
#define ABSTRACT_TYPE TypeMethodReturn<ValType>

/** IArchive - Main serializer archive interface */
class IArchive
{
protected:
    uint16_t mArchiveVersion{};
    uint16_t mFileVersion{};
    EGame mGame = EGame::Invalid;

    // Subclasses must fill in flags in their constructors!!!
    uint32_t mArchiveFlags{};

    // Info about the stack of parameters being serialized
    struct SParmStackEntry
    {
        size_t TypeID;
        size_t TypeSize;
        void* pDataPointer;
        uint32_t HintFlags;
    };
    std::vector<SParmStackEntry> mParmStack;

public:
    enum EArchiveVersion
    {
        eArVer_Initial,
        eArVer_32BitBinarySize,
        eArVer_Refactor,
        eArVer_MapAttributes,
        eArVer_GameEnumClass,
        // Insert new versions before this line
        eArVer_Max
    };
    static constexpr uint32_t skCurrentArchiveVersion = (eArVer_Max - 1);

    IArchive() : mArchiveVersion(skCurrentArchiveVersion)
    {
        // hack to reduce allocations
        mParmStack.reserve(16);
    }

    virtual ~IArchive() { ASSERT(mParmStack.empty()); }

    // Serialize archive version. Always call after opening a file.
    void SerializeVersion()
    {
        *this << SerialParameter("ArchiveVer",  mArchiveVersion,    SH_Attribute)
              << SerialParameter("FileVer",     mFileVersion,       SH_Attribute | SH_Optional,     (uint16_t) 0)
              << SerialParameter("Game",        mGame,              SH_Attribute | SH_Optional,     EGame::Invalid);

        if (IsReader())
        {
            if (mArchiveVersion > skCurrentArchiveVersion)
            {
                mArchiveVersion = skCurrentArchiveVersion;
            }
        }
    }

private:
    // Attempts to start a new parameter. Return whether the parameter should be serialized.
    template<typename ValType>
    bool InternalStartParam(const TSerialParameter<ValType>& Param)
    {
        bool IsProxy = (Param.HintFlags & SH_Proxy) != 0;
        return ShouldSerializeParameter(Param) && (IsProxy || ParamBegin(Param.pkName, Param.HintFlags) );
    }

    // Ends a parameter.
    template<typename ValType>
    void InternalEndParam(const TSerialParameter<ValType>& Param)
    {
        if ((Param.HintFlags & SH_Proxy) == 0)
        {
            ParamEnd();
        }
    }

    // Return whether this parameter should be serialized
    template<typename ValType>
    bool ShouldSerializeParameter(const TSerialParameter<ValType>& Param) const
    {
        if ((mArchiveFlags & AF_NoSkipping) != 0)
            return true;

        if (IsWriter())
        {
            if ((Param.HintFlags & SH_NeverSave) != 0)
                return false;

            if ((Param.HintFlags & SH_Optional) != 0 &&
                (Param.HintFlags & SH_AlwaysSave) == 0 &&
                (ParameterMatchesDefault(Param)))
            {
                return false;
            }
        }

        return true;
    }

    // Instantiate an abstract object from the file
    // Only readers are allowed to instantiate objects
    template<typename ValType, typename ObjType = ABSTRACT_TYPE>
    std::enable_if_t<IS_ARCHIVE_CONSTRUCTOR_TYPE(Basic), ValType*>
    inline InstantiateAbstractObject(const TSerialParameter<ValType*>& Param, ObjType Type)
    {
        // Variant for basic static constructor
        ASSERT(IsReader());
        return (ValType*) ValType::ArchiveConstructor(Type);
    }

    template<typename ValType, typename ObjType = ABSTRACT_TYPE>
    std::enable_if_t<IS_ARCHIVE_CONSTRUCTOR_TYPE(Advanced), ValType*>
    InstantiateAbstractObject(const TSerialParameter<ValType*>& Param, ObjType Type)
    {
        // Variant for advanced static constructor
        ASSERT(IsReader());
        return (ValType*) ValType::ArchiveConstructor(Type, *this);
    }

    template<typename ValType, typename ObjType = ABSTRACT_TYPE>
    std::enable_if_t<IS_ARCHIVE_CONSTRUCTOR_TYPE(None), ValType*>
    InstantiateAbstractObject(const TSerialParameter<ValType*>& Param, ObjType Type)
    {
        // If you fail here, you are missing an ArchiveConstructor() function, or you do have one but it is malformed.
        // Check the comments at the top of this source file for details on serialization requirements for abstract objects.
        static_assert(!IS_ARCHIVE_CONSTRUCTOR_TYPE(None),
                      "Abstract objects being serialized must have virtual Type() and static ArchiveConstructor() functions.");
    }

    // Parameter stack handling
    template<typename ValType>
    inline void PushParameter(TSerialParameter<ValType>& Param)
    {
#if _DEBUG
        if (mParmStack.size() > 0)
        {
            // Attribute properties cannot have children!
            ASSERT((mParmStack.back().HintFlags & SH_Attribute) == 0);
        }
#endif

        // For InheritHints parameters, and for proxy parameters, copy the hint flags from the parent parameter.
        if ((Param.HintFlags & (SH_InheritHints | SH_Proxy)) != 0)
        {
            Param.HintFlags |= (mParmStack.back().HintFlags & gkInheritableSerialHints);
        }

        SParmStackEntry Entry;
        Entry.TypeID = typeid(ValType).hash_code();
        Entry.TypeSize = sizeof(ValType);
        Entry.pDataPointer = &Param.rValue;
        Entry.HintFlags = Param.HintFlags;
        mParmStack.push_back(Entry);
    }

    template<typename ValType>
    inline void PopParameter(const TSerialParameter<ValType>& Param)
    {
#if _DEBUG
        // Make sure the entry matches the param that has been passed in
        ASSERT(mParmStack.size() > 0);
        const SParmStackEntry& kEntry = mParmStack.back();
        ASSERT(kEntry.TypeID == typeid(ValType).hash_code());
        ASSERT(kEntry.pDataPointer == &Param.rValue);
#endif
        mParmStack.pop_back();
    }

public:
    // Serialize primitives
    template<typename ValType>
    std::enable_if_t<IS_SERIAL_TYPE(Primitive), IArchive&>
    operator<<(TSerialParameter<ValType> rParam)
    {
        PushParameter(rParam);

        if (InternalStartParam(rParam))
        {
            SerializePrimitive(rParam.rValue, rParam.HintFlags);
            InternalEndParam(rParam);
        }
        else if (IsReader())
        {
            InitParameterToDefault(rParam);
        }

        PopParameter(rParam);
        return *this;
    }

    template<typename ValType>
    std::enable_if_t<IS_SERIAL_TYPE(Primitive) && !IS_ABSTRACT, IArchive&>
    operator<<(TSerialParameter<ValType*> rParam)
    {
        ASSERT((mArchiveFlags & AF_Writer) == 0 || rParam.rValue != nullptr);
        PushParameter(rParam);

        if (InternalStartParam(rParam))
        {
            // Support for old versions of archives that serialize types on non-abstract polymorphic pointers
            if (ArchiveVersion() < eArVer_Refactor && IsReader() && std::is_polymorphic_v<ValType>)
            {
                uint32_t Type;
                *this << SerialParameter("Type", Type, SH_Attribute);
            }

            if (PreSerializePointer(rParam.rValue, rParam.HintFlags))
            {
                if (rParam.rValue == nullptr && (mArchiveFlags & AF_Reader) != 0)
                    rParam.rValue = new ValType;

                if (rParam.rValue != nullptr)
                    SerializePrimitive(*rParam.rValue, rParam.HintFlags);
            }
            else if (IsReader())
            {
                rParam.rValue = nullptr;
            }

            InternalEndParam(rParam);
        }

        PopParameter(rParam);
        return *this;
    }

    // Serialize objects with global Serialize functions
    template<typename ValType>
    std::enable_if_t<IS_SERIAL_TYPE(Global), IArchive&>
    inline operator<<(TSerialParameter<ValType> rParam)
    {
        PushParameter(rParam);

        if (InternalStartParam(rParam))
        {
            Serialize(*this, rParam.rValue);
            InternalEndParam(rParam);
        }
        else if (IsReader())
        {
            InitParameterToDefault(rParam);
        }

        PopParameter(rParam);
        return *this;
    }

    template<typename ValType>
    std::enable_if_t<IS_SERIAL_TYPE(Global) && !IS_ABSTRACT, IArchive&>
    operator<<(TSerialParameter<ValType*> rParam)
    {
        ASSERT(!IsWriter() || rParam.rValue != nullptr);
        PushParameter(rParam);

        if (InternalStartParam(rParam))
        {
            // Support for old versions of archives that serialize types on non-abstract polymorphic pointers
            if (ArchiveVersion() < eArVer_Refactor && IsReader() && std::is_polymorphic_v<ValType>)
            {
                uint32_t Type;
                *this << SerialParameter("Type", Type, SH_Attribute);
            }

            if (PreSerializePointer(rParam.rValue, rParam.HintFlags, rParam.HintFlags))
            {
                if (rParam.rValue == nullptr && IsReader())
                    rParam.rValue = new ValType;

                if (rParam.rValue != nullptr)
                    Serialize(*this, *rParam.rValue);
            }
            else if (IsReader())
            {
                rParam.rValue = nullptr;
            }

            InternalEndParam(rParam);
        }

        PopParameter(rParam);
        return *this;
    }

    // Serialize objects with Serialize methods
    template<typename ValType>
    std::enable_if_t<IS_SERIAL_TYPE(Member), IArchive&>
    operator<<(TSerialParameter<ValType> rParam)
    {
        PushParameter(rParam);

        if (InternalStartParam(rParam))
        {
            rParam.rValue.Serialize(*this);
            InternalEndParam(rParam);
        }
        else if (IsReader())
        {
            InitParameterToDefault(rParam);
        }

        PopParameter(rParam);
        return *this;
    }

    template<typename ValType>
    std::enable_if_t<IS_SERIAL_TYPE(Member) && !IS_ABSTRACT, IArchive&>
    operator<<(TSerialParameter<ValType*> rParam)
    {
        PushParameter(rParam);

        if (InternalStartParam(rParam))
        {
            // Support for old versions of archives that serialize types on non-abstract polymorphic pointers
            if (ArchiveVersion() < eArVer_Refactor && IsReader() && std::is_polymorphic_v<ValType>)
            {
                uint32_t Type;
                *this << SerialParameter("Type", Type, SH_Attribute);
            }

            if (PreSerializePointer((void*&) rParam.rValue, rParam.HintFlags))
            {
                if (rParam.rValue == nullptr && IsReader())
                    rParam.rValue = new ValType;

                if (rParam.rValue != nullptr)
                    rParam.rValue->Serialize(*this);
            }
            else if (IsReader())
            {
                rParam.rValue = nullptr;
            }

            InternalEndParam(rParam);
        }

        PopParameter(rParam);
        return *this;
    }

    // Serialize polymorphic objects
    template<typename ValType, typename ObjectType = decltype(std::declval<ValType>().Type())>
    std::enable_if_t<IS_SERIAL_TYPE(Member) && IS_ABSTRACT, IArchive&>
    operator<<(TSerialParameter<ValType*> rParam)
    {
        PushParameter(rParam);

        if (InternalStartParam(rParam))
        {
            if (PreSerializePointer( (void*&) rParam.rValue, rParam.HintFlags ))
            {
                // Non-reader archives cannot instantiate the class. It must exist already.
                if (IsWriter())
                {
                    ObjectType Type = rParam.rValue->Type();
                    *this << SerialParameter("Type", Type, SH_Attribute);
                }
                else
                {
                    // NOTE: If you crash here, it likely means that the pointer was initialized to a garbage value.
                    // It is legal to serialize a pointer that already exists, so you still need to initialize it.
                    ObjectType Type = (rParam.rValue ? rParam.rValue->Type() : ObjectType());
                    ObjectType TypeCopy = Type;
                    *this << SerialParameter("Type", Type, SH_Attribute);

                    if (IsReader() && rParam.rValue == nullptr)
                    {
                        rParam.rValue = (ValType*) InstantiateAbstractObject(rParam, Type);
                    }
                    else if (rParam.rValue != nullptr)
                    {
                        // Make sure the type is what we are expecting
                        ASSERT(Type == TypeCopy);
                    }
                }

                // At this point, the object should exist and is ready for serializing.
                if (rParam.rValue)
                {
                    rParam.rValue->Serialize(*this);
                }
            }
            else if (IsReader())
            {
                rParam.rValue = nullptr;
            }

            InternalEndParam(rParam);
        }
        else
        {
            // Polymorphic types don't support default values
        }

        PopParameter(rParam);
        return *this;
    }

    // Error
    template<typename ValType>
    std::enable_if_t<IS_SERIAL_TYPE(Global) && IS_ABSTRACT, IArchive&>
    operator<<(TSerialParameter<ValType*>)
    {
        static_assert(!(IS_SERIAL_TYPE(Global) && IS_ABSTRACT),
                      "Global Serialize method for polymorphic type pointers is not supported.");
    }

    // Generate compiler errors for classes with no valid Serialize function defined
    template<typename ValType>
    std::enable_if_t<IS_SERIAL_TYPE(None), IArchive&>
    operator<<(TSerialParameter<ValType>)
    {
        static_assert(!IS_SERIAL_TYPE(None),
                      "Object being serialized has no valid Serialize method defined.");
    }

    // Interface
    virtual bool ParamBegin(const char *pkName, uint32_t Flags) = 0;
    virtual void ParamEnd() = 0;

    virtual bool PreSerializePointer(void*& Pointer, uint32_t Flags) = 0;
    virtual void SerializePrimitive(bool& rValue, uint32_t Flags) = 0;
    virtual void SerializePrimitive(char& rValue, uint32_t Flags) = 0;
    virtual void SerializePrimitive(int8_t& rValue, uint32_t Flags) = 0;
    virtual void SerializePrimitive(uint8_t& rValue, uint32_t Flags) = 0;
    virtual void SerializePrimitive(int16_t& rValue, uint32_t Flags) = 0;
    virtual void SerializePrimitive(uint16_t& rValue, uint32_t Flags) = 0;
    virtual void SerializePrimitive(int32_t& rValue, uint32_t Flags) = 0;
    virtual void SerializePrimitive(uint32_t& rValue, uint32_t Flags) = 0;
    virtual void SerializePrimitive(int64_t& rValue, uint32_t Flags) = 0;
    virtual void SerializePrimitive(uint64_t& rValue, uint32_t Flags) = 0;
    virtual void SerializePrimitive(float& rValue, uint32_t Flags) = 0;
    virtual void SerializePrimitive(double& rValue, uint32_t Flags) = 0;
    virtual void SerializePrimitive(TString& rValue, uint32_t Flags) = 0;
    virtual void SerializePrimitive(CFourCC& rValue, uint32_t Flags) = 0;
    virtual void SerializePrimitive(CAssetID& rValue, uint32_t Flags) = 0;
    virtual void SerializeBulkData(void* pData, uint32_t DataSize, uint32_t Flags) = 0;

    // Optional - serialize in an array size. By default, just stores size as an attribute property.
    virtual void SerializeArraySize(uint32_t& Value)
    {
        *this << SerialParameter("Size", Value, SH_Attribute);
    }

    // Non-virtual primitive serialization
    void SerializePrimitive(T16String& rValue, uint32_t Flags)
    {
        TString String = (IsReader() ? "" : rValue.ToUTF8());
        SerializePrimitive(String, Flags);
        if (IsWriter()) rValue = String.ToUTF16();
    }

    void SerializePrimitive(T32String& rValue, uint32_t Flags)
    {
        TString String = (IsReader() ? "" : rValue.ToUTF8());
        SerializePrimitive(String, Flags);
        if (IsWriter()) rValue = String.ToUTF32();
    }

    // Accessors
    bool IsReader() const            { return (mArchiveFlags & AF_Reader) != 0; }
    bool IsWriter() const            { return (mArchiveFlags & AF_Writer) != 0; }
    bool IsTextFormat() const        { return (mArchiveFlags & AF_Text) != 0; }
    bool IsBinaryFormat() const      { return (mArchiveFlags & AF_Binary) != 0; }
    bool CanSkipParameters() const   { return (mArchiveFlags & AF_NoSkipping) == 0; }

    uint16_t ArchiveVersion() const    { return mArchiveVersion; }
    uint16_t FileVersion() const       { return mFileVersion; }
    EGame Game() const               { return mGame; }

    void SetVersion(uint16_t ArchiveVersion, uint16_t FileVersion, EGame Game)
    {
        mArchiveVersion = ArchiveVersion;
        mFileVersion = FileVersion;
        mGame = Game;
    }

    void SetVersion(const CSerialVersion& rkVersion)
    {
        mArchiveVersion = rkVersion.ArchiveVersion();
        mFileVersion = rkVersion.FileVersion();
        mGame = rkVersion.Game();
    }

    CSerialVersion GetVersionInfo() const
    {
        return CSerialVersion(mArchiveVersion, mFileVersion, mGame);
    }

    /** Utility function for class versioning */
    uint32_t SerializeClassVersion(uint32_t CurrentVersion)
    {
        *this << SerialParameter("ClassVer", CurrentVersion, SH_Attribute | SH_Optional, (uint32_t) 0);
        return CurrentVersion;
    }
};

/** Variable template that determines if the type is a primitive */
template <typename Value>
constexpr inline bool TIsPrimitive = SerializeType<Value, IArchive> == SerialType::Primitive;

// Default enum serializer; can be overridden
#include "Common/EnumReflection.h"

template<typename T, typename = std::enable_if_t<std::is_enum_v<T>>>
inline void DefaultEnumSerialize(IArchive& Arc, T& Val)
{
    if (Arc.IsTextFormat())
    {
        if (Arc.IsReader())
        {
            TString ValueName;
            Arc.SerializePrimitive(ValueName, 0);
            Val = TEnumReflection<T>::ConvertStringToValue( *ValueName );
        }
        else
        {
            TString ValueName = TEnumReflection<T>::ConvertValueToString(Val);
            Arc.SerializePrimitive(ValueName, 0);
        }
    }
    else
    {
        Arc.SerializePrimitive((uint32_t&) Val, 0);
    }
}

template<typename T, typename = std::enable_if_t<std::is_enum_v<T>>>
inline void Serialize(IArchive& Arc, T& Val)
{
    DefaultEnumSerialize(Arc, Val);
}

// Container serialize methods

// std::vector
template<typename T>
inline void Serialize(IArchive& Arc, std::vector<T>& Vector)
{
    uint32_t Size = Vector.size();
    Arc.SerializeArraySize(Size);

    if (Arc.IsReader())
    {
        Vector.resize(Size);
    }

    for (uint32_t i = 0; i < Size; i++)
    {
        // SH_IgnoreName to preserve compatibility with older files that may have differently-named items
        Arc << SerialParameter("Element", Vector[i], SH_InheritHints | SH_IgnoreName);
    }
}

// overload for std::vector<uint8_t> that serializes in bulk
template<>
inline void Serialize(IArchive& Arc, std::vector<uint8_t>& Vector)
{
    // Don't use SerializeArraySize, bulk data is a special case that overloads may not handle correctly
    uint32_t Size = Vector.size();
    Arc << SerialParameter("Size", Size, SH_Attribute);

    if (Arc.IsReader())
    {
        Vector.resize(Size);
    }

    Arc.SerializeBulkData(Vector.data(), Vector.size(), 0);
}

// std::list
template<typename T>
inline void Serialize(IArchive& Arc, std::list<T>& List)
{
    uint32_t Size = List.size();
    Arc.SerializeArraySize(Size);

    if (Arc.IsReader())
    {
        List.resize(Size);
    }

    for (auto Iter = List.begin(); Iter != List.end(); Iter++)
        Arc << SerialParameter("Element", *Iter, SH_IgnoreName | SH_InheritHints);
}

// std::set
template<typename T>
inline void Serialize(IArchive& Arc, std::set<T>& Set)
{
    uint32_t Size = Set.size();
    Arc.SerializeArraySize(Size);

    if (Arc.IsReader())
    {
        for (uint32_t i = 0; i < Size; i++)
        {
            T Val;
            Arc << SerialParameter("Element", Val, SH_IgnoreName | SH_InheritHints);
            Set.insert(std::move(Val));
        }
    }
    else
    {
        for (auto Iter = Set.begin(); Iter != Set.end(); Iter++)
        {
            T Val = *Iter;
            Arc << SerialParameter("Element", Val, SH_IgnoreName | SH_InheritHints);
        }
    }
}

// std::map and std::unordered_map
template<typename KeyType, typename ValType, typename MapType>
inline void SerializeMap_Internal(IArchive& Arc, MapType& Map)
{
    uint32_t Size = Map.size();
    Arc.SerializeArraySize(Size);

    uint32_t Hints = SH_IgnoreName | SH_InheritHints;

    // Serialize the key/value as attributes if they are both primitive types.
    if (Arc.ArchiveVersion() >= IArchive::eArVer_MapAttributes && TIsPrimitive<KeyType> && TIsPrimitive<ValType>)
    {
        Hints |= SH_Attribute;
    }

    if (Arc.IsReader())
    {
        for (uint32_t i = 0; i < Size; i++)
        {
            KeyType Key;
            ValType Val;

            if (Arc.ParamBegin("Element", SH_IgnoreName | SH_InheritHints))
            {
                Arc << SerialParameter("Key", Key, Hints)
                    << SerialParameter("Value", Val, Hints);

                ASSERT(!Map.contains(Key));
                Map.insert_or_assign(std::move(Key), std::move(Val));
                Arc.ParamEnd();
            }
        }
    }
    else
    {
        for (auto Iter = Map.begin(); Iter != Map.end(); Iter++)
        {
            // Creating copies is not ideal, but necessary because parameters cannot be const.
            // Maybe this can be avoided somehow?
            KeyType Key = Iter->first;
            ValType Val = Iter->second;

            if (Arc.ParamBegin("Element", SH_IgnoreName | SH_InheritHints))
            {
                Arc << SerialParameter("Key", Key, Hints)
                    << SerialParameter("Value", Val, Hints);

                Arc.ParamEnd();
            }
        }
    }
}

template<typename KeyType, typename ValType, typename HashFunc>
inline void Serialize(IArchive& Arc, std::map<KeyType, ValType, HashFunc>& Map)
{
    SerializeMap_Internal<KeyType, ValType, std::map<KeyType, ValType, HashFunc> >(Arc, Map);
}

template<typename KeyType, typename ValType, typename HashFunc, typename CompFunc>
inline void Serialize(IArchive& Arc, std::unordered_map<KeyType, ValType, HashFunc, CompFunc>& Map)
{
    SerializeMap_Internal<KeyType, ValType, std::unordered_map<KeyType, ValType, HashFunc, CompFunc>>(Arc, Map);
}

// Smart pointer serialize methods
template<typename T>
void Serialize(IArchive& Arc, std::unique_ptr<T>& Pointer)
{
    T* pRawPtr = Pointer.get();
    Arc << SerialParameter("RawPointer", pRawPtr, SH_Proxy);

    if (Arc.IsReader())
        Pointer = std::unique_ptr<T>(pRawPtr);
}

template<typename T>
void Serialize(IArchive& Arc, std::shared_ptr<T>& Pointer)
{
    T* pRawPtr = Pointer.get();
    Arc << SerialParameter("RawPointer", pRawPtr, SH_Proxy);

    if (Arc.IsReader())
        Pointer = std::shared_ptr<T>(pRawPtr);
}

// Remove header-only macros
#undef SUPPORTS_DEFAULT_VALUES
#undef IS_ABSTRACT
#undef IS_SERIAL_TYPE
#undef IS_ARCHIVE_CONSTRUCTOR_TYPE
#undef ABSTRACT_TYPE

#endif // AXIO_IARCHIVE_H
