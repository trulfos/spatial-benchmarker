#include <criterion/criterion.h>
#include "Mbr.hpp"
#include "Entry.hpp"
#include "Link.hpp"


using namespace Rtree;

/**
 * Genrates data for the other tests.
 */
template<class N>
std::vector<Entry<N>> generateData(unsigned n)
{
	std::vector<Entry<N>> entries (n);

	for (unsigned i = 0; i < n; ++i) {
		Mbr<2> mbr = Box(
				Point {1.0f * i, 3.0f * i},
				Point {2.0f * i, 5.0f * i}
			);

		entries[i] = Entry<N>(mbr, i + 1, typename N::Plugin());
	}

	return entries;
}

/**
 * Check that the node accepts data and gives it back when requested.
 */
template<template<unsigned, unsigned, class> class Node>
void testRetainment()
{
	using N = Node<2, 100, EntryPlugin>;

	// Allocate node to smoke out alignment problems
	N node;

	// Create data
	auto originals = generateData<N>(N::capacity);

	// Add data to node
	for (const Entry<N>& e : originals) {
		node.add(e);
	}

	// Check that they have been retained
	for (unsigned i = 0; i < N::capacity; ++i) {
		cr_expect_eq(
				node[i].getMbr(),
				originals[i].getMbr(),
				"Retrieved entry should be equal to stored"
			);

		cr_expect_eq(
				node[i].getLink().getId(),
				originals[i].getId(),
				"Retrieved entry id should be equal to stored"
			);
	}
}


/**
 * Check that the scan operation does what it is expected to.
 */
template<template<unsigned, unsigned, class> class Node>
void testScanning()
{
	using N = Node<2, 100, EntryPlugin>;

	N node;

	// Create data
	auto originals = generateData<N>(N::capacity);

	// Add data to node
	for (const Entry<N>& e : originals) {
		node.add(e);
	}

	// Do a scan for each original and check the results
	for (const Entry<N>& e : originals) {
		Mbr<2> mbr = e.getMbr();

		// Generate correct results
		std::vector<Link<N>> results;

		for (const Entry<N>& c : originals) {
			if (c.getMbr().intersects(mbr)) {
				results.push_back(c.getLink());
			}
		}

		// Generate node results
		auto scanRange = node.scan(mbr);
		auto rit = results.begin();
		auto sit = scanRange.first;

		while (rit != results.end() && sit != scanRange.second) {
			cr_expect_eq(
					rit->getId(),
					sit->getId(),
					"Results should be equal"
				);

			++rit;
			++sit;
		}

		cr_expect_eq(
				rit,
				results.end(),
				"Too few results from node scan"
			);

		cr_expect_eq(
				sit,
				scanRange.second,
				"Too many results from node scan"
			);
	}
}
