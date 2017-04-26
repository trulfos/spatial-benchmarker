#include "PapiReporter.hpp"
#include "ProgressLogger.hpp"
#include <random>
#include <algorithm>
#include <papi.h>

namespace Bench
{

	/**
	 * Check the PAPI status code given and throw an appropriate exception if
	 * the code is an error.
	 *
	 * @param code Code to check
	 */
	void check(int code)
	{
		if (code != PAPI_OK) {
			throw std::runtime_error(
					std::string("PAPI error! ") + PAPI_strerror(code)
				);
		}
	}


	void PapiReporter::run(
			const SpatialIndex& index,
			std::ostream& logStream
		)
	{
		ProgressLogger progress(logStream, RUNS);
		std::default_random_engine engine (11);

		auto querySet = getQuerySet();

		// Load queries into memory
		std::vector<RangeQuery> queries (querySet.getSize());
		std::transform(
				querySet.begin(), querySet.end(),
				queries.begin(),
				[](const decltype(querySet)::value_type& query) {
					return query;
				}
			);

		// Set up performance counters (move to constructor?)
		std::array<int, 3> events = {
				PAPI_TOT_INS,
				PAPI_L3_TCM,
				PAPI_RES_STL
			};

		int nHardwareCounters = PAPI_num_counters();
		if (nHardwareCounters < int(events.size())) {
			throw std::runtime_error("Too few hardware counters");
		}


		// Test multiple times
		for (unsigned i = 0; i < RUNS; ++i) {

			// Clear cache
			clearCache();

			// Make space for results
			std::array<int long long, events.size()> results;


			// Start measurements
			check(PAPI_start_counters(events.data(), events.size()));
			int long long startTime = PAPI_get_real_nsec();

			// Run the code to 
			for (const RangeQuery& query : queries) {
				index.search(query);
			}

			// Stop measurements
			int long long endTime = PAPI_get_real_nsec();
			check(PAPI_stop_counters(results.data(), results.size()));

			// Store metrics
			for (unsigned i = 0; i < events.size(); ++i) {
				char name[PAPI_MAX_STR_LEN];
				if (PAPI_event_code_to_name(events[i], name) != PAPI_OK) {
					throw std::runtime_error("Could not translate event name");
				}

				addEntry(std::string(name), results[i]);
			}

			addEntry("PAPI_REAL_NSEC", endTime - startTime);

			// Rearrange queries
			std::shuffle(
					queries.begin(), queries.end(),
					engine
				);

			// Update progress bar
			progress.increment();

		}
	}

}
