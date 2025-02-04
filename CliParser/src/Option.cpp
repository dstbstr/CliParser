#include "CliParser/Option.h"

#include <ostream>
#include <sstream>
#include <optional>
#include <ranges>
#include <algorithm>

namespace {
	constexpr bool IsShortOption(std::string_view sv) {
		return sv.size() > 1 && sv[0] == '-' && (sv.size() == 2 || sv[1] != '-');
	}
	constexpr bool IsLongOption(std::string_view sv) {
		return sv.size() > 2 && (sv[0] == '/' || (sv[0] == '-' && sv[1] == '-'));
	}
	constexpr std::string_view GetOptionName(std::string_view sv) {
		if (sv[0] == '/') {
			return sv.substr(1);
		} else if(sv.size() > 2 && sv[1] == '-') {
			return sv.substr(2);
		} else {
			return sv.substr(1);
		}
	}
}
namespace CliParser {
	bool IArgs::TryParse(int argc, char* argv[], std::ostream& outErrors) {
		std::vector<std::string_view> args;
		args.reserve(argc);
		for (int i = 1; i < argc; ++i) {
			args.emplace_back(argv[i]);
		}
		return TryParse(args, outErrors);
	}

	bool IArgs::TryParse(std::vector<std::string_view> args, std::ostream& outErrors) {
		OptionWrapper* currentOption{nullptr};
		auto FindOption = [&](std::string_view sv) -> OptionWrapper* {
			for (auto& option : Options) {
				if (option->ShortName == sv[0] || option->LongName == sv) {
					return option.get();
				}
			}
			return nullptr;
		};
		for(size_t i = 0; i < args.size(); i++) {
			auto& arg = args[i];
			if(IsShortOption(arg)) {
				for(size_t c = 1; c < arg.size(); c++) {
					currentOption = FindOption(arg.substr(c, 1));
					if (!currentOption) {
						outErrors << "Unknown option: " << arg[c];
						return false;
					}
					if (c < arg.size() - 1) {
						// short arguments that are combined are assumed to be flags
						// the last short argument (or a short by itself) can have a value
						if (!currentOption->TryParse("true", outErrors)) return false;
					}
				}
			} else if(IsLongOption(arg)) {
				currentOption = FindOption(GetOptionName(arg));
				if (!currentOption) {
					outErrors << "Unknown option: " << arg;
					return false;
				}
			} else {
				if(!currentOption) {
					outErrors << "Unsure which argument to provide the value: " << arg;
					return false;
				}
				else if (!currentOption->TryParse(arg, outErrors)) {
					return false;
				} 
				else if(currentOption->LimitedValues) {
					auto limitedValues = currentOption->LimitedValues();
					if(limitedValues.empty()) {
						outErrors << "No valid values for this field currently";
						return false;
					}
					auto found = std::find(limitedValues.begin(), limitedValues.end(), arg);
					if (found == limitedValues.end()) {
						auto valuesString = std::views::join_with(limitedValues, ", ") | std::ranges::to<std::string>();
						outErrors << arg << " in not in [" << valuesString << "]";
						return false;
					}
				}
				currentOption->Populated = true;
				currentOption = nullptr; // reset to avoid multiple values for one name
			}
		}

		auto missingRequired = Options | std::views::filter([](const auto& op) { return !op->IsOptional() && !op->Populated; });
		if(!missingRequired.empty()) {
			auto missingNames = missingRequired | std::views::transform([](const auto& p) { return p->LongName; });
			outErrors << "Missing required option(s): " << (std::views::join_with(missingNames, ", ") | std::ranges::to<std::string>());
			return false;
		}

		return ValidateParsedArgs(outErrors);
	}

	std::string IArgs::Describe() {
		std::stringstream stream;
		//TODO: use std::format
		for(const auto& option : Options) {
			stream <<
				(option->IsOptional() ? "[optional]" : "[required]") << " "
				<< option->ShortName << "|" << option->LongName << "|" << option->HelpText;
			if(option->IsOptional()) {
				stream << "(default: " << option->GetDefaultValue() << ")";
			}
			stream << std::endl;
		}

		return stream.str();
	}
}