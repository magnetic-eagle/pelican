#include <cmath> // std::round, std::stod
#include <iomanip> // std::quoted

#include <pystring.h>

#include <easyqt/logging.hxx> 

#include "exifutils.hxx"

std::optional<double> _stod(const std::string& s) {
	std::optional<double> v = std::nullopt;
	try {
		v = std::stod(s);
	} catch (std::invalid_argument& e) {
		LOG(ERROR, "Parsing floating point number from " << std::quoted(s) << " failed");
	}
	return v;
}

namespace pelican {
	namespace exif {
		std::optional<double> parseNumber(const std::string& s) {
			if (s.empty()) {
				return std::nullopt;
			}
			if (s.find("/") != std::string::npos) {
				const auto ps = pystring::partition(s, "/");
				std::string dividends = ps[0], divisors = ps[2];
				std::optional<double> dividend = _stod(dividends), divisor = _stod(divisors);
				if (dividend && divisor) {
					return *dividend / *divisor;
				} else {
					return std::nullopt;
				}
			} else {
				return _stod(s);
			}
		}
		GEN_FORMAT_FUNC_IMPL(FocalLength, "{:.1f} mm", "- mm",)
		GEN_FORMAT_FUNC_IMPL(Aperture, "F/{:2.1f}", "F/-",)
		
		std::string formatShutterSpeed(const std::string& durationStr) {
			std::optional<double> duration = parseNumber(durationStr);
			if (duration == std::nullopt) {
				return "- s";
			}
			if (*duration < 1) {
				int one_by_duration = std::round(1.0 / *duration);
				return fmt::format("1/{:d} s", one_by_duration);
			} else {
				return fmt::format("{:.0f} s", *duration);
			}
		}
		
		std::string formatISO(const std::string& isoStr) {
			std::optional<double> iso = parseNumber(isoStr);
			if (iso == std::nullopt) {
				return "-";
			}
			return fmt::format("{:.0f}", *iso);
		}
	}
}

