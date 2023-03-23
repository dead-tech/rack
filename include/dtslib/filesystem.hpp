#ifndef DTS_FILESYSTEM_HPP
#define DTS_FILESYSTEM_HPP

#include <concepts>
#include <expected>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <vector>

namespace dts {

template<typename Container>
concept BufferContainer = std::same_as<Container, std::string>
                          || std::same_as<Container, std::vector<std::string>>;

enum class FileStreamError { NoSuchFile, NonRegularFile, UnableToOpen };

template<BufferContainer Container>
[[nodiscard]] auto read_file(const std::string& file) noexcept
  -> std::expected<Container, FileStreamError> {
    const auto path = std::filesystem::path(file);

    if (!std::filesystem::exists(path)) {
        return std::unexpected(FileStreamError::NoSuchFile);
    }

    if (std::filesystem::status(path).type() != std::filesystem::file_type::regular) {
        return std::unexpected(FileStreamError::NonRegularFile);
    }

    std::ifstream file_handle(path);
    if (!file_handle) { return std::unexpected(FileStreamError::UnableToOpen); }

    if constexpr (std::same_as<Container, std::string>) {
        return std::string(static_cast<const std::stringstream&>(
                             std::stringstream() << file_handle.rdbuf()
        )
                             .str());
    } else {
        const auto               file_size = std::filesystem::file_size(path);
        std::vector<std::string> retval    = {};
        retval.reserve(file_size / sizeof(char));

        for (std::string line; std::getline(file_handle, line);) {
            retval.push_back(std::move(line));
        }
        return retval;
    }
}

template<typename ErrorType, typename Match>
concept ErrorKind = std::same_as<std::remove_cvref_t<ErrorType>, Match>;

template<typename ErrorType>
auto filesystem_error(ErrorType&& error_type) noexcept -> std::string {
    if constexpr (ErrorKind<ErrorType, FileStreamError>) {
        switch (error_type) {
            case FileStreamError::NoSuchFile: {
                return "No such file";
            }
            case FileStreamError::NonRegularFile: {
                return "Not a regular file";
            }
            case FileStreamError::UnableToOpen: {
                return "Unable to open file";
            }
        }
    }

    return "Unknown error";
}

} // namespace dts

#endif // DTS_FILESYSTEM_HPP
