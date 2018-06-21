//-----------------------------------------------------------------------------
// Iron.h                                                   By: Keenan Johnston
// 
// Represents an outcrop of iron.  Derived from BaseResource.
//-----------------------------------------------------------------------------

#ifndef IRON_H
#define IRON_H

#include "BaseResource.h"

class Iron : public BaseResource
{
	public:
		friend class ResourceManager;

        void Harvest();
        inline ResourceTypes GetType() { return m_eType; }

	private:
		Iron(Tile* p_pTile, int p_iAmount);
};

#endif