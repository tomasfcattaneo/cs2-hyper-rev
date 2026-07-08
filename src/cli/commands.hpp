#pragma once

#include "args.hpp"

namespace cli::commands {

int capture(const options& opts);
int verify(const options& opts);
int trigger(const options& opts);

} // namespace cli::commands
