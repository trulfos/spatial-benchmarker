#include "DataObjectInputIterator.hpp"

DataObjectInputIterator::DataObjectInputIterator(
		std::istream& stream, unsigned dimension
	) : BoxInputIterator(stream, dimension)
{
	extract();
};

const DataObject& DataObjectInputIterator::operator*() const
{
	return object;
};


const DataObject * DataObjectInputIterator::operator->() const
{
	return &(operator*());
};


DataObjectInputIterator DataObjectInputIterator::operator++()
{
	BoxInputIterator::operator++();
	extract();
	return *this;
};



DataObjectInputIterator DataObjectInputIterator::operator++(int)
{
	DataObjectInputIterator tmp = *this;
	operator++();
	return tmp;
};


DataObjectInputIterator::difference_type DataObjectInputIterator::operator-(
		DataObjectInputIterator other
	)
{
	return BoxInputIterator::operator-(
			static_cast<BoxInputIterator&>(other)
		);
};


void DataObjectInputIterator::extract()
{
	object = DataObject(id++, BoxInputIterator::operator*());
}
