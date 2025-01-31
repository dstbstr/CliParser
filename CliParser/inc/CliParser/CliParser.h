#pragma once
#include <string>
#include <vector>

#include "CliParser/Option.h"
#include "CliParser/CommandRegistry.h"

namespace CliParser {
	constexpr int SuccessCode = 0;

	int Run(int argc, char* argv[], std::ostream& outErrors);
	int Run(const std::vector<std::string>& args, std::ostream& outErrors);
}