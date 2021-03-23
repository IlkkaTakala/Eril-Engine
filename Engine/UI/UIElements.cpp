#include <list>
#include "UIElements.h"

void Panel::Draw(const int SizeX, const int SizeY, char* Buffer)
{
	if (Location.X > SizeX || Location.Y > SizeY) return;
	int useX = Size.X + Location.X > SizeX ? SizeX : Size.X + Location.X;
	int useY = Size.Y + Location.Y > SizeY ? SizeY : Size.Y + Location.Y;
	for (int y = Location.Y; y < useY; y++) {
		for (int x = Location.X; x < useX; x++) {
			Buffer[y * SizeX + x] = type;
		}
	}
	BeginDraw(SizeX, SizeY, Buffer);
}

void Border::Draw(const int SizeX, const int SizeY, char* Buffer)
{
	if (Location.X > SizeX || Location.Y > SizeY) return;
	int useX = Size.X + Location.X > SizeX ? SizeX : Size.X + Location.X;
	int useY = Size.Y + Location.Y > SizeY ? SizeY : Size.Y + Location.Y;
	for (int y = Location.Y; y < useY; y++) {
		for (int x = Location.X; x < useX; x++) {
			if (y == Location.Y && x == Location.X)
				Buffer[y * SizeX + x] = Data0;
			else if (y == Location.Y && x == useX - 1)
				Buffer[y * SizeX + x] = Data2;
			else if (y == useY - 1 && x == Location.X)
				Buffer[y * SizeX + x] = Data5;
			else if (y == useY - 1 && x == useX - 1)
				Buffer[y * SizeX + x] = Data7;

			else if (y == Location.Y)
				Buffer[y * SizeX + x] = Data1;
			else if (y == useY - 1)
				Buffer[y * SizeX + x] = Data6;

			else if (x == Location.X)
				Buffer[y * SizeX + x] = Data3;
			else if (x == useX - 1)
				Buffer[y * SizeX + x] = Data4;
		}
	}
	BeginDraw(SizeX, SizeY, Buffer);
}

void ElementBase::BeginDraw(const int SizeX, const int SizeY, char* Buffer)
{
	if (Children.size() > 0) {
		std::list<int> Cleared;
		for (int z = 0; ; z++) {
			if (Cleared.size() >= Children.size()) break;
			for (int i = 0; i < Children.size(); i++) {
				if (Children[i]->Z_Order == z) {
					Children[i]->Draw(SizeX, SizeY, Buffer);
					Cleared.push_back(i);
				}
			}
		}
	}
}

void Image::Draw(const int SizeX, const int SizeY, char* Buffer)
{
	if (Location.X > SizeX || Location.Y > SizeY) return;
	int useX = Size.X + Location.X > SizeX ? SizeX : Size.X + Location.X;
	int useY = Size.Y + Location.Y > SizeY ? SizeY : Size.Y + Location.Y;
	int index = 0;
	for (int y = Location.Y; y < useY; y++) {
		for (int x = Location.X; x < useX; x++) {
			Buffer[y * SizeX + x] = Data[index];
			index++;
		}
	}
	BeginDraw(SizeX, SizeY, Buffer);
}

void Text::Draw(const int SizeX, const int SizeY, char* Buffer)
{
	if (Location.X > SizeX || Location.Y > SizeY) return;
	int useX = Size.X + Location.X > SizeX ? SizeX : Size.X + Location.X;
	int useY = Size.Y + Location.Y > SizeY ? SizeY : Size.Y + Location.Y;
	int idx = 0;
	for (int y = Location.Y; y < useY; y++) {
		for (int x = Location.X; x < useX; x++) {
			if (idx > Data.size() - 1) break;
			Buffer[y * SizeX + x] = Data[idx++];
		}
	}
	BeginDraw(SizeX, SizeY, Buffer);
}
