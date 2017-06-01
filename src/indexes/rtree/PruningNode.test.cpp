#include "Node.test.hpp"
#include "PruningNode.hpp"

Test(PruningNode, data_retainment)
{
	testRetainment<PruningNode>();
}


Test(PruningNode, scan)
{
	testScanning<PruningNode>();
}
