#include <iostream>

#include "include/utils/thread_pool.h"
#include "include/utils/wav_file_path_resolver.h"
#include "include/import/wav_reader.h"
#include "include/export/mp3_writer.h"

void usage()
{
    std::cout <<std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "wav2mp3 <path to dir with wav files>" << std::endl;
    std::cout <<std::endl;
}

int main(int argc, char* argv[])
{
    using namespace app::utils;
    using namespace app::sal;

    if (argc != 2)
    {
        usage();
        std::cerr << "wav2mp3 error: " << argc - 1
                  << " arguments provided but 1 required" << std::endl;
        return EXIT_SUCCESS;
    }
    try {
        std::unique_ptr<IWavFilePathResolver> pathResolver = nullptr;

#if defined(WIN32) || defined(WIN64)
        pathResolver = std::make_unique<WindowsWavFilePathResolver>();
#else
        pathResolver = std::make_unique<PosixWavFilePathResolver>();
#endif
        WavFilePathResolver wavResolver(std::move(pathResolver));

        const auto &dirPath = argv[1];
        const auto &wavFiles = wavResolver.getWavFiles(dirPath);
          if (wavFiles.empty())
          {
              throwSystemException("Wav files are absent in dir: " + std::string(dirPath));
          }
        Mutex logMutex;
        ThreadPool pool;
        for (const auto &wavFile: wavFiles) 
        {
            pool.addTask(
                    [&](const std::string &filePath) {
                        app::importing::WavReader wavReader;
                        app::exporting::Mp3Writer mp3Writer;
                        auto audioStorage = wavReader.read(wavFile);
                        mp3Writer.write(std::move(audioStorage));
                        MutexGuard guard(logMutex);
                        std::cout << "File has been processed: " << filePath << std::endl;
                    }, wavFile);
        }
    } catch(std::exception& ex)
    {
        std::cerr << "Internal error has been occurred: "  << ex.what();
        return  EXIT_FAILURE;
    }
    catch(...)
    {
        std::cerr << "Internal error has been occurred.";
        return  EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}