// Created by Pavel Szczesny
#ifndef TERRAINTESTSTATE_H
#define TERRAINTESTSTATE_H

#include "BaseState.h"

namespace Hills
{
	class TerrainTestState : public BaseState
	{
	public:
		TerrainTestState();
		~TerrainTestState();
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

		static BaseState* Create() { return new TerrainTestState(); }

	private:
		Ogre::Root* m_pRoot;
		Ogre::SceneManager* m_pSceneManager;
		Ogre::Camera* m_pCamera;
		Ogre::Light* m_pSpotlight;
	};
}
#endif