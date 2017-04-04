#pragma once
#include "spatial/Results.hpp"
#include "spatial/Box.hpp"
#include "spatial/StatsCollector.hpp"
#include "Link.hpp"

using Spatial::Results;
using Spatial::Box;
using Spatial::StatsCollector;

namespace Rtree
{
	/**
	 * R-tree constructed by transforming another R-tree.
	 *
	 * This R-tree decorator relies on constructing a new R-tree from another
	 * R-tree, where the new R-tree may have entirely different nodes which are
	 * scanned in more or less peculiar ways.
	 *
	 * The new R-tree is constructed by transforming each node in the original
	 * R-tree which it wraps. This is done by constructing the new tree bottom
	 * up, adding the MBRs and object ids from the old nodes to the new ones.
	 *
	 * Note that the concept of entries do not necessarily exist in the
	 * resulting tree since this is a detail of the node implementation.
	 *
	 * @tparam R Original R-tree (to decorate)
	 * @tparam N New node type for this R-tree
	 */
	template<class R, class N>
	class TransformingRtree : public R
	{
		using OldEntry = typename R::E;

		public:
			// These are assumed to exist (depends on template parameter R)
			using R::getRoot;
			using R::getHeight;


			// Inherit constructor(s)
			using R::R;


			/**
			 * Deletes allocated nodes.
			 */
			virtual ~TransformingRtree();


			/**
			 * Constructs the new R-tree from the parent R-tree.
			 *
			 * This should be run before performing any search through this tree
			 * since the search will only use the new R-tree.
			 */
			void prepare();


			/**
			 * Collects tree statistics.
			 *
			 * Includes statistics for the transformed tree in addition to the
			 * original R-tree.
			 *
			 * @return Statistics collected
			 */
			StatsCollector collectStatistics() const override;



			/**
			 * Range search with an algorithm partly specified by the node.
			 *
			 * The node is responsible for scanning through its entries, thus
			 * this only provides the depth-first algorithm.
			 *
			 * @param box Query box
			 */
			Results rangeSearch(const Box& box) const override;


			/**
			 * Instrumented range search collecting statistics along the way.
			 *
			 * As for range search without instrumentation, this also delegates
			 * a lot of the work to the node itself, which is also allowed to
			 * collect statistics.
			 *
			 * @param box Query box
			 * @param stats Object in which statistics should be collected
			 */
			Results rangeSearch(
					const Box& box,
					StatsCollector& stats
				) const override;


		private:

			/**
			 * Root for the new R-tree.
			 */
			Link<N> newRoot = nullptr;


			/**
			 * Height of the new R-tree.
			 *
			 * This must be stored as the two trees are not necessarily
			 * synchronized.
			 */
			unsigned newHeight = 0;


			/**
			 * Transform a subtree of the old type to the new type.
			 *
			 * @param subtree Root node of subtree to transform
			 * @param height Height of subtree
			 */
			static Link<N> transform(OldEntry& subtree, unsigned height);


			/**
			 * Deletes the (new) entire tree.
			 *
			 * Traverse the tree and delete all nodes on the way.
			 *
			 * @param root Root node of tree to delete
			 * @param height Height of given tree
			 */
			static void deleteTree(N * root, unsigned height);
	};

}


#include "TransformingRtree.cpp"
