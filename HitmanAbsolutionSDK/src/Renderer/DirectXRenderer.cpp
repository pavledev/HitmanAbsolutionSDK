#include "Glacier/Render/ZRenderManager.h"
#include "Glacier/ZLevelManager.h"
#include "Glacier/ZGraphicsSettingsManager.h"

#include "Renderer/DirectXRenderer.h"
#include "Renderer/ImGuiRenderer.h"
#include "Global.h"
#include "Hooks.h"
#include "Mutex.h"
#include "Connection/PipeServer.h"

DirectXRenderer::DirectXRenderer()
{
	isRendererSetup = false;
	hwnd = nullptr;
	windowWidth = 0;
	windowHeight = 0;
	inputLayout = nullptr;
}

DirectXRenderer::~DirectXRenderer()
{
}

bool DirectXRenderer::Setup()
{
	if (isRendererSetup)
	{
		return true;
	}

	hwnd = GraphicsSettingsManager->GetHWND();

	ZRenderDevice* renderDevice = RenderManager->GetRenderDevice();
	ID3D11Device* device = renderDevice->GetDirect3DDevice();
	ID3D11DeviceContext* immediateContext = renderDevice->GetImmediateContext();
	RECT rect = { 0, 0, 0, 0 };

	GetClientRect(hwnd, &rect);

	windowWidth = static_cast<float>(rect.right - rect.left);
	windowHeight = static_cast<float>(rect.bottom - rect.top);

	lineBatch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>(immediateContext);
	lineEffect = std::make_unique<DirectX::BasicEffect>(device);

	void const* shaderByteCode;
	size_t byteCodeLength;

	lineEffect->SetVertexColorEnabled(true);
	lineEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

	HRESULT hr = renderDevice->GetDirect3DDevice()->CreateInputLayout(
		DirectX::VertexPositionColor::InputElements,
		DirectX::VertexPositionColor::InputElementCount,
		shaderByteCode, byteCodeLength,
		&inputLayout
	);

	if (FAILED(hr))
	{
		Logger::GetInstance().Log(Logger::Level::Error, "Unable to create device input layout.");

		return false;
	}

	constexpr const wchar_t* regularFontPath = L"assets/fonts/GenEiGothicPro-Regular.otf";

	//font = std::make_unique<DirectX::SpriteFont>(device, regularFontPath);

	spriteBatch = std::make_unique<DirectX::SpriteBatch>(immediateContext);

	D3D11_VIEWPORT viewport = { 0.0f, 0.0f, windowWidth, windowHeight, D3D11_MIN_DEPTH, D3D11_MAX_DEPTH };

	spriteBatch->SetViewport(viewport);

	lineEffect->SetWorld(world);
	lineEffect->SetView(view);
	lineEffect->SetProjection(projection);

	Logger::GetInstance().Log(Logger::Level::Info, "DirectX renderer successfully set up.");

	isRendererSetup = true;

	return true;
}

void DirectXRenderer::OnPresent(ZRenderDevice* renderDevice)
{
	if (!Setup())
	{
		Logger::GetInstance().Log(Logger::Level::Error, "Failed to set up DirectXTK renderer.");
		Cleanup();

		return;
	}

	ID3D11Device* device = renderDevice->GetDirect3DDevice();
	ID3D11DeviceContext* immediateContext = renderDevice->GetImmediateContext();
	ZHitman5* player = LevelManager->GetHitman().GetRawPointer();

	if (player)
	{
		ZHM5MainCamera* mainCamera = player->GetMainCamera();

		const SMatrix viewMatrix = mainCamera->GetViewMatrix();
		const SMatrix projectionMatrix = mainCamera->GetProjectionMatrix();

		view = *reinterpret_cast<DirectX::FXMMATRIX*>(&viewMatrix);
		projection = *reinterpret_cast<DirectX::FXMMATRIX*>(&projectionMatrix);

		viewProjection = view * projection;
		projectionViewInverse = (projection * view).Invert();

		lineEffect->SetView(view);
		lineEffect->SetProjection(projection);

		std::unique_ptr<DirectX::CommonStates> states = std::make_unique<DirectX::CommonStates>(device);

		immediateContext->OMSetBlendState(states->Opaque(), nullptr, 0xFFFFFFFF);
		immediateContext->OMSetDepthStencilState(states->DepthDefault(), 0);
		immediateContext->RSSetState(states->CullNone());

		lineEffect->Apply(immediateContext);
		immediateContext->IASetInputLayout(inputLayout);

		Render();
	}
}

void DirectXRenderer::OnResize(const SRenderDestinationDesc* pDescription)
{
	if (spriteBatch.get())
	{
		windowWidth = static_cast<float>(pDescription->m_nWidth);
		windowHeight = static_cast<float>(pDescription->m_nHeight);

		D3D11_VIEWPORT viewport = { 0.0f, 0.0f, windowWidth, windowHeight, D3D11_MIN_DEPTH, D3D11_MAX_DEPTH };

		spriteBatch->SetViewport(viewport);
	}
}

void DirectXRenderer::Cleanup()
{
	lineEffect.reset();
	lineBatch.reset();
	font.reset();
	spriteBatch.reset();
	inputLayout->Release();
}

void DirectXRenderer::Render()
{
	spriteBatch->Begin();
	lineBatch->Begin();

	//Draw

	lineBatch->End();
	spriteBatch->End();
}

void DirectXRenderer::DrawLine3D(const SVector3& from, const SVector3& to, const SVector4& fromColor, const SVector4& toColor)
{
	DirectX::VertexPositionColor from2(
		DirectX::SimpleMath::Vector3(from.x, from.y, from.z),
		DirectX::SimpleMath::Vector4(fromColor.x, fromColor.y, fromColor.z, fromColor.w)
	);

	DirectX::VertexPositionColor to2(
		DirectX::SimpleMath::Vector3(to.x, to.y, to.z),
		DirectX::SimpleMath::Vector4(toColor.x, toColor.y, toColor.z, toColor.w)
	);

	lineBatch->DrawLine(from2, to2);
}

void DirectXRenderer::DrawText2D(const ZString& text, const SVector2& pos, const SVector4& color, float rotation, float scale, TextAlignment alignment)
{
	const std::string text2(text.ToCString(), text.Length());
	const DirectX::SimpleMath::Vector2 stringSize = font->MeasureString(text2.c_str());

	DirectX::SimpleMath::Vector2 s_Origin(0.f, 0.f);

	if (alignment == TextAlignment::Center)
	{
		s_Origin.x = stringSize.x / 2.f;
	}
	else if (alignment == TextAlignment::Right)
	{
		s_Origin.x = stringSize.x;
	}

	font->DrawString(
		spriteBatch.get(),
		text2.c_str(),
		DirectX::SimpleMath::Vector2(pos.x, pos.y),
		DirectX::SimpleMath::Vector4(color.x, color.y, color.z, color.w),
		rotation,
		s_Origin,
		scale
	);
}

bool DirectXRenderer::WorldToScreen(const SVector3& worldPos, SVector2& out)
{
	const DirectX::SimpleMath::Vector4 world(worldPos.x, worldPos.y, worldPos.z, 1.f);
	const DirectX::SimpleMath::Vector4 projected = DirectX::XMVector4Transform(world, viewProjection);

	if (projected.w <= 0.000001f)
	{
		return false;
	}

	const float invertedZ = 1.f / projected.w;
	const DirectX::SimpleMath::Vector3 finalProjected(projected.x * invertedZ, projected.y * invertedZ, projected.z * invertedZ);

	out.x = (1.f + finalProjected.x) * 0.5f * windowWidth;
	out.y = (1.f - finalProjected.y) * 0.5f * windowHeight;

	return true;
}

bool DirectXRenderer::ScreenToWorld(const SVector2& screenPos, SVector3& worldPosOut, SVector3& directionOut)
{
	ZHitman5* player = LevelManager->GetHitman().GetRawPointer();

	if (!player)
	{
		return false;
	}

	ZHM5MainCamera* mainCamera = player->GetMainCamera();
	SMatrix cameraTrans = mainCamera->GetObjectToWorldMatrix();

	auto screenPos2 = DirectX::SimpleMath::Vector3((2.0f * screenPos.x) / windowWidth - 1.0f, 1.0f - (2.0f * screenPos.y) / windowHeight, 1.f);
	auto rayClip = DirectX::SimpleMath::Vector4(screenPos2.x, screenPos2.y, 0.f, 1.f);

	DirectX::SimpleMath::Vector4 rayEye = DirectX::XMVector4Transform(rayClip, projection.Invert());
	rayEye.z = -1.f;
	rayEye.w = 0.f;

	DirectX::SimpleMath::Vector4 rayWorld = DirectX::XMVector4Transform(rayEye, view.Invert());
	rayWorld.Normalize();

	worldPosOut.x = cameraTrans.Trans.x + rayWorld.x;
	worldPosOut.y = cameraTrans.Trans.y + rayWorld.y;
	worldPosOut.z = cameraTrans.Trans.z + rayWorld.z;

	directionOut.x = rayWorld.x;
	directionOut.y = rayWorld.y;
	directionOut.z = rayWorld.z;

	return true;
}

void DirectXRenderer::DrawBox3D(const SVector3& min, const SVector3& max, const SVector4& color)
{
	SVector3 corners[] = {
		SVector3(min.x, min.y, min.z),
		SVector3(min.x, max.y, min.z),
		SVector3(max.x, max.y, min.z),
		SVector3(max.x, min.y, min.z),
		SVector3(max.x, max.y, max.z),
		SVector3(min.x, max.y, max.z),
		SVector3(min.x, min.y, max.z),
		SVector3(max.x, min.y, max.z),
	};

	DrawLine3D(corners[0], corners[1], color, color);
	DrawLine3D(corners[1], corners[2], color, color);
	DrawLine3D(corners[2], corners[3], color, color);
	DrawLine3D(corners[3], corners[0], color, color);

	DrawLine3D(corners[4], corners[5], color, color);
	DrawLine3D(corners[5], corners[6], color, color);
	DrawLine3D(corners[6], corners[7], color, color);
	DrawLine3D(corners[7], corners[4], color, color);

	DrawLine3D(corners[1], corners[5], color, color);
	DrawLine3D(corners[0], corners[6], color, color);

	DrawLine3D(corners[2], corners[4], color, color);
	DrawLine3D(corners[3], corners[7], color, color);
}

SVector3 XMVecToSVec3(const DirectX::XMVECTOR& vector)
{
	return SVector3(DirectX::XMVectorGetX(vector), DirectX::XMVectorGetY(vector), DirectX::XMVectorGetZ(vector));
}

void DirectXRenderer::DrawOBB3D(const SVector3& min, const SVector3& max, const SMatrix& transform, const SVector4& color)
{
	const auto transform2 = *reinterpret_cast<DirectX::FXMMATRIX*>(&transform);

	DirectX::XMVECTOR corners[] = {
		DirectX::XMVector3Transform(DirectX::SimpleMath::Vector3(min.x, min.y, min.z), transform2),
		DirectX::XMVector3Transform(DirectX::SimpleMath::Vector3(min.x, max.y, min.z), transform2),
		DirectX::XMVector3Transform(DirectX::SimpleMath::Vector3(max.x, max.y, min.z), transform2),
		DirectX::XMVector3Transform(DirectX::SimpleMath::Vector3(max.x, min.y, min.z), transform2),
		DirectX::XMVector3Transform(DirectX::SimpleMath::Vector3(max.x, max.y, max.z), transform2),
		DirectX::XMVector3Transform(DirectX::SimpleMath::Vector3(min.x, max.y, max.z), transform2),
		DirectX::XMVector3Transform(DirectX::SimpleMath::Vector3(min.x, min.y, max.z), transform2),
		DirectX::XMVector3Transform(DirectX::SimpleMath::Vector3(max.x, min.y, max.z), transform2),
	};

	DrawLine3D(XMVecToSVec3(corners[0]), XMVecToSVec3(corners[1]), color, color);
	DrawLine3D(XMVecToSVec3(corners[1]), XMVecToSVec3(corners[2]), color, color);
	DrawLine3D(XMVecToSVec3(corners[2]), XMVecToSVec3(corners[3]), color, color);
	DrawLine3D(XMVecToSVec3(corners[3]), XMVecToSVec3(corners[0]), color, color);

	DrawLine3D(XMVecToSVec3(corners[4]), XMVecToSVec3(corners[5]), color, color);
	DrawLine3D(XMVecToSVec3(corners[5]), XMVecToSVec3(corners[6]), color, color);
	DrawLine3D(XMVecToSVec3(corners[6]), XMVecToSVec3(corners[7]), color, color);
	DrawLine3D(XMVecToSVec3(corners[7]), XMVecToSVec3(corners[4]), color, color);

	DrawLine3D(XMVecToSVec3(corners[1]), XMVecToSVec3(corners[5]), color, color);
	DrawLine3D(XMVecToSVec3(corners[0]), XMVecToSVec3(corners[6]), color, color);

	DrawLine3D(XMVecToSVec3(corners[2]), XMVecToSVec3(corners[4]), color, color);
	DrawLine3D(XMVecToSVec3(corners[3]), XMVecToSVec3(corners[7]), color, color);
}
