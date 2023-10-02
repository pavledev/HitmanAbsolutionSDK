#pragma once

#include <string_view>
#include <array>

template <typename T>
constexpr std::string_view GenerateTypeName();

template <>
constexpr std::string_view GenerateTypeName<void>()
{
    return "void";
}

using TypeNameProber = void;

template <typename T>
constexpr std::string_view GetWrappedTypeName()
{
    return __FUNCSIG__;
}

constexpr std::size_t GetWrappedTypeNamePrefixLength()
{
    return GetWrappedTypeName<TypeNameProber>().find(GenerateTypeName<TypeNameProber>());
}

constexpr std::size_t GetWrappedTypeNameSuffixLength()
{
    return GetWrappedTypeName<TypeNameProber>().length()
        - GetWrappedTypeNamePrefixLength()
        - GenerateTypeName<TypeNameProber>().length();
}

template <typename T>
constexpr std::string_view GenerateTypeName()
{
    constexpr auto wrappedName = GetWrappedTypeName<T>();
    constexpr auto prefixLength = GetWrappedTypeNamePrefixLength();
    constexpr auto suffixLength = GetWrappedTypeNameSuffixLength();
    constexpr auto typeNameLength = wrappedName.length() - prefixLength - suffixLength;

    return wrappedName.substr(prefixLength, typeNameLength);
}

template <std::size_t N>
struct GlacierTypeNameData
{
    std::array<char, N> data;
    std::size_t size;
};

template <std::size_t N>
constexpr GlacierTypeNameData<N> GenerateGlacierTypeName(std::string_view typeName) noexcept
{
    constexpr auto classPrefix = std::string_view("class ");
    constexpr auto structPrefix = std::string_view("struct ");
    constexpr auto enumPrefix = std::string_view("enum ");
    std::array<char, N> glacierTypeNameStorage = std::array<char, N>();
    std::size_t finalSize = 0;

    for (std::size_t i = 0; i < N; ++i)
    {
        if (typeName[i] == 'c' && i + classPrefix.size() < N &&
            typeName.substr(i, classPrefix.size()) == classPrefix)
        {
            i += classPrefix.size() - 1;

            continue;
        }

        if (typeName[i] == 's' && i + structPrefix.size() < N &&
            typeName.substr(i, structPrefix.size()) == structPrefix)
        {
            i += structPrefix.size() - 1;

            continue;
        }

        if (typeName[i] == 'e' && i + enumPrefix.size() < N &&
            typeName.substr(i, enumPrefix.size()) == enumPrefix)
        {
            i += enumPrefix.size() - 1;

            continue;
        }

        if (typeName[i] == ':' && i + 1 < N && typeName[i + 1] == ':')
        {
            ++i;
            glacierTypeNameStorage[finalSize++] = '.';

            continue;
        }

        glacierTypeNameStorage[finalSize++] = typeName[i];
    }

    return GlacierTypeNameData<N>{ glacierTypeNameStorage, finalSize };
}

template <typename T>
inline constexpr auto GlacierTypeNameStorage = GenerateGlacierTypeName<GenerateTypeName<T>().size()>(GenerateTypeName<T>());

template <typename T>
inline constexpr std::string_view TypeName = GenerateTypeName<T>();

template <typename T>
inline constexpr std::string_view GlacierTypeName = std::string_view(GlacierTypeNameStorage<T>.data.data(), GlacierTypeNameStorage<T>.size);

template <> inline constexpr std::string_view GlacierTypeName<signed char> = "int8";
template <> inline constexpr std::string_view GlacierTypeName<unsigned char> = "uint8";
template <> inline constexpr std::string_view GlacierTypeName<short> = "int16";
template <> inline constexpr std::string_view GlacierTypeName<unsigned short> = "uint16";
template <> inline constexpr std::string_view GlacierTypeName<int> = "int32";
template <> inline constexpr std::string_view GlacierTypeName<unsigned int> = "uint32";
template <> inline constexpr std::string_view GlacierTypeName<long> = "int64";
template <> inline constexpr std::string_view GlacierTypeName<unsigned long long> = "uint64";
template <> inline constexpr std::string_view GlacierTypeName<float> = "float32";
template <> inline constexpr std::string_view GlacierTypeName<double> = "float64";
