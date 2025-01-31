#include "CliParser/ArgParse.h"

namespace CliParser::ArgParse {
	bool TryParse(std::string_view sv, std::string& outResult, std::ostream& outErrors) {
		if(sv.empty()) {
			outErrors << "Empty string";
			return false;
		}
		outResult = std::string(sv);
		return true;
	}

	bool TryParse(std::string_view sv, std::string_view& outResult, std::ostream& outErrors) {
		if (sv.empty()) {
			outErrors << "Empty string";
			return false;
		}
		outResult = sv;
		return true;
	}

	bool TryParse(std::string_view sv, char& outResult, std::ostream& outErrors) {
		if (sv.size() != 1) {
			outErrors << "Expected a single character, got '" << sv << "'";
			return false;
		}
		outResult = sv[0];
		return true;
	}

	bool TryParse(std::string_view sv, bool& outResult, std::ostream& outErrors) {
		auto lower = sv | std::views::transform([](char c) { return static_cast<char>(std::tolower(c)); })
			| std::ranges::to<std::string>();
		
		if(lower == "true" || lower == "1" || lower == "y" || lower == "on") {
			outResult = true;
			return true;
		}
		else if (lower == "false" || lower == "0" || lower == "n" || lower == "off") {
			outResult = false;
			return true;
		} else {
			outErrors << "Could not convert '" << sv << "' to a boolean";
			return false;
		}
	}
}