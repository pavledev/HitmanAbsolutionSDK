#include <Glacier/Resource/SHeaderLibrary.h>
#include <Glacier/Serializer/ZBinarySerializer.h>

void SHeaderLibrary::SerializeToMemory(ZBinarySerializer& binarySerializer)
{
    unsigned int chunksOffset = offsetof(SHeaderLibrary, chunks);
    unsigned int externalResourceIdsOffset = offsetof(SHeaderLibrary, externalResourceIds);
    unsigned int ridSourceOffset = offsetof(SHeaderLibrary, ridSource);

    binarySerializer.WriteToMemory(&ridSource, sizeof(long long), ridSourceOffset);

    chunks.SerializeToMemory(binarySerializer, chunksOffset);
    externalResourceIds.SerializeToMemory(binarySerializer, externalResourceIdsOffset);
}
