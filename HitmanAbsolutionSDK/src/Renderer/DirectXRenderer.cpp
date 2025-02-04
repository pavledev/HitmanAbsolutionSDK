#include <directxtk/DDSTextureLoader.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Glacier/Render/ZRenderManager.h"
#include "Glacier/ZLevelManager.h"
#include "Glacier/ZGraphicsSettingsManager.h"
#include "Glacier/Engine/ZApplicationEngineWin32.h"
#include "Glacier/UI/ZScaleformManager.h"
#include "Glacier/UI/ZHUDManager.h"

#include "Renderer/DirectXRenderer.h"
#include "Renderer/ImGuiRenderer.h"
#include "Global.h"
#include "Hooks.h"
#include "Mutex.h"
#include "Connection/PipeServer.h"
#include "SDK.h"

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

	constexpr const wchar_t* robotoRegularFontPath = L"assets/fonts/Roboto-Regular.spritefont";

	font = std::make_unique<DirectX::SpriteFont>(device, robotoRegularFontPath);
	spriteBatch = std::make_unique<DirectX::SpriteBatch>(immediateContext);

	D3D11_VIEWPORT viewport = { 0.0f, 0.0f, windowWidth, windowHeight, D3D11_MIN_DEPTH, D3D11_MAX_DEPTH };

	spriteBatch->SetViewport(viewport);

	lineEffect->SetWorld(world);
	lineEffect->SetView(view);
	lineEffect->SetProjection(projection);

	CD3D11_TEXTURE2D_DESC sceneDesc(
		DXGI_FORMAT_R8G8B8A8_UNORM, windowWidth, windowHeight,
		1, 1, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);

	device->CreateTexture2D(&sceneDesc, nullptr, &sceneTexture);
	device->CreateRenderTargetView(sceneTexture, nullptr, &sceneView);

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

	if (ScaleformManager->IsInMainMenu() || HUDManager->IsPauseMenuActive())
	{
		return;
	}

	ID3D11Device* device = renderDevice->GetDirect3DDevice();
	ID3D11DeviceContext* immediateContext = renderDevice->GetImmediateContext();
	ZHitman5* player = LevelManager->GetHitman().GetRawPointer();

	if (player)
	{
		ZCameraEntity* activeCamera = ZApplicationEngineWin32::GetInstance()->GetActiveCamera();
		const SMatrix viewMatrix = activeCamera->GetViewMatrix();
		const SMatrix projectionMatrix = activeCamera->GetProjectionMatrix();

		view = *reinterpret_cast<DirectX::FXMMATRIX*>(&viewMatrix);
		projection = *reinterpret_cast<DirectX::FXMMATRIX*>(&projectionMatrix);

		viewProjection = view * projection;
		projectionViewInverse = (projection * view).Invert();

		lineEffect->SetView(view);
		lineEffect->SetProjection(projection);

		std::unique_ptr<DirectX::CommonStates> states = std::make_unique<DirectX::CommonStates>(device);

		immediateContext->OMSetBlendState(states->AlphaBlend(), nullptr, 0xFFFFFFFF);
		immediateContext->OMSetDepthStencilState(states->DepthReadReverseZ(), 0);
		immediateContext->RSSetState(states->CullNone());

		lineEffect->Apply(immediateContext);
		immediateContext->IASetInputLayout(inputLayout);

		//immediateContext->OMSetRenderTargets(1, &sceneView, nullptr);

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

	SDK::GetInstance().OnDraw3D();

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

void DirectXRenderer::DrawQuad3D(const SVector3& vector1, const SVector4& color1, const SVector3& vector2, const SVector4& color2, const SVector3& vector3, const SVector4& color3, const SVector3& vector4, const SVector4& color4)
{
	lineBatch->DrawQuad(
		DirectX::VertexPositionColor(DirectX::SimpleMath::Vector3(vector1.x, vector1.y, vector1.z), DirectX::SimpleMath::Vector4(color1.x, color1.y, color1.z, color1.w)),
		DirectX::VertexPositionColor(DirectX::SimpleMath::Vector3(vector2.x, vector2.y, vector2.z), DirectX::SimpleMath::Vector4(color2.x, color2.y, color2.z, color2.w)),
		DirectX::VertexPositionColor(DirectX::SimpleMath::Vector3(vector3.x, vector3.y, vector3.z), DirectX::SimpleMath::Vector4(color3.x, color3.y, color3.z, color3.w)),
		DirectX::VertexPositionColor(DirectX::SimpleMath::Vector3(vector4.x, vector4.y, vector4.z), DirectX::SimpleMath::Vector4(color4.x, color4.y, color4.z, color4.w))
	);
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

	ZCameraEntity* activeCamera = ZApplicationEngineWin32::GetInstance()->GetActiveCamera();
	SMatrix cameraTrans = activeCamera->GetObjectToWorldMatrix();

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

void DirectXRenderer::CreateDDSTextureFromMemory(const void* data, const unsigned int resourceDataSize, ID3D11Resource** texture, ID3D11ShaderResourceView** textureView, float& width, float& height)
{
	ID3D11Device* device = RenderManager->GetRenderDevice()->GetDirect3DDevice();
	HRESULT result = DirectX::CreateDDSTextureFromMemory(device, static_cast<const unsigned char*>(data), resourceDataSize, texture, textureView);

	if (FAILED(result))
	{
		Logger::GetInstance().Log(Logger::Level::Error, "Failed to create DDS texture!");

		return;
	}

	ID3D11Texture2D* texture2D = nullptr;
	result = (*texture)->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&texture2D));

	if (FAILED(result))
	{
		Logger::GetInstance().Log(Logger::Level::Error, "Failed to query ID3D11Texture2D interface!");

		return;
	}

	if (texture2D)
	{
		D3D11_TEXTURE2D_DESC desc;

		texture2D->GetDesc(&desc);

		width = static_cast<float>(desc.Width);
		height = static_cast<float>(desc.Height);

		texture2D->Release();
	}
}

void DirectXRenderer::LoadTextureFromFile(const char* textureFilePath, ID3D11Texture2D** texture, ID3D11ShaderResourceView** texureView, unsigned int& width, unsigned int& height)
{
	ID3D11Device* device = RenderManager->GetRenderDevice()->GetDirect3DDevice();
	int imageWidth = 0;
	int imageHeight = 0;
	unsigned char* imageData = stbi_load(textureFilePath, &imageWidth, &imageHeight, nullptr, 4);

	if (!imageData)
	{
		Logger::GetInstance().Log(Logger::Level::Error, "Failed to load texture!");

		return;
	}

	width = static_cast<unsigned int>(imageWidth);
	height = static_cast<unsigned int>(imageHeight);

	D3D11_TEXTURE2D_DESC desc;

	ZeroMemory(&desc, sizeof(desc));

	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA subResource;
	subResource.pSysMem = imageData;
	subResource.SysMemPitch = desc.Width * 4;
	subResource.SysMemSlicePitch = 0;

	HRESULT result = device->CreateTexture2D(&desc, &subResource, texture);

	if (FAILED(result))
	{
		Logger::GetInstance().Log(Logger::Level::Error, "Failed to create texture!");

		stbi_image_free(imageData);

		return;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	ZeroMemory(&srvDesc, sizeof(srvDesc));

	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;

	result = device->CreateShaderResourceView(*texture, &srvDesc, texureView);

	if (FAILED(result))
	{
		Logger::GetInstance().Log(Logger::Level::Error, "Failed to create DDS texture!");

		stbi_image_free(imageData);

		return;
	}

	stbi_image_free(imageData);
}
