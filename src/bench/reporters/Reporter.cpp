#include "Reporter.hpp"

namespace Bench
{

std::ostream& operator<<(
		std::ostream& stream, const std::shared_ptr<Reporter>& reporter
	)
{
	reporter->generate(stream);
	return stream;
}

}
