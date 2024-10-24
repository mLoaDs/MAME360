#include "GFilterManager.h"

CGFilterManager::CGFilterManager(void)
{
	m_CurrentFilter = eftNone;
	InitFilterMap();
}

CGFilterManager::~CGFilterManager(void)
{
}

//-------------------------------------------------------------
//	InitFilterList
//
// This function is called from the constructor to build our
// internal map of possible filters.  
//-------------------------------------------------------------
void CGFilterManager::InitFilterMap()
{
	SFilterInfo oFilter;

	// The default filter (does no scaling/filtering)
	lstrcpyW(oFilter.m_strName, L"None");
	oFilter.m_FilterType = eftNone;
	oFilter.m_dwMagnificatonLvl = 1;
	oFilter.m_filterFunction16 = NULL;
	oFilter.m_filterFunction32 = NULL;
	m_mapFilters[eftNone] = oFilter;

 
}

void CGFilterManager::SetActiveFilter(EFilterType filterType)
{
	m_CurrentFilter = filterType;
}

void CGFilterManager::SetBitmapDepth(UINT32 depth)
{
	m_bIs16Bit = (depth == 16);
}

void CGFilterManager::FilterBitmap(UINT8* dest, UINT8* src, UINT32 origRowBytes, int width, int height)
{
	// The caller shouldn't ever call us with a filter type of none
	// since it does nothing and is wasteful to call the filter
	// function, just return if they do
	if (m_CurrentFilter == eftNone)
	{
		return;
	}
	else
	{
		if (m_bIs16Bit)
		{
			m_mapFilters[m_CurrentFilter].m_filterFunction16(src, 
				origRowBytes, NULL, dest, origRowBytes*2, width, height);
		}
		else
		{
			m_mapFilters[m_CurrentFilter].m_filterFunction32(src, 
				origRowBytes, NULL, dest, origRowBytes*2, width, height);
		}
	}
}