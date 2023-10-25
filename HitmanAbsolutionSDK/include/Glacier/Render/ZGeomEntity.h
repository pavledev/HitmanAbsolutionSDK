#pragma once

#include "ZPrimitiveContainerEntity.h"
#include "../Entity/ZPresenceState.h"
#include "../Resource/ZResourcePtr.h"

class IRenderPrimitive;
class ZEventNull;
template <typename A, class B, class C, class D, class E> class ZEvent;
template <typename T> class TRefCountPtr;
template <typename T> class TEnumerator;

class ZGeomEntity : public ZPrimitiveContainerEntity, public ZPresenceState
{
public:
	virtual ZResourcePtr GetResourcePtr() const = 0;
	virtual ZEvent<ZEventNull, ZEventNull, ZEventNull, ZEventNull, ZEventNull>& GetResourceChangedEvent() const = 0;
	virtual ZResourcePtr ChangePrimitiveResourcePtr(const ZResourcePtr& resourcePtr) = 0;
	virtual TEnumerator<TRefCountPtr<IRenderPrimitive> const> GetResourcePrimitiveEnumerator() const = 0;
	virtual TEnumerator<TRefCountPtr<IRenderPrimitive>> GetResourcePrimitiveEnumerator() = 0;
	virtual unsigned int GetResourcePrimitivesCount() const = 0;
	virtual void HandlePrimitiveChanged(const ZResourcePtr& oldValue) = 0;

private:
	ZResourcePtr m_pPrimitive;
	ZEvent<ZEventNull, ZEventNull, ZEventNull, ZEventNull, ZEventNull>* m_pResourceChanged;
	unsigned short m_nRoomID;
};
