#include "Glacier/ZCamera.h"
#include "Glacier/ZApplication.h"

#include "ViewFrustum.h"

#undef min
#undef max

void ViewFrustum::UpdateClipPlanes(const SMatrix& p_View, const SMatrix& p_Projection)
{
    ZCameraEntity* activeCamera = (*Globals::ApplicationEngineWin32)->GetActiveCamera();

    if (!activeCamera)
    {
        return;
    }

    if (m_IsDistanceCullingEnabled && m_MaxDrawDistance > 0.f)
    {
        const float fovYDeg = activeCamera->GetFovYDeg();
        const float aspectWByH = activeCamera->GetAspectWByH();
        const float nearZ = activeCamera->GetNearZ();

        if (fovYDeg != m_FovYDeg || aspectWByH != m_AspectWByH || nearZ != m_NearZ || m_FarZ != m_MaxDrawDistance)
        {
            m_FovYDeg = fovYDeg;
            m_AspectWByH = aspectWByH;
            m_NearZ = nearZ;
            m_FarZ = m_MaxDrawDistance;

            m_ClipPlaneProjectionMatrix = MatrixPerspectiveFovRH(m_FovYDeg, m_AspectWByH, m_NearZ, m_MaxDrawDistance);
        }
    }
    else
    {
        m_FarZ = -1.f;
        m_ClipPlaneProjectionMatrix = p_Projection;
    }

    const DirectX::FXMMATRIX view = *reinterpret_cast<DirectX::FXMMATRIX*>(&p_View);
    const DirectX::FXMMATRIX projection = *reinterpret_cast<DirectX::FXMMATRIX*>(&m_ClipPlaneProjectionMatrix);

    MatrixCreateClipPlanesNormalized(m_Planes, view * projection);
}

SMatrix ViewFrustum::MatrixPerspectiveFovRH(float p_FovYDeg, float p_AspectWByH, float p_NearZ, float p_FarZ)
{
    const float fovYRad = DirectX::XMConvertToRadians(p_FovYDeg);
    const float height = std::tan(fovYRad * 0.5f) * (p_NearZ * 2.f);
    const float width = height * p_AspectWByH;

    return MatrixPerspectiveRH(width, height, p_NearZ, p_FarZ);
}

SMatrix ViewFrustum::MatrixPerspectiveRH(float p_Width, float p_Height, float p_NearZ, float p_FarZ)
{
    return SMatrix(
        { (p_NearZ * 2.f) / p_Width, 0.f, 0.f, 0.f }, { 0.f, (p_NearZ * 2.f) / p_Height, 0.f, 0.f }, { 0.f, 0.f, -p_FarZ / (p_FarZ - p_NearZ), -1.f },
        { 0.f, 0.f, -(p_FarZ * p_NearZ) / (p_FarZ - p_NearZ), 0.f }
    );
}

void ViewFrustum::MatrixCreateClipPlanes(float4* p_Planes, const SMatrix& p_ViewProjection)
{
    const SMatrix viewProjection = p_ViewProjection.Transposed();

    static const float4 planeFront = { 0.f, 0.f, -1.f, 0.f };
    static const float4 planeBack = { 0.f, 0.f, 1.f, -1.f };
    static const float4 planeLeft = { -1.f, 0.f, 0.f, -1.f };
    static const float4 planeRight = { 1.f, 0.f, 0.f, -1.f };
    static const float4 planeBottom = { 0.f, -1.f, 0.f, -1.f };
    static const float4 planeTop = { 0.f, 1.f, 0.f, -1.f };

    p_Planes[0] = viewProjection.WVectorTransformH(planeFront);
    p_Planes[1] = viewProjection.WVectorTransformH(planeBack);
    p_Planes[2] = viewProjection.WVectorTransformH(planeLeft);
    p_Planes[3] = viewProjection.WVectorTransformH(planeRight);
    p_Planes[4] = viewProjection.WVectorTransformH(planeBottom);
    p_Planes[5] = viewProjection.WVectorTransformH(planeTop);
}

void ViewFrustum::MatrixCreateClipPlanesNormalized(float4* p_Planes, const SMatrix& p_ViewProjection)
{
    MatrixCreateClipPlanes(p_Planes, p_ViewProjection);

    for (int i = 0; i < 6; ++i)
    {
        p_Planes[i].Normalize();
    }
}

bool ViewFrustum::ContainsPoint(const SVector3& p_Point) const
{
    return CheckPointInsidePlanes(p_Point) != ContainmentType::FullyOutside;
}

bool ViewFrustum::ContainsAABB(const AABB& p_AABB) const
{
    return CheckAABBInsidePlanes(p_AABB) != ContainmentType::FullyOutside;
}

bool ViewFrustum::ContainsOBB(const SMatrix& p_Transform, const float4& p_Center, const float4& p_HalfSize) const
{
    return CheckOBBInsidePlanes(p_Transform, p_Center, p_HalfSize) != ContainmentType::FullyOutside;
}

bool ViewFrustum::ContainsSphere(const SVector3& p_Center, float p_Radius) const
{
    return CheckSphereInsidePlanes(p_Center, p_Radius) != ContainmentType::FullyOutside;
}

void ViewFrustum::SetDistanceCullingEnabled(bool p_Enabled)
{
    m_IsDistanceCullingEnabled = p_Enabled;
}

bool ViewFrustum::IsDistanceCullingEnabled() const
{
    return m_IsDistanceCullingEnabled;
}

void ViewFrustum::SetMaxDrawDistance(float p_MaxDrawDistance)
{
    m_MaxDrawDistance = p_MaxDrawDistance;
}

float ViewFrustum::GetMaxDrawDistance() const
{
    return m_MaxDrawDistance;
}

ViewFrustum::ContainmentType ViewFrustum::CheckPointInsidePlanes(const SVector3& p_Point) const
{
    constexpr float epsilon = 1.f / 4096.f;

    for (const auto& plane : m_Planes)
    {
        const SVector3 normal = plane;

        const float distance = normal.x * p_Point.x + normal.y * p_Point.y + normal.z * p_Point.z + plane.w;

        if (distance > epsilon)
        {
            return ContainmentType::FullyOutside;
        }
    }

    return ContainmentType::FullyInside;
}

ViewFrustum::ContainmentType ViewFrustum::CheckAABBInsidePlanes(const AABB& p_AABB) const
{
    const SVector3 center = (p_AABB.m_Min + p_AABB.m_Max) * 0.5f;
    const SVector3 halfSize = (p_AABB.m_Max - p_AABB.m_Min) * 0.5f;

    bool isPartiallyInside = false;

    constexpr float epsilon = 1.f / 4096.f;

    for (const auto& plane : m_Planes)
    {
        const SVector3 normal = plane;

        const float distance = normal.x * center.x + normal.y * center.y + normal.z * center.z + plane.w;

        const float radius = std::fabs(normal.x) * halfSize.x + std::fabs(normal.y) * halfSize.y + std::fabs(normal.z) * halfSize.z;

        if (distance - radius > epsilon)
        {
            return ContainmentType::FullyOutside;
        }

        if (distance + radius > 0.f)
        {
            isPartiallyInside = true;
        }
    }

    return isPartiallyInside ? ContainmentType::PartiallyInside : ContainmentType::FullyInside;
}

ViewFrustum::ContainmentType ViewFrustum::CheckOBBInsidePlanes(const SMatrix& p_Transform, const float4& p_Center, const float4& p_HalfSize) const
{
    const SVector3 axisX = p_Transform.XAxis;
    const SVector3 axisY = p_Transform.YAxis;
    const SVector3 axisZ = p_Transform.ZAxis;

    bool isPartiallyInside = false;

    constexpr float epsilon = 1.f / 4096.f;

    for (const auto& plane : m_Planes)
    {
        const SVector3 normal = plane;

        const float distance = normal.x * p_Center.x + normal.y * p_Center.y + normal.z * p_Center.z + plane.w;

        const float radius =
            std::fabs(normal.Dot(axisX)) * p_HalfSize.x + std::fabs(normal.Dot(axisY)) * p_HalfSize.y + std::fabs(normal.Dot(axisZ)) * p_HalfSize.z;

        if (distance - radius > epsilon)
        {
            return ContainmentType::FullyOutside;
        }

        if (distance + radius > 0.f)
        {
            isPartiallyInside = true;
        }
    }

    return isPartiallyInside ? ContainmentType::PartiallyInside : ContainmentType::FullyInside;
}

ViewFrustum::ContainmentType ViewFrustum::CheckSphereInsidePlanes(const SVector3& p_Center, float p_Radius) const
{
    bool isPartiallyInside = false;

    for (const auto& plane : m_Planes)
    {
        const SVector3 normal = plane;

        const float distance = normal.x * p_Center.x + normal.y * p_Center.y + normal.z * p_Center.z + plane.w;

        constexpr float epsilon = 1.f / 4096.f;

        if (distance - p_Radius > epsilon)
        {
            return ContainmentType::FullyOutside;
        }

        if (distance + p_Radius > 0.f)
        {
            isPartiallyInside = true;
        }
    }

    return isPartiallyInside ? ContainmentType::PartiallyInside : ContainmentType::FullyInside;
}
