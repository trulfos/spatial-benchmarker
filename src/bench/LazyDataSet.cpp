#include "LazyDataSet.hpp"

using LazyDataSetIterator = LazyDataSet::LazyDataSetIterator;

LazyDataSetIterator::LazyDataSetIterator(
		std::istream& stream,
		unsigned dimension,
		unsigned index
	) : stream(stream), element(dimension), index(index)
{
};

bool LazyDataSetIterator::operator!=(const LazyDataSetIterator& other)
{
	return index != other.index;
}


LazyDataSetIterator LazyDataSetIterator::operator++()
{
	index++;
	extracted = false;
	return *this;
}

DataObject& LazyDataSetIterator::operator*()
{
	if (!extracted) {
		extracted = true;
		stream >> element;
	}
	return element;
}


/**
 * Create a new lazy data set.
 */
LazyDataSet::LazyDataSet(std::string filename)
{
	stream.exceptions(std::fstream::badbit | std::fstream::failbit);
	stream.open(filename, std::fstream::in);
	stream >> header;
};


/**
 * Start iteration.
 */
LazyDataSetIterator LazyDataSet::begin()
{
	//TODO: Be careful about making more than one iterator! This is not
	// very intuitive
	return LazyDataSetIterator(stream, header.getDimension(), 0);
};


/**
 * Iterator at the end.
 */
LazyDataSetIterator LazyDataSet::end()
{
	return LazyDataSetIterator(stream, header.getDimension(), size());
};


/**
 * Get size of data set.
 */
unsigned LazyDataSet::size() const
{
	return header.getN();
};


/**
 * Get data dimension.
 */
unsigned LazyDataSet::dimension() const
{
	return header.getDimension();
}

bool LazyDataSet::empty() const
{
	return size() == 0;
}
