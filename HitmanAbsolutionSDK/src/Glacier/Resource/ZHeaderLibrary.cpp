#include <Glacier/Resource/ZHeaderLibrary.h>
#include <Glacier/Resource/ZResourceManager.h>

#include <Global.h>

ZResourcePtr ZHeaderLibrary::GetSourceResource() const
{
	const ZRuntimeResourceID sourceRuntimeResourceID = m_LibraryLoadOrder[0]->GetSourceRuntimeResourceID();

	return ResourceManager->GetResourcePtr(sourceRuntimeResourceID, 0);
}
