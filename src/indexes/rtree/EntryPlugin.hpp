#pragma once
#include "common/DataObject.hpp"

namespace Rtree
{

	/**
	 * Base construction plugin class.
	 *
	 * This can be used to extend the behaviour of entries during construction.
	 * The following contains no implementation and is used as the default
	 * plugin if none is specified.
	 *
	 * @tparam Entry type
	 */
	template<class E>
	class EntryPlugin
	{
		// Dummy class with no data
		class Dummy
		{
		};


		public:

		// Type for node data
		using NodeData = Dummy;

		/**
		 * Construct a new plugin.
		 *
		 * This is used when creating a new entry. The include method will be
		 * called on all child entries later on.
		 *
		 * @param host Entry hosting this plugin
		 */
		EntryPlugin(E& host)
		{
		}


		/**
		 * Construct a new plugin from data object.
		 *
		 * Used when constructing a new entry from a data object.
		 *
		 * @param host Entry hosting this plugin
		 * @param object New data object
		 */
		EntryPlugin(E& host, const DataObject& object)
		{
		}


		/**
		 * Called after the initial entries have been included.
		 *
		 * This happens when a node is created or assigned, in which case a new
		 * plugin is constructed, the include method called for each entry and
		 * finally this method is called.
		 *
		 * @param host Host entry for this plugin
		 */
		void init(E& host)
		{
		}


		/**
		 * Includes an extra entry in the related node.
		 *
		 * This is called whenever a new entry is added to the hosting entry's
		 * node.
		 *
		 * @param host Entry hosting this plugin
		 * @param entry New child entry of host
		 */
		void include(E& host, const E& entry)
		{
		}
	};

};
