#include "pch.h"
#include "Node.h"

ValueNode::ValueNode(EVT type, const String& s)
{
	value = Value(type, s);
}
