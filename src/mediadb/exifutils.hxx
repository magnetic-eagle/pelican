#ifndef PELICAN_EXIFUTILS_HXX
#define PELICAN_EXIFUTILS_HXX

#include <string>
#include <optional>

#include <fmt/core.h>

#define GEN_FORMAT_FUNC_IMPL(name, formatStr, default, code) \
std::string format##name(const std::string& name##Str) { \
	std::optional<double> name = parseNumber(name##Str); \
	if (name == std::nullopt) { \
		return default; \
	} \
	code \
	return fmt::format(formatStr, *name); \
}


namespace pelican {
	namespace exif {
		std::optional<double> parseNumber(const std::string& s);
		std::string formatFocalLength(const std::string& focalLength);
		std::string formatShutterSpeed(const std::string& duration_s);
		std::string formatAperture(const std::string& fNumber);
		std::string formatISO(const std::string& iso);
	}
}

#endif

