#ifndef RENAMER_MASK_HPP
#define RENAMER_MASK_HPP

#include <string>
#include <vector>


struct mask_token
{
    std::string name;
    std::string value;
};

typedef std::vector<mask_token> mask_tokens;
 
class mask final
{
    std::string mask_;
    std::string clear_mask_;

public:
    mask() = default;
    mask(const mask & other);
    mask(mask && other) noexcept = delete;
    mask & operator=(const mask & other);
    mask & operator=(mask && other) noexcept = delete;
    ~mask() = default;

    bool set_mask(const std::string & mask);
    std::string get_mask() const;

    bool is_match(const std::string & name) const;
    bool get_tokens(const std::string & name, mask_tokens & tokens) const;
    bool apply_tokens(const std::string & name, const mask_tokens & tokens, std::string & out_name) const;

private:
    static bool create_clear_mask(const std::string raw_mask, std::string clear_mask);
};


#endif // RENAMER_MASK_HPP