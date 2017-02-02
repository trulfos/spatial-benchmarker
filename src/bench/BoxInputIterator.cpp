#include "BoxInputIterator.hpp"
#include <cassert>
#include "common/Coordinate.hpp"

/**
 * BoxSet implementation
 */

BoxInputIterator::BoxInputIterator(std::istream& stream, unsigned dimension)
	: stream(&stream), box(dimension)
{
	operator++();
}

BoxInputIterator::BoxInputIterator()
	: stream(nullptr), box(0)
{
}


bool BoxInputIterator::operator==(const BoxInputIterator& other) const
{
	return (!stream && !other.stream) || stream == other.stream;
}


bool BoxInputIterator::operator!=(const BoxInputIterator& other) const
{
	return !(*this == other);
}


const Box& BoxInputIterator::operator*() const
{
	//TODO: assert(stream != nullptr && *stream);
	return box;
}


const Box * BoxInputIterator::operator->() const
{
	return &(operator*());
}


BoxInputIterator BoxInputIterator::operator++()
{
	assert(stream != nullptr && *stream);

	unsigned dimension = box.getDimension();

	// Extract each point
	Point a (dimension);
	Point b (dimension);

	// Read doubles
	for (unsigned i = 0; i < dimension; ++i) {
		stream->read(
				reinterpret_cast<char *>(&a[i]),
				sizeof(a[i])
			);

		stream->read(
				reinterpret_cast<char *>(&b[i]),
				sizeof(b[i])
			);
	}

	// Construct box
	box = Box(a, b);

	// Check if we were successful
	if (!*stream) {
		stream = nullptr;
	}

	return *this;
}


BoxInputIterator BoxInputIterator::operator++(int)
{
	BoxInputIterator tmp = *this;
	operator++();
	return tmp;
}

/**
 * Compute the distance between two iterators.
 *
 * This can be used to determine the number of items left in the current
 * stream. Note that this will require seeking through the stream.
 */
BoxInputIterator::difference_type BoxInputIterator::operator-(
		BoxInputIterator& other
	)
{
	// This will have to be called on an iterator with a valid stream
	if (other.stream == nullptr) {

		// Prevent endless recursion
		assert(stream != nullptr);

		return -(other - *this);
	}

	assert(*other.stream);

	return 1u + getStreamSize(*other.stream) /
		(2u * sizeof(Coordinate) * other.box.getDimension());
}


unsigned BoxInputIterator::getDimension() const
{
	return box.getDimension();
}


unsigned BoxInputIterator::getStreamSize(std::istream& stream)
{
	// Record state and seek to end
	std::istream::pos_type position = stream.tellg();
	stream.seekg(0, std::istream::end);

	if (!stream) {
		throw std::runtime_error("Could not seek to end of stream");
	}

	// Save position
	unsigned bytes = stream.tellg();

	// Restore state
	stream.seekg(position);

	return bytes - position;
}
