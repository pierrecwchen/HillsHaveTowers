/************************************************************************
BaseState.h										Created by Pavel Szczesny

Base class for creating unique game states
************************************************************************/
#ifndef BASESTATE_H
#define BASESTATE_H

#include <OgreFrameListener.h>
#include <OISEvents.h>
#include <OISKeyboard.h>
#include <OISMouse.h>
#include "../UIManager.h"

namespace Hills
{
	class BaseState
	{
	protected:
		bool m_bInitializing;

	public:
		BaseState();
		~BaseState();
		virtual void Init();
		virtual void Update(const Ogre::FrameEvent& p_evtDelta);
		virtual void Destroy() = 0;
		
		// OIS::KeyListener
		virtual bool KeyPressed( const OIS::KeyEvent& pArg );
		virtual bool KeyReleased( const OIS::KeyEvent& pArg );
		// OIS::MouseListener
		virtual bool MouseMoved( const OIS::MouseEvent& pArg );
		virtual bool MousePressed( const OIS::MouseEvent& pArg, OIS::MouseButtonID pId );
		virtual bool MouseReleased( const OIS::MouseEvent& pArg, OIS::MouseButtonID pId );
	};
}
#endif