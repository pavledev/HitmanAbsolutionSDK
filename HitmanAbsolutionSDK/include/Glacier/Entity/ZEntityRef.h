#pragma once

#include "ZEntityType.h"
#include "../TypeInfo/ZTypeRegistry.h"
#include "EPropertyInfoFlags.h"
#include "ZVariant.h"
#include "../TypeInfo/IType.h"
#include "ZVariantRef.h"

#include <Common.h>
#include <Reflection.h>
#include <Global.h>

class ZEntityImpl;
class ZVariantRef;

class HitmanAbsolutionSDK_API ZEntityRef
{
public:
	ZEntityRef();
	ZEntityRef(ZEntityType** entityTypePtrPtr);
	bool operator==(const ZEntityRef& rhs) const;
	bool operator!=(const ZEntityRef& rhs) const;
	ZEntityImpl* GetEntityImpl() const;
	ZEntityType** GetEntityTypePtrPtr() const;
	void* QueryInterfacePtr(const STypeID* interfaceID) const;

	template <typename T>
	T* QueryInterfacePtr() const
	{
		if (!*TypeRegistry)
		{
			return nullptr;
		}

		const STypeID* typeID = (*TypeRegistry)->GetType(GlacierTypeName<T>);

		if (typeID)
		{
			return static_cast<T*>(QueryInterfacePtr(typeID));
		}

		return nullptr;
	}

	template <typename T>
	const bool HasInterface() const
	{
		if (!*TypeRegistry)
		{
			return false;
		}

		const STypeID* typeID = (*TypeRegistry)->GetType(GlacierTypeName<T>);

		if (typeID)
		{
			void* interfacePtr = QueryInterfacePtr(typeID);

			if (interfacePtr)
			{
				return true;
			}
		}

		return false;
	}

	const bool HasInterface(const std::string& typeName) const;

	TArray<SPropertyData>* GetProperties();
	ZVariant GetProperty(const ZString& propertyName);
	ZVariant GetProperty(const unsigned int propertyID);
	bool SetProperty(const unsigned int propertyID, const ZVariantRef& value, bool invokeChangeHandlers = true);
	bool SetProperty(const ZString& propertyName, const ZVariantRef& value, bool invokeChangeHandlers = true);

	template <typename T>
	bool SetProperty(const unsigned int propertyID, const T& value, bool invokeChangeHandlers = true)
	{
		ZVariant variant;

		variant.Set(value);

		return SetPropertyValue(m_pEntityTypePtrPtr, propertyID, variant, invokeChangeHandlers);
	}

	template <typename T>
	bool SetProperty(const ZString& propertyName, const T& value, bool invokeChangeHandlers = true)
	{
		return SetProperty(Hash::Crc32(propertyName.ToCString(), propertyName.Length()), value, invokeChangeHandlers);
	}

	template <typename T>
	bool SetProperty(unsigned int propertyID, const ZVariant& value, bool invokeChangeHandlers = true)
	{
		return SetPropertyValue(m_pEntityTypePtrPtr, propertyID, value, invokeChangeHandlers);
	}

	template <typename T>
	bool SetProperty(const ZString& propertyName, const ZVariant& value, bool invokeChangeHandlers = true)
	{
		return SetProperty(Hash::Crc32(propertyName.ToCString(), propertyName.Length()), value, invokeChangeHandlers);
	}

	template <typename T>
	bool SetProperty(unsigned int propertyID, const ZVariantRef& value, bool invokeChangeHandlers = true)
	{
		return SetPropertyValue(m_pEntityTypePtrPtr, propertyID, value, invokeChangeHandlers);
	}

	template <typename T>
	bool SetProperty(const ZString& propertyName, const ZVariantRef& value, bool invokeChangeHandlers = true)
	{
		return SetProperty(Hash::Crc32(propertyName.ToCString(), propertyName.Length()), value, invokeChangeHandlers);
	}

	void SignalInputPin(const ZString& pinName, const ZVariantRef& data = ZVariantRef()) const;
	void SignalInputPin(unsigned int pinID, const ZVariantRef& data = ZVariantRef()) const;
	void SignalOutputPin(const ZString& pinName, const ZVariantRef& data = ZVariantRef()) const;
	void SignalOutputPin(unsigned int pinID, const ZVariantRef& data = ZVariantRef()) const;

	TArray<SPinData>* GetInputPins();
	TArray<SPinData>* GetOutputPins();

private:
	ZEntityType** m_pEntityTypePtrPtr;
};
