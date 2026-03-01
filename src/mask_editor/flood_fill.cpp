#include "flood_fill.h"
#include <stack>

void clearMask(Mask& mask) {
	for(int y = 0; y<mask.HEIGHT;y++) {
		for(int x = 0; x < mask.WIDTH; x++) {
			mask.setSolid(y,x,false);
		}
	}
}


void floodFill(Mask& mask, int startY, int startX, bool targetValue, bool newValue) {
	if(mask.isSolid(startY, startX) != targetValue) {
		return;
	}

	std::stack<std::pair<int,int>> stack;
	stack.push({startY, startX});

	while(!(stack.empty()))  {
		auto [y,x] = stack.top();
		stack.pop();
		
		if(y<0 || y >= mask.HEIGHT ||  x < 0 || x > mask.WIDTH) {
			continue;
		
		}

		if(mask.isSolid(y,x) != targetValue) {
			continue;
		
		}

	 mask.setSolid(y,x,newValue);
	 stack.push({y + 1, x});  // вниз
         stack.push({y - 1, x});  // вверх
         stack.push({y, x + 1});  // вправо
         stack.push({y, x - 1});  // влево

	 }



}
