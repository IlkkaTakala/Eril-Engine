#include <UI/Image.h>
#include "..\include\UI\Text.h"

struct Font
{

};

static std::map<String, Font> fonts;

Font loadFont(String name)
{
	
}

Text::Text()
{
	font = "Assets/Fonts/arial";

	auto it = fonts.find(font);
	if (it == fonts.end()) fonts[font] = loadFont(font);
}

void Text::Render()
{

}
