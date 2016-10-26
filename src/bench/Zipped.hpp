#pragma once
#include <utility>
#include <iterator>

template <typename A, typename B>
class Zipped
{
	public:

		using value_type = std::pair<typename A::value_type&, typename B::value_type&>;

		class ZipIterator : public std::iterator<std::forward_iterator_tag, value_type>
		{
			public:
				ZipIterator(
						typename A::iterator iterA, typename B::iterator iterB
					) : iterA(iterA), iterB(iterB)
				{
				}

				bool operator!=(const ZipIterator& other)
				{
					return iterA != other.iterA || iterB != other.iterB;
				}

				ZipIterator operator++()
				{
					iterA++;
					iterB++;
					return *this;
				}

				value_type operator*()
				{
					return value_type(*iterA, *iterB);
				}

			private:
				typename A::iterator iterA;
				typename B::iterator iterB;

		};

		Zipped(A& a, B& b) : a(a), b(b)
		{
		};

		ZipIterator begin()
		{
			return ZipIterator(a.begin(), b.begin());
		}

		ZipIterator end()
		{
			return ZipIterator(a.end(), b.end());
		}

	private:
		A& a;
		B& b;
};

template <typename A, typename B>
Zipped<A, B> zip(A& a, B& b)
{
	return Zipped<A, B>(a, b);
}
