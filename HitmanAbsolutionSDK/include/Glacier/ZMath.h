#pragma once

#include <xmmintrin.h>
#include <cmath>
#include <DirectXMath.h>

#include <directxtk/SimpleMath.h>

struct SVector2
{
    SVector2() : x(0.f), y(0.f) {}

    SVector2(float p_X, float p_Y) : x(p_X), y(p_Y) {}

    union
    {
        struct
        {
            float x;
            float y;
        };

        float v[2];
    };
};

struct float4;

struct SVector3
{
    SVector3() : x(0.f), y(0.f), z(0.f) {}

    SVector3(float p_X, float p_Y, float p_Z) : x(p_X), y(p_Y), z(p_Z) {}

    SVector3(const DirectX::XMVECTOR& p_Vector)
    {
        DirectX::XMStoreFloat3(reinterpret_cast<DirectX::XMFLOAT3*>(this), p_Vector);
    }

    SVector3(const DirectX::XMFLOAT3& p_Vector) : x(p_Vector.x), y(p_Vector.y), z(p_Vector.z) {}

    SVector3 operator+(const SVector3& p_Other)
    {
        SVector3 result;

        result.x = x + p_Other.x;
        result.y = y + p_Other.y;
        result.z = z + p_Other.z;

        return result;
    }

    SVector3 operator-(const SVector3& p_Other)
    {
        SVector3 result;

        result.x = x - p_Other.x;
        result.y = y - p_Other.y;
        result.z = z - p_Other.z;

        return result;
    }

    SVector3 operator*(float p_Value) const
    {
        return SVector3(x * p_Value, y * p_Value, z * p_Value);
    }

    const bool operator==(const SVector3& p_Other) const
    {
        return x == p_Other.x && y == p_Other.y && z == p_Other.z;
    }

    const bool operator!=(const SVector3& p_Other) const
    {
        return !(x == p_Other.x && y == p_Other.y && z == p_Other.z);
    }

    SVector3 operator/(const SVector3& p_Other) const
    {
        return SVector3(x / p_Other.x, y / p_Other.y, z / p_Other.z);
    }

    SVector3 operator/(const float p_Other) const
    {
        return SVector3(x / p_Other, y / p_Other, z / p_Other);
    }

    float Length() const
    {
        return std::sqrt(x * x + y * y + z * z);
    }

    union
    {
        struct
        {
            float x;
            float y;
            float z;
        };

        float v[3];
    };
};

struct SVector4
{
    SVector4() : x(0.f), y(0.f), z(0.f), w(0.f) {}

    SVector4(float p_X, float p_Y, float p_Z, float p_W) : x(p_X), y(p_Y), z(p_Z), w(p_W) {}

    union
    {
        struct
        {
            float x;
            float y;
            float z;
            float w;
        };

        struct
        {
            SVector3 n;
            float d;
        };

        float v[4];
    };
};

struct float4
{
    float4() : m(_mm_setzero_ps()) {}

    float4(__m128 p_Value) : m(p_Value) {}

    float4(float p_X, float p_Y, float p_Z, float p_W) : x(p_X), y(p_Y), z(p_Z), w(p_W) {}

    float4(float p_Value) : x(p_Value), y(p_Value), z(p_Value), w(p_Value) {}

    float4(const SVector2& p_Vector) : x(p_Vector.x), y(p_Vector.y), z(0.f), w(0.f) {}

    float4(const SVector3& p_Vector, float p_W = 0.f) : x(p_Vector.x), y(p_Vector.y), z(p_Vector.z), w(p_W) {}

    operator SVector3() const
    {
        return SVector3(x, y, z);
    }

    float4 operator-(const float4& p_Other) const
    {
        return _mm_sub_ps(m, p_Other.m);
    }

    float4 operator+(const float4& p_Other) const
    {
        return _mm_add_ps(m, p_Other.m);
    }

    float4 operator*(const float4& p_Other) const
    {
        return _mm_mul_ps(m, p_Other.m);
    }

    float4 operator*(float p_Value) const
    {
        return _mm_mul_ps(m, _mm_load1_ps(&p_Value));
    }

    float4& operator*=(float p_Value)
    {
        m = _mm_mul_ps(m, _mm_set1_ps(p_Value));
        return *this;
    }

    float4 operator/(const float4& p_Other) const
    {
        return _mm_div_ps(m, p_Other.m);
    }

    float4 operator/(float p_Value) const
    {
        return _mm_div_ps(m, _mm_load1_ps(&p_Value));
    }

    bool operator==(const float4& p_Other) const
    {
        return (_mm_movemask_ps(_mm_cmpeq_ps(m, p_Other.m)) == 0xF) & 1;
    }

    bool operator!=(const float4& p_Other) const
    {
        return !(p_Other == (*this));
    }

    float4& operator+=(const float4& p_Other)
    {
        m = _mm_add_ps(m, p_Other.m);
        return *this;
    }

    float4& operator-=(const float4& p_Other)
    {
        m = _mm_sub_ps(m, p_Other.m);
        return *this;
    }

    float4 operator-() const
    {
        return float4(-x, -y, -z, -w);
    }

    static float4 CrossProduct(const float4& p_Vector1, const float4& p_Vector2)
    {
        return _mm_sub_ps(
            _mm_mul_ps(
                _mm_shuffle_ps(p_Vector1.m, p_Vector1.m, _MM_SHUFFLE(3, 0, 2, 1)), _mm_shuffle_ps(p_Vector2.m, p_Vector2.m, _MM_SHUFFLE(3, 1, 0, 2))
            ),
            _mm_mul_ps(
                _mm_shuffle_ps(p_Vector1.m, p_Vector1.m, _MM_SHUFFLE(3, 1, 0, 2)), _mm_shuffle_ps(p_Vector2.m, p_Vector2.m, _MM_SHUFFLE(3, 0, 2, 1))
            )
        );
    }

    static float DotProduct(const float4& p_Vector1, const float4& p_Vector2)
    {
        return _mm_cvtss_f32(_mm_dp_ps(p_Vector1.m, p_Vector2.m, 0x71));
    }

    static float4 Dot3(const float4& p_Vector1, const float4& p_Vector2)
    {
        return _mm_dp_ps(p_Vector1.m, p_Vector2.m, 0x7F);
    }

    static float Normalize(const float4& p_Vector)
    {
        return sqrt(DotProduct(p_Vector, p_Vector));
    }

    static float Distance(const float4& p_From, const float4& p_To)
    {
        return Normalize(p_From - p_To);
    }

    float Length() const
    {
        return sqrtf(x * x + y * y + z * z + w * w);
    }

    float4 Normalized() const
    {
        const float lengthSq = DotProduct(*this, *this);

        if (lengthSq > 1e-12f)
        {
            __m128 invLength = _mm_rsqrt_ps(_mm_set1_ps(lengthSq));
            return _mm_mul_ps(m, invLength);
        }

        return _mm_setzero_ps();
    }

    void Normalize()
    {
        *this = Normalized();
    }

    union
    {
        __m128 m;

        struct
        {
            float x;
            float y;
            float z;
            float w;
        };
    };
};

struct SMatrix33
{
    SMatrix33() : XAxis(1.f, 0.f, 0.f), YAxis(0.f, 1.f, 0.f), ZAxis(0.f, 0.f, 1.f) {}

    SMatrix33(SVector3 p_XAxis, SVector3 p_YAxis, SVector3 p_ZAxis) : XAxis(p_XAxis), YAxis(p_YAxis), ZAxis(p_ZAxis) {}

    union
    {
        struct
        {
            SVector3 XAxis;
            SVector3 YAxis;
            SVector3 ZAxis;
        };

        struct
        {
            float m11;
            float m12;
            float m13;
            float m21;
            float m22;
            float m23;
            float m31;
            float m32;
            float m33;
        };

        float v[9];
        SVector3 r[3];
    };
};

struct SMatrix43
{
    SMatrix43() : Rot(SVector3(1.f, 0.f, 0.f), SVector3(0.f, 1.f, 0.f), SVector3(0.f, 0.f, 1.f)), Trans(0.f, 0.f, 0.f) {}

    union
    {
        struct
        {
            SVector3 XAxis;
            SVector3 YAxis;
            SVector3 ZAxis;
        };

        struct
        {
            SMatrix33 Rot;
            SVector3 Trans;
        };

        struct
        {
            float m11;
            float m12;
            float m13;
            float m21;
            float m22;
            float m23;
            float m31;
            float m32;
            float m33;
            float m41;
            float m42;
            float m43;
        };

        float v[12];
        SVector3 r[4];
    };
};

struct SMatrix44
{
    SMatrix44()
    {
        r[0] = SVector4(1.f, 0.f, 0.f, 0.f);
        r[1] = SVector4(0.f, 1.f, 0.f, 0.f);
        r[2] = SVector4(0.f, 0.f, 1.f, 0.f);
        r[3] = SVector4(0.f, 0.f, 0.f, 1.f);
    }

    union
    {
        struct
        {
            float m11;
            float m12;
            float m13;
            float m14;
            float m21;
            float m22;
            float m23;
            float m24;
            float m31;
            float m32;
            float m33;
            float m34;
            float m41;
            float m42;
            float m43;
            float m44;
        };

        SVector4 r[4];
        float v[16];
    };
};

class EulerAngles
{
  public:
    EulerAngles() : yaw(0.f), pitch(0.f), roll(0.f) {}

    EulerAngles(float p_Yaw, float p_Pitch, float p_Roll) : yaw(p_Yaw), pitch(p_Pitch), roll(p_Roll) {}

  public:
    float yaw;
    float pitch;
    float roll;
};

class Quat
{
  public:
    Quat() : m(0.f, 0.f, 0.f, 1.f) {}

    Quat(float4 p_Vec) : m(p_Vec) {}

    Quat(float p_X, float p_Y, float p_Z, float p_W) : m(p_X, p_Y, p_Z, p_W) {}

    EulerAngles ToEuler() const
    {
        // Adapted from DirectXTK.
        const float xx = m.x * m.x;
        const float yy = m.y * m.y;
        const float zz = m.z * m.z;

        const float m31 = 2.f * m.x * m.z + 2.f * m.y * m.w;
        const float m32 = 2.f * m.y * m.z - 2.f * m.x * m.w;
        const float m33 = 1.f - 2.f * xx - 2.f * yy;

        const float cy = sqrtf(m33 * m33 + m31 * m31);
        const float cx = atan2f(-m32, cy);
        if (cy > 16.f * FLT_EPSILON)
        {
            const float m12 = 2.f * m.x * m.y + 2.f * m.z * m.w;
            const float m22 = 1.f - 2.f * xx - 2.f * zz;

            return { cx, atan2f(m31, m33), atan2f(m12, m22) };
        }
        else
        {
            const float m11 = 1.f - 2.f * yy - 2.f * zz;
            const float m21 = 2.f * m.x * m.y - 2.f * m.z * m.w;

            return { cx, 0.f, atan2f(-m21, m11) };
        }
    }

    Quat operator*(Quat p_Other)
    {
        return Quat(
            m.w * p_Other.m.x + m.x * p_Other.m.w + m.y * p_Other.m.z - m.z * p_Other.m.y,
            m.w * p_Other.m.y - m.x * p_Other.m.z + m.y * p_Other.m.w + m.z * p_Other.m.x,
            m.w * p_Other.m.z + m.x * p_Other.m.y - m.y * p_Other.m.x + m.z * p_Other.m.w,
            m.w * p_Other.m.w - m.x * p_Other.m.x - m.y * p_Other.m.y - m.z * p_Other.m.z
        );
    }

  public:
    float4 m;
};

struct DecomposedTransform
{
    SVector3 m_Position;
    Quat m_Quaternion;
    SVector3 m_Scale;
};

struct SMatrix
{
    SMatrix() : XAxis(1.f, 0.f, 0.f, 0.f), YAxis(0.f, 1.f, 0.f, 0.f), ZAxis(0.f, 0.f, 1.f, 0.f), Trans(0.f, 0.f, 0.f, 1.f) {}

    SMatrix(float4 p_XAxis, float4 p_YAxis, float4 p_ZAxis, float4 p_Trans) : XAxis(p_XAxis), YAxis(p_YAxis), ZAxis(p_ZAxis), Trans(p_Trans) {}

    SMatrix(const SMatrix43& other)
        : XAxis(other.XAxis.x, other.XAxis.y, other.XAxis.z, 0.f)
        , YAxis(other.YAxis.x, other.YAxis.y, other.YAxis.z, 0.f)
        , ZAxis(other.ZAxis.x, other.ZAxis.y, other.ZAxis.z, 0.f)
        , Trans(other.Trans.x, other.Trans.y, other.Trans.z, 1.f)
    {}

    SMatrix(const SMatrix44& other)
        : XAxis(other.m11, other.m12, other.m13, other.m14)
        , YAxis(other.m21, other.m22, other.m23, other.m24)
        , ZAxis(other.m31, other.m32, other.m33, other.m34)
        , Trans(other.m41, other.m42, other.m43, other.m44)
    {}

    SMatrix(const DirectX::XMMATRIX& other) : XAxis(other.r[0]), YAxis(other.r[1]), ZAxis(other.r[2]), Trans(other.r[3]) {}

    SMatrix(const DirectX::SimpleMath::Matrix& other)
        : XAxis(other._11, other._12, other._13, other._14)
        , YAxis(other._21, other._22, other._23, other._24)
        , ZAxis(other._31, other._32, other._33, other._34)
        , Trans(other._41, other._42, other._43, other._44)
    {}

    const DirectX::XMMATRIX& DX() const
    {
        return *reinterpret_cast<const DirectX::XMMATRIX*>(this);
    }

    DirectX::XMMATRIX& DX()
    {
        return *reinterpret_cast<DirectX::XMMATRIX*>(this);
    }

    float4 operator*(const float4& p_Other) const
    {
        return DirectX::XMVector4Transform(p_Other.m, DX());
    }

    SMatrix operator*(const SMatrix& p_Other) const
    {
        return DirectX::XMMatrixMultiply(DX(), p_Other.DX());
    }

    SMatrix Inverse() const
    {
        return DirectX::XMMatrixInverse(nullptr, DX());
    }

    float Determinant() const
    {
        const auto determinant = DirectX::XMMatrixDeterminant(DX());
        return DirectX::XMVectorGetX(determinant);
    }

    SMatrix Transposed() const
    {
        return DirectX::XMMatrixTranspose(DX());
    }

    DecomposedTransform Decompose() const
    {
        DirectX::XMVECTOR scale;
        DirectX::XMVECTOR rotQuat;
        DirectX::XMVECTOR trans;

        DirectX::XMMatrixDecompose(&scale, &rotQuat, &trans, DX());

        return { trans, Quat(float4(rotQuat)), scale };
    }

    void ScaleTransform(const SVector3& p_Scale)
    {
        XAxis *= p_Scale.x;
        YAxis *= p_Scale.y;
        ZAxis *= p_Scale.z;
    }

    static SMatrix ScaleTransform(const SVector3& p_Scale, SMatrix& p_Transform)
    {
        return SMatrix(p_Transform.XAxis * p_Scale.x, p_Transform.YAxis * p_Scale.y, p_Transform.ZAxis * p_Scale.z, p_Transform.Trans);
    }

    SVector3 GetScale() const
    {
        return SVector3(XAxis.Length(), YAxis.Length(), ZAxis.Length());
    }

    [[nodiscard]] SMatrix43 ToMatrix43() const
    {
        SMatrix43 matrix;
        matrix.XAxis = { XAxis.x, XAxis.y, XAxis.z };
        matrix.YAxis = { YAxis.x, YAxis.y, YAxis.z };
        matrix.ZAxis = { ZAxis.x, ZAxis.y, ZAxis.z };
        matrix.Trans = { Trans.x, Trans.y, Trans.z };

        return matrix;
    }

    static SMatrix ScaleTranslate(const float4& p_Scale, const float4& p_Translate)
    {
        return SMatrix(
            { p_Scale.x, 0.f, 0.f, 0.f }, { 0.f, p_Scale.y, 0.f, 0.f }, { 0.f, 0.f, p_Scale.z, 0.f },
            { p_Translate.x, p_Translate.y, p_Translate.z, 1.f }
        );
    }

    SMatrix AffineMultiply(const SMatrix& p_Other) const
    {
        SMatrix matrix;

        matrix.XAxis = XAxis * p_Other.XAxis.x + YAxis * p_Other.XAxis.y + ZAxis * p_Other.XAxis.z;
        matrix.YAxis = XAxis * p_Other.YAxis.x + YAxis * p_Other.YAxis.y + ZAxis * p_Other.YAxis.z;
        matrix.ZAxis = XAxis * p_Other.ZAxis.x + YAxis * p_Other.ZAxis.y + ZAxis * p_Other.ZAxis.z;
        matrix.Trans = XAxis * p_Other.Trans.x + YAxis * p_Other.Trans.y + ZAxis * p_Other.Trans.z + Trans;

        return matrix;
    }

    float4 WVectorTransform(const float4& p_Vector) const
    {
        return XAxis * p_Vector.x + YAxis * p_Vector.y + ZAxis * p_Vector.z + Trans;
    }

    float4 WVectorTransformH(const float4& p_Vector) const
    {
        return XAxis * p_Vector.x + YAxis * p_Vector.y + ZAxis * p_Vector.z + Trans * p_Vector.w;
    }

    float4 WVectorRotate(const float4& p_Vector) const
    {
        return XAxis * p_Vector.x + YAxis * p_Vector.y + ZAxis * p_Vector.z;
    }

    static SMatrix RotationAxisAngle(const float4& p_Axis, const float p_Angle)
    {
        const float x = p_Axis.x;
        const float y = p_Axis.y;
        const float z = p_Axis.z;

        const float c = cosf(p_Angle);
        const float s = sinf(p_Angle);

        SMatrix result;

        result.XAxis = float4(c + (1.f - c) * x * x, (1.f - c) * y * x - s * z, (1.f - c) * z * x + s * y, 0.f);

        result.YAxis = float4((1.f - c) * x * y + s * z, c + (1.f - c) * y * y, (1.f - c) * z * y - s * x, 0.f);

        result.ZAxis = float4((1.f - c) * x * z - s * y, (1.f - c) * y * z + s * x, c + (1.f - c) * z * z, 0.f);

        result.Trans = float4(0.f, 0.f, 0.f, 1.f);

        return result;
    }

    union
    {
        float4 mat[4];
        float flt[4 * 4];

        struct
        {
            float4 XAxis;
            float4 YAxis;
            float4 ZAxis;
            float4 Trans;
        };

        struct
        {
            float4 Left;
            float4 Backward;
            float4 Up;
            float4 Pos;
        };

        struct
        {
            float m11;
            float m12;
            float m13;
            float m14;
            float m21;
            float m22;
            float m23;
            float m24;
            float m31;
            float m32;
            float m33;
            float m34;
            float m41;
            float m42;
            float m43;
            float m44;
        };
    };
};

class ZMath
{
  public:
    static float MapRange01(float p_Source, float p_SourceRangeMin, float p_SourceRangeMax)
    {
        if (p_SourceRangeMin > p_SourceRangeMax)
        {
            std::swap(p_SourceRangeMin, p_SourceRangeMax);
        }

        if (p_Source > p_SourceRangeMax)
        {
            p_Source = p_SourceRangeMax;
        }
        else if (p_Source < p_SourceRangeMin)
        {
            p_Source = p_SourceRangeMin;
        }

        return (p_Source - p_SourceRangeMin) / (p_SourceRangeMax - p_SourceRangeMin);
    }
};
