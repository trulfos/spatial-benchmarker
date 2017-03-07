#include "FileHeader.hpp"

FileHeader::FileHeader(unsigned dimension, unsigned n)
	: dimension(dimension), n(n)
{
}

void FileHeader::write(std::ostream& stream) const
{
	stream.write(reinterpret_cast<const char *>(&dimension), sizeof(dimension));
	stream.write(reinterpret_cast<const char *>(&n), sizeof(n));
}

void FileHeader::read(std::istream& stream)
{
	stream.read(reinterpret_cast<char *>(&dimension), sizeof(dimension));
	stream.read(reinterpret_cast<char *>(&n), sizeof(n));
}

std::ostream& operator<<(std::ostream& stream, const FileHeader& fh)
{
	return stream << fh.dimension << ' ' << fh.n;
}

std::istream& operator>>(std::istream& stream, FileHeader& fh)
{
	return stream >> fh.dimension >> fh.n;
}
