//-----------------------------------------------------------------------------
// Stone.h                                                  By: Keenan Johnston
// 
// Represents an outcrop of stone.  Derived from BaseResource.
//-----------------------------------------------------------------------------

#ifndef STONE_H
#define STONE_H

#include "BaseResource.h"

class Stone : public BaseResource
{
	public:
		friend class ResourceManager;

        void Harvest();
        inline ResourceTypes GetType() { return m_eType; }

	private:
		Stone(Tile* p_pTile, int p_iAmount);
};

#endif