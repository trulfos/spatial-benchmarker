#include "Node.test.hpp"
#include "VectorizedNode.hpp"

Test(VectorizedNode, data_retainment)
{
	testRetainment<VectorizedNode>();
}


Test(VectorizedNode, scan)
{
	testScanning<VectorizedNode>();
}
