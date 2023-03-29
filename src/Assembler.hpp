#ifndef ASSEMBLER_HPP
#define ASSEMBLER_HPP

#include "Compiler.hpp"
#include "Error.hpp"
#include "Lexer.hpp"
#include <expected>
#include <filesystem>
#include <fmt/format.h>
#include <fstream>
#include <ranges>
#include <vector>

enum class AssembleError {
    Eof = 0,
    NoSuchFileOrDirectory,
    MissingFunctionName,
    MissingFunctionParametersOrReturnType,
    NoBeginToken,
    NoEndToken,
    Max,
};

class Assembler {
  public:
    virtual ~Assembler()                                = default;
    Assembler(const Assembler& other)                   = delete;
    Assembler(Assembler&& other)                        = default;
    Assembler& operator=(const Assembler& rhs) noexcept = delete;
    Assembler& operator=(Assembler&& rhs) noexcept      = default;

    virtual std::expected<void, AssembleError> compile_to_assembly()       = 0;
    virtual void                               generate_assembly_prelude() = 0;

  protected:
    explicit Assembler(const std::string& output_filename);

    void writeln(const std::string_view str);

    std::unique_ptr<std::ofstream> m_output_file;
    std::vector<std::string>       m_strings;
};

class Assembler_x86_64 : public Assembler {
  public:
    // TODO: Add custom output file name
    [[nodiscard]] static auto compile(
      const std::shared_ptr<Compiler>& compiler,
      const std::vector<Token>&        tokens
    ) -> std::expected<void, AssembleError>;

  private:
    Assembler_x86_64(
      const std::shared_ptr<Compiler>& compiler,
      const std::vector<Token>&        tokens,
      const std::string&               output_filename
    );

    auto compile_to_assembly() -> std::expected<void, AssembleError> final;
    void generate_assembly_prelude() final;

    void generate_assembly_header();
    void generate_assembly_start_label();
    void generate_data_section();

    [[nodiscard]] auto
      span(const std::size_t start, const std::size_t end) const -> Span;

    void error(const std::string& message, const Span& span);

    [[nodiscard]] auto eof() const -> bool;
    [[nodiscard]] auto peek() const -> std::expected<Token, AssembleError>;
    [[nodiscard]] auto peek_ahead(const std::size_t offset) const
      -> std::expected<Token, AssembleError>;
    [[nodiscard]] auto next() -> std::expected<void, AssembleError>;

    [[nodiscard]] auto compile_function() -> std::expected<void, AssembleError>;
    [[nodiscard]] auto compile_function_body(const Span& begin_span)
      -> std::expected<void, AssembleError>;

    std::shared_ptr<Compiler> m_compiler;
    std::vector<Token>        m_tokens;
    std::size_t               m_cursor = 0;
};

// {fmt} - Custom Formatters
template<>
struct fmt::formatter<AssembleError> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const AssembleError& error, FormatContext& ctx) {
        static_assert(
          std::to_underlying(AssembleError::Max) == 6,
          "[INTERNAL ERROR] fmt::formatter<AssembleError> requires to handle "
          "all "
          "enum variants"
        );

        const auto enum_to_str = [](const AssembleError& error) {
            switch (error) {
                case AssembleError::Eof: {
                    return "AssembleError::Eof";
                }
                case AssembleError::NoSuchFileOrDirectory: {
                    return "AssembleError::NoSuchFileOrDirectory";
                }
                case AssembleError::MissingFunctionName: {
                    return "AssembleError::MissingFunctionName";
                }
                case AssembleError::MissingFunctionParametersOrReturnType: {
                    return "AssembleError::"
                           "MissingFunctionParametersOrReturnType";
                }
                case AssembleError::NoBeginToken: {
                    return "AssembleError::NoBeginToken";
                }
                case AssembleError::NoEndToken: {
                    return "AssembleError::NoEndToken";
                }
                default: {
                    return "Unknown Assemble Error";
                }
            }
        };

        return fmt::format_to(ctx.out(), "{}", enum_to_str(error));
    }
};

#endif // ASSEMBLER_HPP
