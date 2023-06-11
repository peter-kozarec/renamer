#include "renamer/options.hpp"
#include "renamer/version.hpp"
#include "renamer/logger.hpp"

#include <iostream>
#include <boost/program_options.hpp>

options::options(const options & other)
{
    recurse_ = other.recurse_;
    silent_rename_ = other.silent_rename_;
    include_dir_ = other.include_dir_;
    in_mask_ = other.in_mask_;
    out_mask_ = other.out_mask_;
    exclude_mask_ = other.exclude_mask_;
    path_ = other.path_;
}

options & options::operator=(const options & other)
{
    if (this != &other)
    {
        recurse_ = other.recurse_;
        silent_rename_ = other.silent_rename_;
        include_dir_ = other.include_dir_;
        in_mask_ = other.in_mask_;
        out_mask_ = other.out_mask_;
        exclude_mask_ = other.exclude_mask_;
        path_ = other.path_;
    }

    return *this;
}

bool options::process(int argc, char ** argv)
{
    boost::program_options::options_description desc("Allowed options");
    boost::program_options::variables_map vm;

    desc.add_options()
        ("help,h", "Prints help.")
        ("version,v", "Prints program version.")
        ("recurse,r", boost::program_options::value<bool>(&recurse_)->default_value(false), "Recurse into directories.")
        ("silent,s", boost::program_options::value<bool>(&silent_rename_)->default_value(false), "Silent renaming.")
        ("directories,d", boost::program_options::value<bool>(&include_dir_)->default_value(false), "Include directories.")
#ifndef NDEBUG
        //
        // Disabled for debug build, since max debug level is used.
        //
        ("log,l", boost::program_options::value<bool>(&log_)->default_value(false), "Increase log level.")
#endif // NDEBUG
        ("in,i", boost::program_options::value<std::string>(&in_mask_)->default_value("*"), "Input file mask.")
        ("out,o", boost::program_options::value<std::string>(&out_mask_)->default_value(""), "Output file mask.")
        ("exclude,e", boost::program_options::value<std::string>(&exclude_mask_)->default_value(""), "Exclude mask.")
        ("path,p", boost::program_options::value<std::string>(&path_)->default_value("./"), "Path to directory.");

    try
    {
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
        boost::program_options::notify(vm);    
    }
    catch(const std::exception& e)
    {
        BOOST_LOG_FATAL << e.what();
        return false;
    }
    
    if (vm.count("help"))
    {
        std::cout << "Usage:\n" 
                  << "renamer [Allowed options]\n\n"
                  << desc << "\n"
                  << "Mask format:\n"
                  << "  *   Wildcard for multiple characters.\n"
                  << "  ?   Wildcard for one character.\n"
                  << "  [name:length]   Copy number of characters specified from in mask to out mask\n\n"
                  << "Mask examples:\n"
                  << "  \"hello_v1_2\" can be masked as \"hello_v*\", \"hello_v1_?\" or \"hel*o?v[version:3]\"\n";
        exit(EXIT_SUCCESS);
    }

    if (vm.count("version"))
    {
        std::cout << "renamer v" << version << "\n\n"
                  << "License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>.\n"
                  << "This is free software: you are free to change and redistribute it.\n"
                  << "There is NO WARRANTY, to the extent permitted by law.\n\n"
                  << "Written by Peter Kozarec.\n";
        exit(EXIT_SUCCESS);
    }

    return true;
}

bool options::recurse() const
{
    return recurse_;
}

bool options::include_dir() const
{
    return include_dir_;
}

bool options::silent_rename() const
{
    return silent_rename_;
}

bool options::log() const
{
    return log_;
}

std::string options::in_mask() const
{
    return in_mask_;
}

std::string options::out_mask() const
{
    return out_mask_;
}

std::string options::exclude_mask() const
{
    return exclude_mask_;
}

std::string options::path() const
{
    return path_;
}
