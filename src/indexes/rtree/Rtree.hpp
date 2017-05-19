#pragma once
#include "spatial/SpatialIndex.hpp"
#include "spatial/InvalidStructureError.hpp"
#include "KnnQueueEntry.hpp"
#include "Mbr.hpp"
#include "Entry.hpp"
#include <algorithm>
#include <vector>


namespace Rtree
{

/**
 * R-tree algorithms without optimizations.
 *
 * Does not consider disk pages, as this is memory resident anyway. The node
 * layout and node scan strategy is entriely defined by the node itself.
 *
 * The height of the tree may be slightly confusing, but becomes natural when
 * considering the root as an entry. Some explantions:
 *  - Level 0: The tree is empty
 *  - Level 1: The root entry contains a data object
 *  - Level 2: The root entry points to a node with data objects
 *
 * @tparam N Node type
 * @tparam m Minimum node children
 */
template <class N, unsigned m = 1>
class Rtree : public ::SpatialIndex
{
	public:
		using M = typename N::Mbr;
		using Id = DataObject::Id;

		/**
		 * Construct a new index from the given data set.
		 */
		Rtree();


		/**
		 * Walks the tree and deletes all nodes.
		 */
		virtual ~Rtree();


		/**
		 * Insert a data object into this index.
		 *
		 * @param object Data object to insert
		 */
		virtual void insert(const DataObject& object) = 0;


		/**
		 * Get the tree height.
		 *
		 * @return Height of tree
		 */
		const unsigned& getHeight() const;


		/**
		 * Get root node.
		 *
		 * @return Pointer to root
		 */
		Entry<N>& getRoot();


		/**
		 * Add a new level by replacing the root.
		 */
		void addLevel(const Entry<N>& newRoot);


		/**
		 * Sanity checks on the structure of this R-tree.
		 *
		 * Currently checks the following:
		 *  - All MBRs are contained within their parents MBR,
		 *  - MBRs are as tight as possible and
		 *  - the upper and lower number of children is respected
		 */
		void checkStructure() const override;


		/**
		 * Collects tree statistics.
		 *
		 * Walks through the tree and e.g. counts the number of nodes at each
		 * level.
		 *
		 * @return Statistics collected
		 */
		StatsCollector collectStatistics() const override;




	protected:

		/**
		 * Traverses the entire tree and executes the visitor for each entry.
		 *
		 * The tree is traversed in preorder and executes the visitor with the
		 * entry and level as arguments. The children of an entry is visited if
		 * the visitor returns true. Data entrys are not visited and neither is
		 * the root.
		 *
		 * @param visitor Function taking a entry and the level (depth)
		 */
		template<class F>
		void traverse(F visitor) const;


		/**
		 * Range search with Guttman's algorithm.
		 */
		void rangeSearch(Results& results, const Box& box) const override;


		/**
		 * Range search with Guttman's algorithm - with instrumentation.
		 */
		void rangeSearch(StatsCollector& stats, const Box& box) const override;


		/**
		 * k-NN search is currently not implemented, but throws when called.
		 */
		void knnSearch(Results&, unsigned, const Point&) const override;

	private:

		unsigned height;
		Entry<N> root;


		/**
		 * Deletes the nodes in this tree.
		 *
		 * Traverse the tree and delete all nodes on the way.
		 *
		 * @param root Root node of tree to delete
		 * @param height Height of given tree
		 */
		void deleteTree(N& root, unsigned height);
};

/*
 ___                 _                           _        _   _             
|_ _|_ __ ___  _ __ | | ___ _ __ ___   ___ _ __ | |_ __ _| |_(_) ___  _ __  
 | || '_ ` _ \| '_ \| |/ _ \ '_ ` _ \ / _ \ '_ \| __/ _` | __| |/ _ \| '_ \ 
 | || | | | | | |_) | |  __/ | | | | |  __/ | | | || (_| | |_| | (_) | | | |
|___|_| |_| |_| .__/|_|\___|_| |_| |_|\___|_| |_|\__\__,_|\__|_|\___/|_| |_|
              |_|                                                           
*/
template <class N, unsigned m>
Rtree<N, m>::Rtree() : height(0)
{
};

template <class N, unsigned m>
Rtree<N, m>::~Rtree()
{
	deleteTree(getRoot().getNode(), getHeight());
};


template <class N, unsigned m>
const unsigned& Rtree<N, m>::getHeight() const
{
	return height;
};


template <class N, unsigned m>
Entry<N>& Rtree<N, m>::getRoot()
{
	return root;
};


template <class N, unsigned m>
void Rtree<N, m>::addLevel(const Entry<N>& newRoot)
{
	root = newRoot;
	height++;
};


template <class N, unsigned m>
void Rtree<N, m>::checkStructure() const
{
	traverse([&](const Entry<N>& entry, unsigned level) {
		// Skip leafs
		if (level == height) {
			return false;
		}

		// Check child count
		const unsigned& size = entry.getNode().getSize();

		if (size > N::capacity) {
			throw InvalidStructureError(
					"Too many children of node"
				);
		}

		if (size < (level == 1 ? 2 : m)) {
			throw InvalidStructureError(
					"Too few children at level " + std::to_string(level)
				);
		}

		// Check MBR containment
		N& node = entry.getNode();
		M mbr = node[0].getMbr();

		for (const auto& e : node) {
			if (!entry.getMbr().contains(e.getMbr())) {
				throw InvalidStructureError(
						"Node not contained within parent at level " +
						std::to_string(level)
					);
			}

			mbr += e.getMbr();
		}

		if (mbr != entry.getMbr()) {
			throw InvalidStructureError(
					"MBR too large at level " + std::to_string(level)
				);
		}


		return true;
	});
}


template <class N, unsigned m>
StatsCollector Rtree<N, m>::collectStatistics() const
{
	StatsCollector stats;

	stats["height"] = height;
	stats["nodes"] = 0;
	stats["level_" + std::to_string(height)] = 1;

	traverse([&](const Entry<N>& entry, unsigned level) {
		// Skip leafs
		if (level == height) {
			return false;
		}

		std::string key = "level_" + std::to_string(height - level);
		stats[key] += entry.getNode().getSize();
		stats["nodes"]++;
		return true;
	});

	return stats;
};


template <class N, unsigned m>
template<class F>
void Rtree<N, m>::traverse(F visitor) const
{
	if (height < 2) {
		return;
	}

	using EIt = typename N::const_iterator;

	const N& rootNode = root.getNode();

	if (!visitor(root, 1)) {
		return;
	}

	if (height == 2) {
		return;
	}

	// Store start and end iterators for each level
	std::vector<std::pair<EIt, EIt>> path {
			{rootNode.begin(), rootNode.end()}
		};

	while (!path.empty()) {
		auto& top = path.back();

		// Have we finished this node?
		if (top.first == top.second) {
			path.pop_back();
			continue;
		}

		const Entry<N> entry = *(top.first++);

		// Call visitor
		bool descend = visitor(entry, path.size() + 1);

		// Push children (if they exist)
		if (descend && path.size() < getHeight() - 1) {
			const N& node = entry.getNode();
			path.emplace_back(node.begin(), node.end());
		}
	}
}


template <class N, unsigned m>
void Rtree<N, m>::rangeSearch(Results& results, const Box& box) const
{
	assert(getHeight() > 0);
	using NIt = typename N::ScanIterator;
	using Link = typename N::Link;
	const typename N::Mbr query (box);

	unsigned depth = 0;
	std::pair<NIt, NIt> path [getHeight()];

	// "Scan" root node
	path[depth++] = root.getNode().scan(query);

	while (depth) {
		auto& top = path[depth - 1];

		if (top.first == top.second) {
			--depth;
			continue;
		}

		// Find node to descend into
		const Link link = *top.first;
		++top.first;

		if (depth < getHeight() - 1) {
			path[depth++] = link.getNode().scan(query);
		} else {
			results.push_back(link.getId());
		}
	}
};


template <class N, unsigned m>
void Rtree<N, m>::rangeSearch(StatsCollector& stats, const Box& box) const
{
	Results resultSet;
	stats["leaf_accesses"] = 0;
	stats["node_accesses"] = 0;
	stats["contained"] = 0;
	stats["includes"] = 0;
	stats["cut"] = 0;
	stats["unnecessary_comparisons"] = 0;

	stats["skipped"] = 0;
	unsigned lastLevel = 0;
	bool descended = false;

	traverse([&](const Entry<N>& entry, unsigned level) {
			// Detect skipped nodes
			if (level > lastLevel) {
				descended = false;
			} else 	if (level < lastLevel && !descended) {
				descended = true;
				stats["skipped"]++;
			}

			descended |= (level == height);
			lastLevel = level;

			// Skip nodes not overlapping
			if (!entry.getMbr().intersects(box)) {
				return false;
			}

			// Count leaf accesses
			if (level == height - 1) {
				stats["leaf_accesses"]++;
			}

			// Push result if leaf
			if (level == height) {
				resultSet.push_back(entry.getId());
				return false;
			}

			// Count skippable checks
			M qMbr (box);

			for (unsigned d = 0; d < M::dimension; ++d) {
				if (qMbr.getTop()[d] >= entry.getMbr().getTop()[d]) {
					stats["unnecessary_comparisons"]++;
				}

				if (qMbr.getBottom()[d] <= entry.getMbr().getBottom()[d]) {
					stats["unnecessary_comparisons"]++;
				}


			}

			if (qMbr.contains(entry.getMbr())) {
				stats["contained"]++;
			}

			if (entry.getMbr().contains(qMbr)) {
				stats["includes"]++;
			}

			if (qMbr.intersectionComplexity(entry.getMbr()) == 1) {
				stats["cut"]++;
			}

			// Count node accesses
			stats["node_accesses"]++;
			return true;
		});

	// Never reached the bottom after last descend?
	if (!descended) {
		stats["skipped"]++;
	}

	stats["results"] = resultSet.size();
};


template <class N, unsigned m>
void Rtree<N, m>::knnSearch(Results&, unsigned, const Point&) const
{
	throw std::runtime_error("k-NN search not implemented");
};


template<class N, unsigned m>
void Rtree<N, m>::deleteTree(N& root, unsigned height)
{
	if (height < 3) {
		return;
	}

	for (auto e : root) {
		deleteTree(e.getNode(), height - 1);
	}

	delete &root;
}

}
