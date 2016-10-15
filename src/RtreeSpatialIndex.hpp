#pragma once
#include "SpatialIndex.hpp"
#include "DataSet.hpp"
/**
 * R-tree algorithms without optimizations.
 *
 * Does not consider disk pages, as this is memory resident anyway.
 */
class RtreeSpatialIndex : public SpatialIndex
{

	public:
		~RtreeSpatialIndex();
		RtreeSpatialIndex(const DataSet& dataSet);

	protected:
		ResultSet rangeSearch(const AxisAlignedBox& box) const;
		ResultSet knnSearch(unsigned k, const Point& point) const;

	private:
		struct Node;

		struct Entry {
			Entry(DataObject::Id id);
			union {
				DataObject::Id id;
				Node * child;
			};

			float mbr[];
		};

		struct Node {
			void * operator new (size_t size, unsigned d, unsigned capacity);
			short unsigned nEntries;
			Entry entries[];
			// A list of entries follow
		};


		Node * root;
		unsigned maxChildren = 10;
		unsigned dimension;
		unsigned nObjects;
		unsigned height;

		void insert(const DataObject& object, Node * root);
};
