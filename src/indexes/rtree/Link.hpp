#pragma once
#include "spatial/DataObject.hpp"
#include <cassert>

using Spatial::DataObject;

namespace Rtree
{

	/**
	 * Represents a outbound link from a node.
	 *
	 * This is either a new node or an object id and is thus represented a union
	 * with some simple constructors for convenience.
	 *
	 * Note that this class also stores whether an id or pointer is stored
	 * during debugging, causing its size to be slightly larger.
	 *
	 * @tparam N Node type
	 */
	template<class N>
	class Link {
		public:
			using Id = typename DataObject::Id;


			/**
			 * Get reference to node referenced by this link.
			 */
			N& getNode() const
			{
				assert(!isId);
				return *node;
			}

			/**
			 * Get reference to the id stored in this link
			 */
			const Id& getId() const
			{
				assert(isId);
				return id;
			}


			/**
			 * Default constructible.
			 */
			Link() = default;

#ifdef NDEBUG
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

#else
			Link(Id id) : isId(true), id(id)
			{
			}

			Link(N * node) : isId(false), node(node)
			{
			}


		private:
			bool isId;
#endif

		private:
			union {
				N * node;
				Id id;
			};

	};

}
