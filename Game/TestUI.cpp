#include "TestUI.h"
#include <UI/Panel.h>

void TestUI::Construct()
{
	AddComponent(
		Create<Panel>()
	);
}
