#pragma once
#include "spatial/DataObject.hpp"
#include "Mbr.hpp"
#include "EntryPlugin.hpp"
#include <algorithm>

namespace Rtree
{

	/**
	 * An entry in a node of the R-tree.
	 *
	 * Each entry works like a handle - it contains the MBR of a node and a
	 * pointer to the node itself. The MBR is separated from the node to avoid
	 * loading the an entire node when only the MBRs should be scanned.
	 *
	 * Note that the node may actually store entries in an entirely different
	 * format, but is requried to be able to reconstruct the entries stored at
	 * request.
	 *
	 * @tparam N Node type for link pointer
	 * @tparam D Derived class (for static polymorphism)
	 */
	template<class N>
	class BaseEntry
	{
		public:
			using Node = N;
			using Mbr = typename N::Mbr;
			using Link = typename N::Link;
			using Plugin = typename N::Plugin;

			/**
			 * Assignment operator.
			 *
			 * This is necessary to make different subclasses assignable to each
			 * other. It simply copies all fields using the getters and settes.
			 */
			BaseEntry& operator=(const BaseEntry& other)
			{
				setMbr(other.getMbr());
				setLink(other.getLink());
				setPlugin(other.getPlugin());

				return *this;
			}

			/**
			 * Inculdes the given entry in this entry.
			 *
			 * This typically updates the MBR and plugin. Useful when adjusting
			 * MBRs in the tree during insertion.
			 *
			 * @param entry New entry to include
			 */
			void include(const BaseEntry& entry)
			{
				setMbr(getMbr() + entry.getMbr());

				// TODO: Simplify this!
				Plugin p = getPlugin();
				p.include(*this, entry);

				setPlugin(p);
			}


			/**
			 * Includes the given range of entries.
			 *
			 * Equivalent to calling include with each entry in the range.
			 *
			 * @param first Iterator to first entry
			 * @param last Iterator to past-the-end entry
			 */
			template<class InputIterator>
			void include(InputIterator first, InputIterator last)
			{
				// Cache copy of fields as they may be expensive to get
				Mbr m = getMbr();
				Plugin p = getPlugin();

				while (first != last) {
					m += first->getMbr();
					p.include(*this, *first);
					++first;
				}

				setMbr(m);
				setPlugin(p);
			}

			/**
			 * Recalculate the MBR and plugin for the associated node.
			 *
			 * When the node has changed, this can be used to refresh the book
			 * keeping information stored in this entry. This is more expensive
			 * than the add method, but is (currently) the only option if
			 * entries have been removed from the node.
			 *
			 * Note that the plugin is notified about the recalculation (by
			 * calling the init method) as if the entry was reconstructed.
			 */
			void recalculate()
			{
				N& node = getNode();
				assert(node.getSize() > 0);
				setMbr(node[0].getMbr());
				setPlugin(Plugin(*this));

				include(node.begin(), node.end());

				Plugin p = getPlugin();
				p.init(*this);
				setPlugin(p);
			}


			/**
			 * Convenience method for getting the node of this entry's link.
			 *
			 * @return Node associated with link
			 */
			N& getNode() const
			{
				return getLink().getNode();
			}


			/**
			 * Convenience method for getting the id of this entry's link.
			 */
			typename Link::Id getId() const
			{
				return getLink().getId();
			}


			/**
			 * Retrieve the MBR of this entry.
			 *
			 * @return MBR of this entry.
			 */
			virtual const Mbr getMbr() const = 0;


			/**
			 * Get the link associated with this entry.
			 *
			 * @return Link of this entry
			 */
			virtual const Link getLink() const = 0;


			/**
			 * Get the plugin of this entry.
			 *
			 * @return Plugin of this entry
			 */
			virtual const Plugin getPlugin() const = 0;

		protected:

			virtual void setMbr(const Mbr& m) = 0;

			virtual void setPlugin(const Plugin& p) = 0;

			virtual void setLink(const Link& l) = 0;
	};


	/**
	 * Default entry with immediate storage.
	 *
	 * @see Rtree::BaseEntry
	 */
	template<class N>
	class Entry : public BaseEntry<N>
	{
		using Base = BaseEntry<N>;

		public:
			// Types
			using Mbr = typename Base::Mbr;
			using Link = typename Base::Link;
			using Plugin = typename Base::Plugin;

			// Depends on template parameter
			using Base::include;
			using Base::recalculate;


			/**
			 * Create an invalid entry.
			 */
			Entry() = default;


			/**
			 * Construct a new entry from the data it should contain.
			 *
			 * @param mbr MBR of new entry
			 * @param link Pointer to node or object id
			 * @param plugin Initial plugin value
			 */
			Entry(const Mbr& mbr, const Link& link, const Plugin& plugin)
				: mbr(mbr), link(link), plugin(plugin)
			{
			}


			/**
			 * Create a new entry from the given object, passing a set of
			 * arguments to the plugin.
			 *
			 * @param object Data object
			 * @param args Arguments to forward
			 */
			template<class ...Args>
			Entry(const DataObject& object, Args ...args)
				: mbr(object.getBox()), link(object.getId()),
					plugin(*this, object, std::forward<Args...>(args...))
			{
			};


			/**
			 * Creates a new entry from the given object.
			 *
			 * The plugin is default initialized.
			 *
			 * @param object Object to grab MBR and id from
			 */
			Entry(const DataObject& object)
				: mbr(object.getBox()), link(object.getId()),
					plugin(*this, object)
			{
			}


			/**
			 * Copy constructor generalized for any entry type.
			 *
			 * @param other Entry to copy
			 */
			template<class E>
			Entry(const E& other)
				: mbr(other.getMbr()), link(other.getLink()),
					plugin(other.getPlugin())
			{
			}


			/**
			 * Create an entry based on a node.
			 *
			 * @param node Node with entries
			 */
			Entry(N& node) : link(&node), plugin(*this)
			{
				recalculate();
			}


			Entry(N * node)
				: Entry(*node)
			{
			}


			template<class E>
			Entry& operator=(const E& other)
			{
				setMbr(other.getMbr());
				setLink(other.getLink());
				setPlugin(other.getPlugin());

				return *this;
			}



			const Mbr getMbr() const override
			{
				return mbr;
			}


			const Link getLink() const override
			{
				return link;
			}


			const Plugin getPlugin() const override
			{
				return plugin;
			}

		protected:
			void setMbr(const Mbr& m) override
			{
				mbr = m;
			}

			void setPlugin(const Plugin& p) override
			{
				plugin = p;
			}

			void setLink(const Link& l) override
			{
				link = l;
			}

		private:
			Mbr mbr;
			Link link;
			Plugin plugin;
	};

	//TODO: Don't think we need this?
	template<class N>
	void swap(Entry<N>& a, Entry<N>& b)
	{
		Entry<N> tmp;
		tmp = a;
		a = b;
		b = tmp;
	}
}
