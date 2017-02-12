#include "QueryInputIterator.hpp"

QueryInputIterator::QueryInputIterator(
		std::istream& stream, unsigned dimension
	) : BoxInputIterator(stream, dimension)
{
	extract();
};

const RangeQuery& QueryInputIterator::operator*() const
{
	return query;
};


const RangeQuery * QueryInputIterator::operator->() const
{
	return &(operator*());
};


QueryInputIterator QueryInputIterator::operator++()
{
	BoxInputIterator::operator++();
	extract();
	return *this;
};



QueryInputIterator QueryInputIterator::operator++(int)
{
	QueryInputIterator tmp = *this;
	operator++();
	return tmp;
};


QueryInputIterator::difference_type QueryInputIterator::operator-(
		QueryInputIterator other
	)
{
	return BoxInputIterator::operator-(
			static_cast<BoxInputIterator&>(other)
		);
};

void QueryInputIterator::extract()
{
	query = RangeQuery(id++, BoxInputIterator::operator*());
}
