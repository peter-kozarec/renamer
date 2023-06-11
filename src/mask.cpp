#include "renamer/mask.hpp"
#include "renamer/logger.hpp"

#include <boost/regex.hpp>

mask::mask(const mask & other)
{
    mask_ = other.mask_;
    clear_mask_ = other.clear_mask_;
}

mask & mask::operator=(const mask & other)
{
    if (this != &other)
    {
        mask_ = other.mask_;
        clear_mask_ = other.clear_mask_;
    }
    return *this;
}

bool mask::set_mask(const std::string & mask)
{
    if (!create_clear_mask(mask, clear_mask_))
    {
        BOOST_LOG_WARNING << "Could not create clear mask out of "
                          << mask_ << " filter.";
        return false;
    }

    mask_ = mask;
    return true;
}

std::string mask::get_mask() const
{
    return mask_;
}

bool mask::is_match(const std::string & name) const
{
    boost::regex expression(clear_mask_);

    if (!boost::regex_match(name.begin(), name.end(), expression))
    {
        return false;
    }

    return true;
}

bool mask::get_tokens(const std::string & name, mask_tokens & tokens) const
{
    return true;
}

bool mask::apply_tokens(const std::string & name, const mask_tokens & tokens, std::string & out_name) const
{
    return true;
}

bool mask::create_clear_mask(const std::string raw_mask, std::string clear_mask)
{
    return true;
}