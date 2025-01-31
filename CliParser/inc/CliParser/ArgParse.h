#pragma once
#include <charconv>
#include <ostream>
#include <string>
#include <string_view>
#include <vector>
#include <ranges>

namespace CliParser::ArgParse {
	constexpr auto DefaultDelimiter = ',';

	template<typename T>
	requires std::is_arithmetic_v<T>
	bool TryParse(std::string_view sv, T& outResult, std::ostream& outErrors) {
		if(sv.empty()) {
			outErrors << "Empty string";
			return false;
		}
		auto end = sv.data() + sv.size();
		auto [ptr, ec] = std::from_chars(sv.data(), end, outResult);
		auto success = (ec == std::errc{} && ptr == end);
		if(!success) {
			outErrors << "Could not convert '" << sv << "' to a number";
		}
		return success;
	}

	bool TryParse(std::string_view sv, std::string& outResult, std::ostream& outErrors);
	bool TryParse(std::string_view sv, std::string_view& outResult, std::ostream& outErrors);
	bool TryParse(std::string_view sv, char& outResult, std::ostream& outErrors);
	bool TryParse(std::string_view sv, bool& outResult, std::ostream& outErrors);

	template<typename T>
	inline bool TryParse(std::string_view sv, std::vector<T>& outResult, std::ostream& outErrors) {
		T value;
		if (!TryParse(sv, value, outErrors)) {
			return false;
		}
		outResult.emplace_back(value);
		return true;
	}

	template<typename T>
	requires std::is_arithmetic_v<T>
	std::string ToString(const T& value) {
		return std::to_string(value);
	}

	inline std::string ToString(const std::string& value) { return value; }
	inline std::string ToString(const std::string_view& value) { return std::string(value); }
	inline std::string ToString(char value) { return std::string(1, value); }

	template<typename T>
	inline std::string ToString(const std::vector<T>& values) {
		return values
			| std::views::transform([](const T& value) { return ToString(value); })
			| std::views::join(DefaultDelimiter)
			| std::ranges::to<std::string>();
	}
}