/************************************************************************
FrontEndState.h										Created by Pavel Szczesny

Class which houses the main menu
************************************************************************/
#ifndef FRONTENDSTATE_H
#define FRONTENDSTATE_H

#include "BaseState.h"

namespace Hills
{
	class FrontEndState : public BaseState
	{
	public:
		/************************************************************************
		Public Functions
		************************************************************************/
		FrontEndState();
		~FrontEndState();
		virtual void Init();
		virtual void Update(const Ogre::FrameEvent& evt);
		virtual void Destroy();

		// OIS::KeyListener
		virtual bool KeyPressed( const OIS::KeyEvent& pArg );
		virtual bool KeyReleased( const OIS::KeyEvent& pArg );
		// OIS::MouseListener
		virtual bool MouseMoved( const OIS::MouseEvent& pArg );
		virtual bool MousePressed( const OIS::MouseEvent& pArg, OIS::MouseButtonID pId );
		virtual bool MouseReleased( const OIS::MouseEvent& pArg, OIS::MouseButtonID pId );

		static BaseState* Create() { return new FrontEndState(); }
	};
}
#endif