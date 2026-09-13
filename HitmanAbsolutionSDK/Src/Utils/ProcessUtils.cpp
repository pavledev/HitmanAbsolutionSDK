#include "ProcessUtils.h"

#include <TlHelp32.h>
#include <unordered_set>

#include "Logging.h"

namespace util
{
    uintptr_t ProcessUtils::SearchPattern(uintptr_t p_BaseAddress, size_t p_ScanSize, const uint8_t* p_Pattern, const char* p_Mask)
    {
        const size_t patternSize = strlen(p_Mask);

        if (patternSize <= 1)
        {
            return 0;
        }

        const uintptr_t searchEnd = p_BaseAddress + p_ScanSize - patternSize;

        for (uintptr_t searchAddr = p_BaseAddress; searchAddr <= searchEnd; ++searchAddr)
        {
            const uint8_t* memoryPtr = reinterpret_cast<uint8_t*>(searchAddr);

            bool found = true;

            for (size_t i = 0; i < patternSize; ++i)
            {
                if (p_Mask[i] == '?')
                {
                    continue;
                }

                if (memoryPtr[i] != p_Pattern[i])
                {
                    found = false;
                    break;
                }
            }

            if (found)
            {
                return searchAddr;
            }
        }

        return 0;
    }

    uint32_t ProcessUtils::GetSizeOfCode(HMODULE p_Module)
    {
        PIMAGE_DOS_HEADER dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(p_Module);
        PIMAGE_NT_HEADERS ntHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<uintptr_t>(p_Module) + dosHeader->e_lfanew);

        if (!ntHeader)
        {
            return 0;
        }

        return static_cast<uint32_t>(ntHeader->OptionalHeader.SizeOfCode);
    }

    uintptr_t ProcessUtils::GetBaseOfCode(HMODULE p_Module)
    {
        PIMAGE_DOS_HEADER dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(p_Module);
        PIMAGE_NT_HEADERS ntHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<uintptr_t>(p_Module) + dosHeader->e_lfanew);

        if (!ntHeader)
        {
            return 0;
        }

        return static_cast<uintptr_t>(ntHeader->OptionalHeader.BaseOfCode);
    }

    uint32_t ProcessUtils::GetSizeOfImage(HMODULE p_Module)
    {
        PIMAGE_DOS_HEADER dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(p_Module);
        PIMAGE_NT_HEADERS ntHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<uintptr_t>(p_Module) + dosHeader->e_lfanew);

        if (!ntHeader)
        {
            return 0;
        }

        return static_cast<uint32_t>(ntHeader->OptionalHeader.SizeOfImage);
    }

    std::tuple<uintptr_t, uintptr_t> ProcessUtils::GetSectionStartAndEnd(HMODULE p_Module, const std::string& p_SectionName)
    {
        PIMAGE_DOS_HEADER dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(p_Module);
        PIMAGE_NT_HEADERS ntHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<uintptr_t>(p_Module) + dosHeader->e_lfanew);

        if (!ntHeader)
        {
            return std::make_tuple<uintptr_t, uintptr_t>(0, 0);
        }

        PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(ntHeader);

        for (int i = 0; i < ntHeader->FileHeader.NumberOfSections; ++i)
        {
            if (strcmp(reinterpret_cast<const char*>(section->Name), p_SectionName.c_str()) == 0)
            {
                uintptr_t rDataSectionStart = section->VirtualAddress;
                rDataSectionStart += reinterpret_cast<uintptr_t>(p_Module);

                uintptr_t rDataSectionEnd = rDataSectionStart + section->SizeOfRawData;

                return std::make_tuple(rDataSectionStart, rDataSectionEnd);
            }

            ++section;
        }

        return std::make_tuple<uintptr_t, uintptr_t>(0, 0);
    }

    uintptr_t ProcessUtils::GetRelativeAddr(uintptr_t p_Base, int32_t p_Offset)
    {
        uintptr_t relAddrPtr = p_Base + p_Offset;
        int32_t relAddr = *reinterpret_cast<int32_t*>(relAddrPtr);

        return relAddrPtr + relAddr + sizeof(int32_t);
    }
}
