#include "renamer/options.hpp"
#include "renamer/logger.hpp"
#include "renamer/options.hpp"
#include "renamer/mask.hpp"

#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>

namespace fs = boost::filesystem;
namespace lt = boost::log::trivial;
namespace bs = boost::system;

struct renamer_descriptor
{
    fs::path old_descriptor;
    fs::path new_descriptor;
};

using path_descriptors    = std::vector<fs::path>;
using renamer_descriptors = std::vector<renamer_descriptor>;

int main(int argc, char ** argv)
{
    //
    // Initialize app logger. When building with NDEBUG option
    // trace level logging is enabled. To increase logging level
    // to "debug" use --l program option.
    //
    initialize_logger();

    options opts;

    //
    // Process program options. Exit with 0 return code if
    // --version or --help is used. Exit with 1 return code
    // if invalid argument or value is used.
    //
    if (!opts.process(argc, argv))
    {
        BOOST_LOG_FATAL << "Failed to process program options. Exiting...";
        return EXIT_FAILURE;
    }

    //
    // Increase log level if log option (--l) is present. In
    // debug build type this option is disabled, since max log
    // severity is used.
    //
#ifndef DEBUG
    if (opts.log())
    {
        change_logger_severity(lt::debug);
    }
#endif // NDEBUG


    const fs::path working_dir(opts.path());

    //
    // Check if renamers working path is a directory. If not exit with
    // return code 1.
    //
    if (!fs::is_directory(working_dir))
    {
        BOOST_LOG_FATAL << opts.path() << " is not a directory. Exiting...";
        return EXIT_FAILURE;
    }

    path_descriptors descriptors;

    //
    // Retrieve file descriptors to the files and directories inside
    // the path program option. If retrieval failed exit with return code 1.
    //
    if (opts.recurse())
    {
        for (fs::recursive_directory_iterator end, it(working_dir); it != end; ++it)
        {
            if ((fs::is_directory(it->path()) && opts.include_dir()) || 
                (fs::is_regular_file(it->path())))
            {
                descriptors.push_back(it->path());
            }
        }
    }
    else
    {
        for (fs::directory_iterator end, it(working_dir); it != end; ++it)
        {
            if ((fs::is_directory(it->path()) && opts.include_dir()) || 
                (fs::is_regular_file(it->path())))
            {
                descriptors.push_back(it->path());
            }
        }
    }

    //
    // If there is nothing to rename exit with return code 0;
    //
    if (descriptors.empty())
    {
        BOOST_LOG_INFO << "Nothing to rename. Exiting...";
        return EXIT_SUCCESS;
    }

    mask exclude_mask;

    //
    // Set exclude mask. If not valid exit with return code 1.
    //
    if (!exclude_mask.set_mask(opts.exclude_mask()))
    {
        BOOST_LOG_FATAL << "Invalid exclude mask " << opts.exclude_mask() << ". Exiting...";
        return EXIT_FAILURE;
    }

    //
    // Apply the exclude mask on the retrieved descriptors, removing
    // excluded descriptors from list.
    //
    for (auto it = descriptors.begin(); it != descriptors.end(); ++it)
    {
        if (!exclude_mask.is_match(it->filename().c_str()))
        {
            descriptors.erase(it);
        }
    }

    //
    // If there is nothing to rename exit with return code 0;
    //
    if (descriptors.empty())
    {
        BOOST_LOG_INFO << "Nothing to rename. Exiting...";
        return EXIT_SUCCESS;
    }

    mask in_mask;

    //
    // Set input mask. If not valid exit with return code 1.
    //
    if (!in_mask.set_mask(opts.in_mask()))
    {
        BOOST_LOG_FATAL << "Invalid input mask " << opts.in_mask() << ". Exiting...";
        return EXIT_FAILURE;
    }

    mask out_mask;

    //
    // Set output mask. If not valid exit with return code 1.
    //
    if (!out_mask.set_mask(opts.out_mask()))
    {
        BOOST_LOG_FATAL << "Invalid output mask " << opts.out_mask() << ". Exiting...";
        return EXIT_FAILURE;
    }

    renamer_descriptors renamer_descriptors;

    for (const auto & descr : descriptors)
    {
        mask_tokens tokens;

        if (in_mask.get_tokens(descr.filename().c_str(), tokens))
        {
            BOOST_LOG_TRACE << "Tokens for "
                            << descr.filename().c_str()
                            << " succesfully retrieved.";

            for (const auto & token : tokens)
            {
                BOOST_LOG_TRACE << token.name
                                << " = "
                                << token.value;
            }
        }
        else
        {
            BOOST_LOG_WARNING << "Could not retrieve tokens from " 
                              << descr.filename().c_str()
                              << " with input mask "
                              << opts.in_mask()
                              << ". Exiting...";
            continue;
        }

        std::string out_filename;

        if (out_mask.apply_tokens(descr.filename().c_str(), tokens, out_filename))
        {
            if (out_filename.empty())
            {
                BOOST_LOG_WARNING << "Empty string is not a valid output name for "
                                  << descr.c_str()
                                  << ". Removing from final rename list.";
                continue;
            }

            BOOST_LOG_TRACE << "Succesfully formated output file name as "
                            << out_filename;

            renamer_descriptors.push_back({descr, fs::path(descr.root_path().c_str() + out_filename)});
        }
        else
        {
            BOOST_LOG_WARNING << "Could not format output file name for "
                              << descr.filename().c_str();
            continue;
        }
    }

    //
    // If there is nothing to rename exit with return code 0;
    //
    if (renamer_descriptors.empty())
    {
        BOOST_LOG_INFO << "Nothing to rename. Exiting...";
        return EXIT_SUCCESS;
    }

    //
    // Print and ask user if silent rename is not active (--s) to confirm.
    //
    if (!opts.silent_rename())
    {
        std::cout << std::endl;
        for (const auto & renamer_descriptor : renamer_descriptors)
        {
            BOOST_LOG_INFO << renamer_descriptor.old_descriptor
                           << " >> "
                           << renamer_descriptor.new_descriptor;
        }
        std::cout << std::endl;

        BOOST_LOG_INFO << "Do you want to rename all? [Y/n] ";

        std::string user_input;
        std::cin >> user_input;

        //
        // If user does not want to rename exit with return code 0.
        //
        if (user_input != "Y" || user_input != "y")
        {
            return EXIT_SUCCESS;
        }
    }

    bool all_renamed_succesfully = true;
    unsigned int error_count = 0;

    //
    // Rename
    //
    for (const auto & renamer_descriptor : renamer_descriptors)
    {
        bs::error_code ec;

        fs::rename(renamer_descriptor.old_descriptor, renamer_descriptor.new_descriptor, ec);

        if (ec != bs::errc::success)
        {
            BOOST_LOG_DEBUG << ec.message();
            BOOST_LOG_WARNING << "Could not rename "
                              << renamer_descriptor.old_descriptor.c_str();

            all_renamed_succesfully = false;
            ++error_count;
            continue;
        }
    }

    if (all_renamed_succesfully)
    {
        BOOST_LOG_INFO << "Everything was renamed succesfully.";
    }
    else
    {
        BOOST_LOG_WARNING << "There were " << error_count << " error(s).";
    }

    return EXIT_SUCCESS;
}

