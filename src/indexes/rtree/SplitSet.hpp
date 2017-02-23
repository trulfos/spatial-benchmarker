#pragma once
#include "common/Algorithm.hpp"
#include "Split.hpp"
#include <vector>
#include <algorithm>
#include <memory>
#include <iterator>

/**
 * A collection of possible splits.
 */
template<class E, unsigned m>
class SplitSet
{
	public:
		static constexpr unsigned D = E::dimension;

		using value_type = Split<E>;

		/**
		 * Iterator for iterating through a split set.
		 */
		class SplitIterator
			: public std::iterator<std::forward_iterator_tag, value_type>
		{
			public:
				/**
				 * Create an iterator iterating through the given dimension.
				 */
				SplitIterator(
						const std::shared_ptr<std::vector<E>>& entries,
						unsigned dimension,
						bool skipDimension = true
					) : dimension(dimension), skipDimension(skipDimension),
						entries(entries)
				{
					std::transform(
							entries->begin(), entries->end(),
							std::back_inserter(entryView),
							[](E& e) { return &e; }
						);

					sort();

					split = std::make_shared<Split<E>>(
							entryView,
							sortOrder,
							dimension,
							splitPoint
						);
				}

				/**
				 * Create a new iterator iterating through all dimensions.
				 */
				SplitIterator(const std::shared_ptr<std::vector<E>>& entries)
					: SplitIterator(entries, 0, false)
				{
				}


				/**
				 * Singular iterator used for end.
				 */
				SplitIterator()
					: dimension(D)
				{
				}

				bool operator==(const SplitIterator& other) const
				{
					return splitPoint == other.splitPoint &&
						sortOrder == other.sortOrder &&
						dimension == other.dimension;
				}


				bool operator!=(const SplitIterator& other) const
				{
					return !(*this == other);
				}


				/**
				 * Increment to the next split.
				 */
				SplitIterator operator++()
				{
					++splitPoint;

					// Increment sort order?
					if (splitPoint == entryView.size() - m) {
						splitPoint = m;
						++sortOrder;

						// Incremement dimension?
						if (sortOrder == 2) {
							sortOrder = 0;

							if (skipDimension) {
								dimension = D;
							} else {
								++dimension;
							}

							if (dimension == D) {
								return *this;
							}
						}

						// We need to re-sort anyway
						sort();
					}

					// Create new split
					split = std::make_shared<Split<E>>(
							entryView,
							sortOrder,
							dimension,
							splitPoint
						);

					return *this;
				}

				const Split<E>& operator*() const
				{
					return *split;
				}


			private:

				unsigned splitPoint = m,
						sortOrder = 0,
						dimension = 0;

				bool skipDimension = false;

				std::shared_ptr<Split<E>> split;
				std::shared_ptr<std::vector<E>> entries;
				std::vector<E *> entryView;

				/**
				 * Sort the entries in this iterator.
				 */
				void sort()
				{
					unsigned& d = dimension;
					if (sortOrder == 0) {
						std::sort(
							entryView.begin(), entryView.end(),
							[&](const E * a, const E * b) {
								return std::tie(
										a->mbr.getTop()[d],
										a->mbr.getBottom()[d]
									) < std::tie(
											b->mbr.getTop()[d],
											b->mbr.getBottom()[d]
										);
							}
						);

						return;
					}

					std::sort(
						entryView.begin(), entryView.end(),
						[&](const E * a, const E * b) {
								return std::tie(
										a->mbr.getBottom()[d],
										a->mbr.getTop()[d]
									) < std::tie(
											b->mbr.getBottom()[d],
											b->mbr.getTop()[d]
										);
						}
					);
				}
		};

		using iterator = SplitIterator;

		/**
		 * Construct a set of splits from th entries in two ranges.
		 */
		template<class FIt>
		SplitSet(FIt first, FIt first2, FIt last, FIt last2) : SplitSet()
		{
			entries->insert(entries->end(), first, last);
			entries->insert(entries->end(), first2, last2);
		}

		/**
		 * Create an empty split set.
		 */
		SplitSet()
		{
			entries = std::make_shared<std::vector<E>>();
		}

		/**
		 * Start iteration over all splits in this set.
		 */
		iterator begin() const
		{
			if (dimension == D) {
				return iterator(entries);
			}

			return iterator(entries, dimension);
		}

		/**
		 * Past-the-end iterator for this set.
		 */
		iterator end() const
		{
			return iterator();
		}

		/**
		 * Restrict to only the given dimension.
		 * Only splits across the given dimension will be considered.
		 *
		 * @param dimension Dimension to consider
		 */
		void restrictTo(unsigned dimension)
		{
			assert(dimension < D);
			this->dimension = dimension;
		}

	private:
		unsigned dimension = D;
		std::shared_ptr<std::vector<E>> entries;
};
