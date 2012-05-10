#ifndef GUI_H
#define GUI_H

#include<gl\glew.h>
#include <SDL.h>
#include <CEGUI.h>
#include <RendererModules\OpenGL\CEGUIOpenGLRenderer.h>
#include <iostream>





class GUI
{
	public:
				GUI();
				~GUI();
		void	init();
		void	render();

	private:
		void	setResourcePaths();		
		void	injectInput();
		void	injectMouseDown( Uint8 input );
		void	injectMouseUp( Uint8 input );
		void	injectTimePulse(double &last_t_pulse);
		void	captureGUI();
		void	capturePButton(CEGUI::Window &win);	
		void	captureScrollbar(CEGUI::Window &win);
		
		double last_time_pulse;
		
		CEGUI::WindowManager *winManager;
		CEGUI::Window *rootWin;
		CEGUI::Window *layoutWin;

};

#endif