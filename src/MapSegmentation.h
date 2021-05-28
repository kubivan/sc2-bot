#pragma once
#include <SC2.h>

struct MapSegmentation
{
public:
    MapSegmentation(SC2& sc2);
    void segment();

//private:
	SC2 m_sc2;
};

