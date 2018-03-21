#ifdef _MSC_VER
#include <filesystem>
namespace filesystem = std::experimental::filesystem;
#else
#if __has_include(<filesystem>)
#include <filesystem>
namespace filesystem = std::filesystem;
#elif __has_include(<experimental/filesystem>)
#include <experimental/filesystem>
namespace filesystem = std::experimental::filesystem;
#elif __has_include(<boost/filesystem.hpp>)
#include <boost/filesystem.hpp>
namespace filesystem = boost::filesystem;
#endif
#endif
#include <iostream>
#include <string>
#include <set>

int main(int argc, char** argv) {
    if (argc < 4) {
        std::cout << "Argument Error! Expecting 3 Got " << argc - 1 << std::endl;
        return -1;
    }
    try {
        const filesystem::path build{std::string(argv[1])};
        const filesystem::path src{std::string(argv[2])};
        const std::string name(argv[3]);
        const std::string asset{"Assets"};
        std::cout << "- From: " << src << std::endl << "- To: " << build << std::endl;
        if (const auto srcAsset = src / asset; filesystem::exists(srcAsset)) {
            const filesystem::path dest = build / asset / name;
            if (filesystem::exists(dest)) {
                std::set<filesystem::path> desired{};
                for (auto &&x : filesystem::directory_iterator(srcAsset)) {
                    desired.insert(x.path().filename());
                }
                for (auto &&x : filesystem::directory_iterator(dest)) {
                    if (desired.find(x.path().filename()) == desired.end()) {
                        filesystem::remove_all(x.path());
                        std::cout << "- Removing Out Dated: " << filesystem::absolute(x.path()) << std::endl;
                    }
                }
            } else {
                std::cout << "- Creating Dir: " << filesystem::absolute(build / asset) << std::endl;
                filesystem::create_directory(build / asset);
            }
            std::cout << "- Updating: " << filesystem::absolute(dest) << std::endl;
            filesystem::copy(src / asset, dest,
                             filesystem::copy_options::recursive | filesystem::copy_options::update_existing);
            std::cout << "- Copying Done!" << std::endl;
        }
    }
    catch (std::exception& e) {
        std::cout << e.what() << std::endl;
        return -1;
    }
    return 0;
}
