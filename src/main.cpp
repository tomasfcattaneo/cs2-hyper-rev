#include "cli/args.hpp"
#include "cli/commands.hpp"

#include <iostream>

int main(const int argc, char* argv[])
{
    std::cout.setf(std::ios::unitbuf);
    std::cerr.setf(std::ios::unitbuf);

    const auto opts = cli::parse(argc, argv);

    using cli::command;
    switch (opts.verb) {
        case command::help:    cli::print_usage(); return 0;
        case command::capture: return cli::commands::capture(opts);
        case command::verify:  return cli::commands::verify(opts);
        case command::trigger: return cli::commands::trigger(opts);
    }
    return 0;
}
