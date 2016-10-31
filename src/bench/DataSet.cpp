#include "DataSet.hpp"

using LazyDataSetIterator = DataSet::LazyDataSetIterator;

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
DataSet::DataSet(std::string filename)
{
	stream.exceptions(std::fstream::badbit | std::fstream::failbit);
	stream.open(filename, std::fstream::in);
	stream >> header;
};


/**
 * Start iteration.
 */
LazyDataSetIterator DataSet::begin()
{
	//TODO: Be careful about making more than one iterator! This is not
	// very intuitive
	return LazyDataSetIterator(stream, header.getDimension(), 0);
};


/**
 * Iterator at the end.
 */
LazyDataSetIterator DataSet::end()
{
	return LazyDataSetIterator(stream, header.getDimension(), size());
};


/**
 * Get size of data set.
 */
unsigned DataSet::size() const
{
	return header.getN();
};


/**
 * Get data dimension.
 */
unsigned DataSet::dimension() const
{
	return header.getDimension();
}

bool DataSet::empty() const
{
	return size() == 0;
}
