#include <Glacier/Resource/SHeaderLibraryChunk.h>
#include <Glacier/Serializer/ZBinarySerializer.h>

void SHeaderLibraryChunk::SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset)
{
    unsigned int libraryIDOffset = offset + offsetof(SHeaderLibraryChunk, sLibraryID);
    unsigned int offsetOffset = offset + offsetof(SHeaderLibraryChunk, nOffset);
    unsigned int flagsOffset = offset + offsetof(SHeaderLibraryChunk, nFlags);
    unsigned int localizationCategoryOffset = offset + offsetof(SHeaderLibraryChunk, localizationCategory);
    unsigned int languagesOffset = offset + offsetof(SHeaderLibraryChunk, sLanguages);
    unsigned int statesOffset = offset + offsetof(SHeaderLibraryChunk, states);
    unsigned int resourceHeadersOffset = offset + offsetof(SHeaderLibraryChunk, resourceHeaders);
    unsigned int ridMappingIndicesOffset = offset + offsetof(SHeaderLibraryChunk, ridMappingIndices);
    unsigned int ridMappingIDsOffset = offset + offsetof(SHeaderLibraryChunk, ridMappingIDs);

    binarySerializer.WriteToMemory(&nOffset, sizeof(int), offsetOffset);
    binarySerializer.WriteToMemory(&nFlags, sizeof(char), flagsOffset);
    binarySerializer.WriteToMemory(&localizationCategory, sizeof(int), localizationCategoryOffset);

    sLibraryID.SerializeToMemory(binarySerializer, libraryIDOffset);
    sLanguages.SerializeToMemory(binarySerializer, languagesOffset);
    states.SerializeToMemory(binarySerializer, statesOffset);
    resourceHeaders.SerializeToMemory(binarySerializer, resourceHeadersOffset);
    ridMappingIndices.SerializeToMemory(binarySerializer, ridMappingIndicesOffset);
    ridMappingIDs.SerializeToMemory(binarySerializer, ridMappingIDsOffset);
}
