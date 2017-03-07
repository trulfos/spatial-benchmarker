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

		unsigned getN() const
		{
			return n;
		};

		unsigned getDimension() const
		{
			return dimension;
		};

		/**
		 * Write this file header to a file in binary.
		 */
		void write(std::ostream& stream) const;


		/**
		 * Read this header from a binary file.
		 */
		void read(std::istream& stream);

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
