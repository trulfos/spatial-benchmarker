#include "Node.test.hpp"
#include "FullScanNode.hpp"

Test(FullScanNode, data_retainment)
{
	testRetainment<FullScanNode>();
}


Test(FullScanNode, scan)
{
	testScanning<FullScanNode>();
}
