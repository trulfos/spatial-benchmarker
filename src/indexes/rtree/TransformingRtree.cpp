#pragma once
#include "TransformingRtree.hpp"
#include "Link.hpp"
#include <cassert>

namespace Rtree
{
	template<class R, class N>
	TransformingRtree<R, N>::~TransformingRtree()
	{
		deleteTree(newRoot.node, newHeight);
	}


	template<class R, class N>
	void TransformingRtree<R, N>::prepare()
	{
		newRoot = transform(getRoot(), getHeight());
		newHeight = getHeight();
	}


	template<class R, class N>
	StatsCollector TransformingRtree<R, N>::collectStatistics() const
	{
		StatsCollector stats = R::collectStatistics();

		// Testing, testing
		stats["meh"] = 123;

		return stats;
	}



	template<class R, class N>
	Results TransformingRtree<R, N>::rangeSearch(const Box& box) const
	{
		assert(newHeight > 0);
		assert(newRoot != nullptr);

		using NIt = typename N::iterator;

		std::vector<std::pair<NIt, NIt>> path {{
				newRoot.node->scan(box), newRoot.node->end()
			}};

		Results resultSet;

		while (!path.empty()) {
			auto& top = path.back();

			if (top.first == top.second) {
				path.pop_back();
				continue;
			}

			// Find node to descend into
			const Link<N>& link = *(top.first++);

			if (path.size() < newHeight - 1) {
				const N * const & node = link.node;
				path.emplace_back(
						node->scan(box),
						node->end()
					);
			} else {
				resultSet.push_back(link.id);
			}
		}

		return resultSet;
	}



	template<class R, class N>
	Results TransformingRtree<R, N>::rangeSearch(
			const Box& box,
			StatsCollector& stats
		) const
	{
		Results resultSet;
		return resultSet;
	}



	template<class R, class N>
	Link<N> TransformingRtree<R, N>::transform(
			OldEntry& subtree,
			unsigned height
		)
	{
		// Ids are used as-is
		if (height < 2) {
			return subtree.getId();
		}

		// Transform all children
		N * newSubtree = new N();

		for (auto& entry : subtree) {
			newSubtree->add(
					entry.getMbr(),
					transform(entry, height - 1)
				);
		}

		return newSubtree;
	}



	template<class R, class N>
	void TransformingRtree<R, N>::deleteTree(N * root, unsigned height)
	{
		if (height < 3) {
			return;
		}

		for (Link<N>& l : root->getLinks()) {
			deleteTree(l.node, height - 1);
		}

		delete root;

	}

}
