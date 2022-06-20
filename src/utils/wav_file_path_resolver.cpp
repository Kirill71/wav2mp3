#ifdef WIN32
 #include <windows.h>
#else
#include <dirent.h>
#endif

#include "../include/utils/utils.h"
#include "../include/utils/wav_file_path_resolver.h"

namespace app
{
    namespace sal
    {
            std::vector<std::string> PosixWavFilePathResolver::getWavFiles(const std::string& dirPath)
            {
                std::vector<std::string> result;
#if defined(__unix__)

                std::unique_ptr<DIR, decltype(&closedir)> dir(opendir(dirPath.c_str()), closedir);
                if (!dir)
                {
                    utils::throwSystemException("Unable to open directory: " + dirPath);
                }

                dirent* dirent;
                while ((dirent = readdir(dir.get())))
                {
                    const auto& fileName = dirent->d_name;
                    if (utils::isWavFile(fileName))
                    {
                        result.emplace_back(dirPath + Separator + fileName);
                    }
                }
#endif
                return result;
            }

            std::vector<std::string> WindowsWavFilePathResolver::getWavFiles(const std::string& dirPath)
            {
                std::vector<std::string> result;
#if defined(WIN32) || defined(WIN64)
                WIN32_FIND_DATA fileInfo;
                using HandleType = std::remove_pointer_t<HANDLE>;
                const auto fullPath = dirPath + Separator + "*";
                std::unique_ptr<HandleType, decltype(&FindClose)> dir(FindFirstFile(fullPath.c_str(), &fileInfo), FindClose);
                if (dir.get() == INVALID_HANDLE_VALUE)
                {
                    utils::throwSystemException("Unable to open directory: " + dirPath);
                }
                do
                {
                     const auto& fileName = fileInfo.cFileName;
                     if (utils::isWavFile(fileName))
                     {
                        result.emplace_back(dirPath + Separator + fileName);
                     }
                }
                while(FindNextFile(dir.get(), &fileInfo));
#endif
                return result;
            }
        }
    }