#ifndef RENAMER_OPTIONS_HPP
#define RENAMER_OPTIONS_HPP

#include <string>

class options final
{
    bool recurse_{false};
    bool include_dir_{false};
    bool silent_rename_{false};
    bool log_{false};
    std::string in_mask_{"*"};
    std::string out_mask_{"*"};
    std::string exclude_mask_{""};
    std::string path_{"."};

public:
    options() = default;
    options(const options & other);
    options(options && other) noexcept = delete;
    options & operator=(const options & other);
    options & operator=(options && other) noexcept = delete;
    ~options() = default;

    bool process(int argc, char ** argv);

    bool recurse() const;
    bool include_dir() const;
    bool silent_rename() const;
    bool log() const;
    std::string in_mask() const;
    std::string out_mask() const;
    std::string exclude_mask() const;
    std::string path() const;
};

#endif // RENAMER_OPTIONS_HPP