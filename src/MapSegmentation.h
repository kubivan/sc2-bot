#pragma once
#include <SC2.h>

class MapSegmentation
{
public:
	MapSegmentation(SC2& sc2);
	void segment();

private:
	SC2 m_sc2;
};

