#pragma once
#include "spatial/DataObject.hpp"

using Spatial::DataObject;

namespace Rtree
{
	/**
	 * Represents a outbound link from a node.
	 *
	 * This is either a new node or an object id and is thus represented a union
	 * with some simple constructors for convenience.
	 *
	 * @tparam N Node type
	 */
	template<class N>
	union Link {
		using Id = typename DataObject::Id;

		N * node;
		Id id;

		/**
		 * Default constructible.
		 */
		Link() = default;

		/**
		 * Convenience constructor setting the id.
		 */
		Link(Id id) : id(id)
		{
		}

		/**
		 * Convenience constructor setting the node.
		 */
		Link(N * node) : node(node)
		{
		}
	};
}
