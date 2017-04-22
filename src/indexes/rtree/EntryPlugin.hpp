#pragma once
#include "spatial/DataObject.hpp"

using namespace Spatial;

namespace Rtree
{

	/**
	 * Base entry plugin class.
	 *
	 * This can be used to extend the behaviour of entries during construction.
	 * The following contains no implementation and is used as the default
	 * plugin if none is specified.
	 *
	 * @tparam Entry type
	 */
	class EntryPlugin
	{
		public:

			/**
			 * Dummy class with no data for node storage.
			 */
			class NodeData
			{
			};


			/**
			 * Default constructor.
			 */
			EntryPlugin() = default;


			/**
			 * Construct a new plugin.
			 *
			 * This is used when creating a new entry. The include method will
			 * be called on all child entries later on.
			 *
			 * @param host Entry hosting this plugin
			 */
			template<class E>
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
			template<class E>
			EntryPlugin(E& host, const DataObject& object)
			{
			}


			/**
			 * Called after the initial entries have been included.
			 *
			 * This happens when a node is created or assigned, in which case a
			 * new plugin is constructed, the include method called for each
			 * entry and finally this method is called.
			 *
			 * @param host Host entry for this plugin
			 */
			template<class E>
			void init(E& host)
			{
			}


			/**
			 * Includes an extra entry in the related node.
			 *
			 * This is called whenever a new entry is added to the hosting
			 * entry's node.
			 *
			 * @param host Entry hosting this plugin
			 * @param entry New child entry of host
			 */
			template<class E, class BE>
			void include(E& host, const BE& entry)
			{
			}
	};

};
