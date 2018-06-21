/************************************************************************
TestState.h										Created by Pavel Szczesny

State class for teting out code.
************************************************************************/
#ifndef TESTSTATE_H
#define TESTSTATE_H

#include "BaseState.h"

namespace Hills
{
	class TestState : public BaseState
	{
	public:
		/************************************************************************
		Public Functions
		************************************************************************/
		TestState();
		~TestState();
		virtual void Init();
		virtual void Update(const Ogre::FrameEvent& p_evtDelta);
		virtual void Destroy();

		// OIS::KeyListener
		virtual bool KeyPressed( const OIS::KeyEvent& pArg );
		virtual bool KeyReleased( const OIS::KeyEvent& pArg );
		// OIS::MouseListener
		virtual bool MouseMoved( const OIS::MouseEvent& pArg );
		virtual bool MousePressed( const OIS::MouseEvent& pArg, OIS::MouseButtonID pId );
		virtual bool MouseReleased( const OIS::MouseEvent& pArg, OIS::MouseButtonID pId );

		static BaseState* Create() { return new TestState(); }
	};
}
#endif