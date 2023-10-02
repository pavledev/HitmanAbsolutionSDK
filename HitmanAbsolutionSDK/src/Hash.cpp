#include <sstream>
#include <iomanip>

#include "Hash.h"
#include "Utility/StringUtility.h"

Hash::MD5Hash Hash::MD5(const std::string& message)
{
    unsigned int a0 = 0x67452301;
    unsigned int b0 = 0xefcdab89;
    unsigned int c0 = 0x98badcfe;
    unsigned int d0 = 0x10325476;

    unsigned int messageLength = message.length();
    unsigned int blockLength = ((messageLength + 8) / 64 + 1) * 64;
    unsigned char* messageBytes = new unsigned char[blockLength];

    std::memset(messageBytes, 0, blockLength);
    std::memcpy(messageBytes, message.c_str(), messageLength);

    messageBytes[message.length()] = 0x80;

    unsigned int bitLength = messageLength * 8;

    std::memcpy(messageBytes + blockLength - 8, &bitLength, sizeof(bitLength));

    for (unsigned int i = 0; i < blockLength / 64; ++i)
    {
        unsigned int message[16]{};

        for (unsigned int j = 0; j < 16; ++j)
        {
            std::memcpy(&message[j], &messageBytes[i * 64 + j * 4], 4);
        }

        unsigned int A = a0;
        unsigned int B = b0;
        unsigned int C = c0;
        unsigned int D = d0;

        for (unsigned int j = 0; j < 64; j++)
        {
            unsigned int F = 0;
            unsigned int g = 0;

            if (j <= 15)
            {
                F = (B & C) | (~B & D);
                g = j;
            }
            else if (j <= 31)
            {
                F = (D & B) | (~D & C);
                g = ((5 * j) + 1) % 16;
            }
            else if (j <= 47)
            {
                F = B ^ C ^ D;
                g = ((3 * j) + 5) % 16;
            }
            else if (j >= 48)
            {
                F = C ^ (B | ~D);
                g = (7 * j) % 16;
            }

            F += A + partsOfSines[j] + message[g];
            A = D;
            D = C;
            C = B;
            B += LeftRotate(F, shiftAmounts[j]);
        }

        a0 += A;
        b0 += B;
        c0 += C;
        d0 += D;
    }

    delete[] messageBytes;

    return MD5Hash{ a0, b0, c0, d0 };
}

constexpr unsigned int Hash::LeftRotate(unsigned int x, int c)
{
    return (x << c) | (x >> (32 - c));
}

std::string Hash::ConvertMD5ToString(const MD5Hash& md5Hash)
{
    std::stringstream stream;
    const unsigned char* bytes = reinterpret_cast<const unsigned char*>(&md5Hash.a);

    for (int i = 0; i < 16; ++i)
    {
        stream << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(bytes[i]);
    }

    return StringUtility::ToUpperCase(stream.str());
}

unsigned long long Hash::GetMD5(const std::string& message)
{
    const MD5Hash md5Hash = MD5(message);
    const unsigned int high = ((md5Hash.a >> 24) & 0x000000FF) | ((md5Hash.a >> 8) & 0x0000FF00) | ((md5Hash.a << 8) & 0x00FF0000);
    const unsigned int low = ((md5Hash.b >> 24) & 0x000000FF) | ((md5Hash.b >> 8) & 0x0000FF00) | ((md5Hash.b << 8) & 0x00FF0000) | ((md5Hash.b << 24) & 0xFF000000);

    return (static_cast<unsigned long long>(high) << 32) | low;
}
