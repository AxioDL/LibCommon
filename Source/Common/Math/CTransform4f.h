#ifndef AXIO_CTRANSFORM4F_H
#define AXIO_CTRANSFORM4F_H

#include "CMatrix4f.h"
#include "CQuaternion.h"
#include "CVector3f.h"

class CVector3f;
class CVector4f;
class CQuaternion;
class IArchive;
class IInputStream;
class IOutputStream;

// Note: The default no-arg constructor initializes to the identity and does not
//       zero everything out.
class CTransform4f : public CMatrix4f
{
public:
    // Initializes to identity
    constexpr CTransform4f() : CTransform4f(1.0f) {}

    constexpr CTransform4f(const CMatrix4f& rkMtx)
    {
        for (size_t row = 0; row < 3; row++)
            m[row] = rkMtx[row];

        SetupRow4();
    }

    constexpr explicit CTransform4f(float Diagonal)
    {
        m[0][0] = Diagonal;
        m[1][1] = Diagonal;
        m[2][2] = Diagonal;
        m[3][3] = 1.f;
    }

    constexpr CTransform4f(float m00, float m01, float m02, float m03,
                           float m10, float m11, float m12, float m13,
                           float m20, float m21, float m22, float m23)
        : CMatrix4f(m00,  m01,  m02,  m03,
                    m10,  m11,  m12,  m13,
                    m20,  m21,  m22,  m23,
                    0.0f, 0.0f, 0.0f, 1.0f)
    {
    }

    CTransform4f(const CVector3f& Position, const CQuaternion& Rotation, const CVector3f& Scale);
    CTransform4f(const CVector3f& Position, const CVector3f& Rotation, const CVector3f& Scale);

    explicit CTransform4f(IInputStream& rInput);

    void Serialize(IArchive& rOut);
    void Write(IOutputStream& rOut) const;

    // Math
    void Translate(const CVector3f& Translation);
    void Translate(float XTrans, float YTrans, float ZTrans);
    void Rotate(const CQuaternion& Rotation);
    void Rotate(const CVector3f& Rotation);
    void Rotate(float XRot, float YRot, float ZRot);
    void Scale(const CVector3f& Scale);
    void Scale(float XScale, float YScale, float ZScale);
    void SetIdentity();
    void ZeroTranslation();
    [[nodiscard]] CTransform4f MultiplyIgnoreTranslation(const CTransform4f& rkMtx) const;
    [[nodiscard]] CTransform4f QuickInverse() const;
    [[nodiscard]] CTransform4f NoTranslation() const;
    [[nodiscard]] CTransform4f TranslationOnly() const;
    [[nodiscard]] CTransform4f RotationOnly() const;
    void SetTranslation(const CVector3f& kTranslation);
    void SetRotationFromAxes(const CVector3f& kX, const CVector3f& kY, const CVector3f& kZ);

    [[nodiscard]] CVector3f ExtractTranslation() const;
    [[nodiscard]] CQuaternion ExtractRotation() const;

    // Static
    [[nodiscard]] static CTransform4f TranslationMatrix(const CVector3f& Translation);
    [[nodiscard]] static CTransform4f RotationMatrix(const CQuaternion& Rotation);
    [[nodiscard]] static CTransform4f ScaleMatrix(const CVector3f& Scale);

    // Operators
    [[nodiscard]] constexpr std::array<float, 4>& operator[](int64_t index) { return m[index]; }
    [[nodiscard]] constexpr const std::array<float, 4>& operator[](int64_t index) const { return m[index]; }
    [[nodiscard]] CVector3f operator*(const CVector3f& rkVec) const;
    [[nodiscard]] CVector4f operator*(const CVector4f& rkVec) const;
    [[nodiscard]] CQuaternion operator*(const CQuaternion& rkQuat) const;
    [[nodiscard]] CTransform4f operator*(const CTransform4f& rkMtx) const;
    void operator*=(const CTransform4f& rkMtx);
    [[nodiscard]] bool operator==(const CTransform4f& rkMtx) const;
    [[nodiscard]] bool operator!=(const CTransform4f& rkMtx) const;

    // Constant
    static const CTransform4f skIdentity;
    static const CTransform4f skZero;

    // Protected Utility
protected:
    constexpr void SetupRow4()
    {
        m[3][0] = 0.f;
        m[3][1] = 0.f;
        m[3][2] = 0.f;
        m[3][3] = 1.f;
    }
};

#endif // AXIO_CTRANSFORM4F_H
