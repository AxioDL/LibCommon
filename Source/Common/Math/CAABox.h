#ifndef CAABOX_H
#define CAABOX_H

#include "CVector3f.h"
#include "Common/FileIO/IInputStream.h"
#include "Common/FileIO/IOutputStream.h"
#include "Common/Serialization/IArchive.h"
#include <utility>

class CRay;
class CRayIntersection;

class CAABox
{
    CVector3f mMin{CVector3f::Infinite()};
    CVector3f mMax{-CVector3f::Infinite()};

public:
    constexpr CAABox() = default;
    constexpr CAABox(const CVector3f& min, const CVector3f& max) : mMin{min}, mMax{max} {}
    explicit CAABox(IInputStream& rInput);

    void Serialize(IArchive& rArc);
    void Write(IOutputStream& rOutput);

    [[nodiscard]] constexpr CVector3f Center() const {
        return mMax - ((mMax - mMin) * 0.5f);
    }
    [[nodiscard]] constexpr CVector3f Size() const {
        return mMax - mMin;
    }
    [[nodiscard]] constexpr CVector3f Min() const {
        return mMin;
    }
    [[nodiscard]] constexpr CVector3f Max() const {
        return mMax;
    }
    constexpr void SetMin(const CVector3f& min) {
        mMin = min;
    }
    constexpr void SetMax(const CVector3f& max) {
        mMax = max;
    }

    [[nodiscard]] bool IsNull() const {
        return Size() == CVector3f::Zero();
    }
    [[nodiscard]] bool IsInfinite() const {
        return Size() == CVector3f::Infinite();
    }

    void ExpandBounds(const CVector3f& rkVtx);
    void ExpandBounds(const CAABox& rkAABox);
    void ExpandBy(const CVector3f& rkAmount);
    [[nodiscard]] CAABox Transformed(const CTransform4f& rkTransform) const;

    [[nodiscard]] bool IsPointInBox(const CVector3f& rkPoint) const;
    [[nodiscard]] CVector3f ClosestPointAlongVector(const CVector3f& rkDir) const;
    [[nodiscard]] CVector3f FurthestPointAlongVector(const CVector3f& rkDir) const;

    // Intersection Tests
    [[nodiscard]] bool IntersectsAABox(const CAABox& rkAABox);
    [[nodiscard]] bool IntersectsSphere(const CVector3f& rkSphereCenter, float SphereRadius);
    [[nodiscard]] std::pair<bool,float> IntersectsRay(const CRay& rkRay) const;

    // Operators
    [[nodiscard]] constexpr CAABox operator+(const CVector3f& translate) const {
        return {mMin + translate, mMax + translate};
    }
    void operator+=(const CVector3f& translate) {
        *this = *this + translate;
    }
    [[nodiscard]] constexpr CAABox operator*(float scalar) const {
        return {mMin * scalar, mMax * scalar};
    }
    constexpr void operator*=(float scalar) {
        *this = *this * scalar;
    }
    [[nodiscard]] constexpr bool operator==(const CAABox& other) const {
        return mMin == other.mMin && mMax == other.mMax;
    }
    [[nodiscard]] constexpr bool operator!=(const CAABox& other) const {
        return !operator==(other);
    }

    // min set to float maximum, max set to float minimum; ideal for creating an AABox from scratch
    static constexpr CAABox Infinite() {
        return {{FLT_MAX}, {-FLT_MAX}};
    }

    // A 1x1x1 bounding box
    static constexpr CAABox One() {
        return {{-0.5f}, {0.5}};
    }

    // A 0x0x0 bounding box
    static constexpr CAABox Zero() {
        return {{0.0f}, {0.0f}};
    }
};

#endif // CAABOX_H
