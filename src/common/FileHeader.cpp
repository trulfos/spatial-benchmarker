#include "FileHeader.hpp"

FileHeader::FileHeader(unsigned dimension, unsigned n)
	: dimension(dimension), n(n)
{
}

std::ostream& operator<<(std::ostream& stream, const FileHeader& fh)
{
	return stream << fh.dimension << ' ' << fh.n;
}

std::istream& operator>>(std::istream& stream, FileHeader& fh)
{
	return stream >> fh.dimension >> fh.n;
}
