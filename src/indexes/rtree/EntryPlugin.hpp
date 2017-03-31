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
		 */
		EntryPlugin() = default;

		/**
		 * Called whenever the host is initialized with a list of entries.
		 *
		 * Called prior to adding the entries, such that the new entries can be
		 * found in the host entry's node.
		 *
		 * @param host Entry hosting this plugin (which new children)
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
