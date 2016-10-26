#pragma once
#include <ostream>
#include <istream>

/**
 * Represents the header in files.
 * Consists of the dimension and a number of entries.
 */
class FileHeader
{

	public:
		FileHeader() = default;
		FileHeader(unsigned dimension, unsigned n);

	private:
		unsigned dimension;
		unsigned n;

		friend std::ostream& operator<<(
				std::ostream& stream, const FileHeader& fh
			);

		friend std::istream& operator>>(
				std::istream& stream, FileHeader& fh
			);
};
