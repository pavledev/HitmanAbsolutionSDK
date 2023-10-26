#include <string>
#include <format>

#include <Glacier/Engine/ZIniFile.h>

#include <Global.h>
#include <Function.h>

bool ZIniFile::LoadIniFileContent(const ZFilePath& plainTextIniFilePath, TArray<unsigned char>& aBuffer, bool bPlainText)
{
    std::string plainTextFilePath = plainTextIniFilePath.ToString().ToCString();
    std::string filePathToRead;

    if (bPlainText)
    {
        filePathToRead = plainTextFilePath;
    }
    else
    {
        filePathToRead = std::format("{}.scrambled", plainTextFilePath);
    }

    FILE* file = nullptr;

    fopen_s(&file, filePathToRead.c_str(), "rb");

    if (file)
    {
        fseek(file, 0, 2);

        unsigned int fileLength = ftell(file);

        fseek(file, 0, 0);

        if (fileLength <= aBuffer.Size())
        {
            unsigned char* start = aBuffer.GetStart();

            aBuffer.SetEnd(&start[fileLength]);
        }
        else
        {
            if (fileLength > aBuffer.Capacity())
            {
                aBuffer.Reserve(fileLength);
            }

            aBuffer.SetEnd(aBuffer.GetEnd() + fileLength);
        }

        fread(aBuffer.GetStart(), 1, fileLength, file);
        fclose(file);

        return true;
    }

    return false;
}

bool ZIniFile::LoadFromStringInternal(const ZString& sIniFileContent, const ZFilePath& path)
{
    return Function::CallMethodAndReturn<bool, ZIniFile*, const ZString&, const ZFilePath&>(BaseAddress + 0x2EEFF0, this, sIniFileContent, path);
}
