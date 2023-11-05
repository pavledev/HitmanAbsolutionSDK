#pragma once

#include <Windows.h>

#include "Glacier/Math/SVector2.h"
#include "Glacier/Math/SVector3.h"
#include "Glacier/Math/SVector4.h"
#include "Glacier/Math/SMatrix.h"
#include "Glacier/ZString.h"
#include "Glacier/Render/ZRenderDevice.h"

#include "directxtk/Effects.h"
#include "directxtk/PrimitiveBatch.h"
#include "directxtk/SimpleMath.h"
#include "directxtk/SpriteBatch.h"
#include "directxtk/VertexTypes.h"
#include "directxtk/SpriteFont.h"
#include "directxtk/CommonStates.h"

enum class TextAlignment
{
	Left,
	Center,
	Right,
};

class HitmanAbsolutionSDK_API DirectXRenderer
{
public:
	DirectXRenderer();
	~DirectXRenderer();
	bool Setup();
	void OnPresent(ZRenderDevice* renderDevice);
	void OnResize(const SRenderDestinationDesc* pDescription);
	void Cleanup();
	void Render();

	void DrawLine3D(const SVector3& from, const SVector3& to, const SVector4& fromColor, const SVector4& toColor);
	void DrawText2D(const ZString& text, const SVector2& pos, const SVector4& color, float rotation = 0.f, float scale = 1.f, TextAlignment alignment = TextAlignment::Center);
	bool WorldToScreen(const SVector3& worldPos, SVector2& out);
	bool ScreenToWorld(const SVector2& screenPos, SVector3& worldPosOut, SVector3& directionOut);
	void DrawBox3D(const SVector3& min, const SVector3& max, const SVector4& color);
	void DrawOBB3D(const SVector3& min, const SVector3& max, const SMatrix& transform, const SVector4& color);

	static void CreateDDSTextureFromMemory(const void* data, const unsigned int resourceDataSize, ID3D11Resource** texture, ID3D11ShaderResourceView** textureView, float& width, float& height);
	static void LoadTextureFromFile(const char* textureFilePath, ID3D11Texture2D** texture, ID3D11ShaderResourceView** texureView, unsigned int& width, unsigned int& height);

private:
	bool isRendererSetup;

	HWND hwnd;
	float windowWidth;
	float windowHeight;

	std::unique_ptr<DirectX::BasicEffect> lineEffect;
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> lineBatch;
	ID3D11InputLayout* inputLayout;

	DirectX::SimpleMath::Matrix world;
	DirectX::SimpleMath::Matrix view;
	DirectX::SimpleMath::Matrix projection;
	DirectX::SimpleMath::Matrix viewProjection;
	DirectX::SimpleMath::Matrix projectionViewInverse;

	std::unique_ptr<DirectX::SpriteFont> font;
	std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
};
