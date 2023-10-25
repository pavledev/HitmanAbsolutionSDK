#pragma once

#include "ZRenderableEntity.h"
#include "../Templates/TArray.h"
#include "../Templates/TList.h"

class IRenderMaterialEntity;
class IRenderPrimitive;
template <typename T> class TRefCountPtr;
template <typename T> class TEnumerator;
enum ERenderGlowTypes;

class ZPrimitiveContainerEntity : public ZRenderableEntity
{
public:
	virtual void ClearPrimitives() = 0;
	virtual void ClearDecalPrimitives() = 0;
	virtual bool IsPrimitiveAdded(const TRefCountPtr<IRenderPrimitive>& pPrimitive) const = 0;
	virtual void AddPrimitive(const TRefCountPtr<IRenderPrimitive>& pPrimitive) = 0;
	virtual void AddPrimitiveNoBoundsUpdate(const TRefCountPtr<IRenderPrimitive>& pPrimitive) = 0;
	virtual void RemovePrimitive(const TRefCountPtr<IRenderPrimitive>& pPrimitive) = 0;
	virtual void RemovePrimitiveNoBoundsUpdate(const TRefCountPtr<IRenderPrimitive>& pPrimitive) = 0;
	virtual void PrimitiveUpdated(const TRefCountPtr<IRenderPrimitive>& pPrimitive, bool bUpdateBounds) = 0;
	virtual TEnumerator<TRefCountPtr<IRenderPrimitive> const> GetPrimitiveEnumerator() const = 0;
	virtual TEnumerator<TRefCountPtr<IRenderPrimitive>> GetPrimitiveEnumerator() = 0;
	virtual unsigned int GetPrimitivesCount() const = 0;
	virtual unsigned int GetPrimitiveChangeCounter() const = 0;
	virtual bool GetCastShadows() const = 0;
	virtual float GetLODScale() const = 0;
	virtual float GetLODOffset() const = 0;
	virtual void SetCastShadows(bool bCastShadows) = 0;
	virtual void SetLODScale(float fLODScale) = 0;
	virtual void SetLODOffset(float fLODOffset) = 0;
	virtual void AttachMaterialModifier(IRenderMaterialEntity* pMaterialModifier) = 0;
	virtual void DetachMaterialModifier(IRenderMaterialEntity* pMaterialModifier) = 0;
	virtual TEnumerator<IRenderMaterialEntity* const> GetMaterialModifiers() const = 0;
	virtual TEnumerator<IRenderMaterialEntity*> GetMaterialModifiers() = 0;
	virtual void ClearMaterialModifiers() = 0;
	virtual void SetOverrideMaterial(IRenderMaterialEntity* pMaterial) = 0;
	virtual void ForceUpdateOverrideMaterial(IRenderMaterialEntity* pMaterial) = 0;
	virtual IRenderMaterialEntity* GetOverrideMaterial() const = 0;
	virtual const ERenderGlowTypes GetGlowType() const = 0;
	virtual void SetGlowType(const ERenderGlowTypes eGlowType) = 0;

private:
	unsigned short m_nRuntimeDecal;
	TArray<TRefCountPtr<IRenderPrimitive>> m_Primitives;
	TList<IRenderMaterialEntity*> m_pMaterialModifiers;
	IRenderMaterialEntity* m_pOverrideMaterial;
	unsigned int m_nPrimitiveChangeCounter;
	float m_fLODScale;
	float m_fLODOffset;
	ERenderGlowTypes m_eGlowType;
	bool m_bCastShadows;
	bool m_bPCCastShadows;
	bool m_bIsPlayer;
};
