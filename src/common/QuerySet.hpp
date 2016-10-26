#pragma once

#include <vector>
#include "Query.hpp"
#include "KnnQuery.hpp"
#include "RangeQuery.hpp"

class QuerySet : private std::vector<Query *>
{
	public:
		using iterator = std::vector<Query *>::iterator;
		using value_type = Query&;

		class QuerySetIterator : public std::vector<Query *>::const_iterator
		{
			public:
				QuerySetIterator(Query * const * start)
					: std::vector<Query *>::const_iterator(start)
				{
				}

				const Query& operator*()
				{
					return *std::vector<Query *>::const_iterator::operator*();
				}

		};


		~QuerySet();

		using std::vector<Query *>::size;


		/**
		 * Get the dimension of this query set.
		 *
		 * TODO: Verify all queries have the same dimension
		 */
		unsigned getDimension() const;


		/**
		 * Begin iteration (using references).
		 */
		QuerySetIterator begin() const
		{
			return QuerySetIterator(data());
		}

		QuerySetIterator end() const
		{
			return QuerySetIterator(data() + size());
		}

		/**
		 * Add a query.
		 * The query will be copied into this query set.
		 */
		template <class Q>
		void add(const Q& q)
		{
			push_back(new Q(q));
		}

		/**
		 * Get a query.
		 */
		const Query& operator[](size_t i) const;

	private:
		friend std::istream& operator>>(
				std::istream& stream, QuerySet& querySet
			);
};

std::ostream& operator<<(std::ostream& stream, const QuerySet& querySet);
