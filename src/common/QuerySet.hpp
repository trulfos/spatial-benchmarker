#pragma once

#include <vector>
#include "Query.hpp"

class QuerySet : private std::vector<Query *>
{

	public:
		~QuerySet();

		using std::vector<Query *>::size;
		using std::vector<Query *>::begin;
		using std::vector<Query *>::end;

		using iterator = std::vector<Query *>::iterator;
		using value_type = Query *;

		template <class Q>
		void add(const Q& q)
		{
			push_back(new Q(q));
		}

		Query& operator[](size_t i);

	private:
		friend std::istream& operator>>(
				std::istream& stream, QuerySet& querySet
			);
};
