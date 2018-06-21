//-----------------------------------------------------------------------------
// Wood.h                                                   By: Keenan Johnston
// 
// Represents a tree.  Derived from BaseResource.
//-----------------------------------------------------------------------------

#ifndef WOOD_H
#define WOOD_H

#include "BaseResource.h"

class Wood : public BaseResource
{
	public:
		friend class ResourceManager;

        void DestroyModel();

        void Harvest();
        inline ResourceTypes GetType() { return m_eType; }

	private:
        Model* m_pModel2;

		Wood(Tile* p_pTile, int p_iAmount);
};

#endif