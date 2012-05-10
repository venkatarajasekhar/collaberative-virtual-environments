#include "GUI.h"

#include<gl\glew.h>
#include <SDL.h>
#include <CEGUI.h>
#include <RendererModules\OpenGL\CEGUIOpenGLRenderer.h>
#include <iostream>
	GUI::GUI()
{
	using namespace CEGUI;
	last_time_pulse = 0.001 * static_cast<double>( SDL_GetTicks( ) );
	OpenGLRenderer::bootstrapSystem();
}

	GUI::~GUI()
{
	delete winManager;
	delete rootWin;
	delete layoutWin;
}


void GUI::init()
{
	using namespace CEGUI;
	setResourcePaths( );
	
	SchemeManager::getSingleton( ).create( "OgreTray.scheme" );
	//System::getSingleton( ).setDefaultMouseCursor( "OgreTrayImages", "MouseArrow" );
	
	winManager = &WindowManager::getSingleton( );
	
	//create add the root window to the manager	
	rootWin =  static_cast<DefaultWindow*>( winManager->createWindow( "DefaultWindow", "rootWin" ) );
	System::getSingleton( ).setGUISheet( rootWin );//set the GUI root
	
	layoutWin = static_cast<Window*>( winManager->loadWindowLayout( "OMB.layout" ) );//attaches the layout to the root window parent
	rootWin->addChildWindow( layoutWin );	
}
void GUI::render()
{
	using namespace CEGUI;

	glUseProgram(0);
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE1);


	System::getSingleton().renderGUI();
	injectInput();
	//captureGUI();
}

void GUI::setResourcePaths()
{
	using namespace CEGUI;

	System &sys = System::getSingleton();

	DefaultResourceProvider &defaultResProvider = *static_cast<DefaultResourceProvider*>
										(sys.getResourceProvider() );

	const std::string CEGUIResPath = "../include/CEGUI/datafiles/";

	std::cout << "Using resource directory: " << CEGUIResPath<<"\n";
	
	//*************SET GROUP DIR'S************************
	defaultResProvider.setResourceGroupDirectory( "schemes",  CEGUIResPath + "schemes/" );
	defaultResProvider.setResourceGroupDirectory( "imagesets",  CEGUIResPath + "imagesets/" );
	defaultResProvider.setResourceGroupDirectory( "fonts",  CEGUIResPath + "fonts/" );
	defaultResProvider.setResourceGroupDirectory( "layouts",  CEGUIResPath + "layouts/" );
	defaultResProvider.setResourceGroupDirectory( "looknfeels",  CEGUIResPath + "looknfeel/" );
	defaultResProvider.setResourceGroupDirectory( "lua_scripts",  CEGUIResPath + "lua_scripts/" );
	defaultResProvider.setResourceGroupDirectory( "schemas",  CEGUIResPath + "xml_schemas/" );
	defaultResProvider.setResourceGroupDirectory( "animations",  CEGUIResPath + "animations/" );

	//**********SET GROUPS *****************
	Scheme::setDefaultResourceGroup( "schemes");
	Imageset::setDefaultResourceGroup( "imagesets");
	Font::setDefaultResourceGroup( "fonts");
	WindowManager::setDefaultResourceGroup( "layouts");
	WidgetLookManager::setDefaultResourceGroup( "looknfeels");
	ScriptModule::setDefaultResourceGroup( "lua_scripts");
	AnimationManager::setDefaultResourceGroup( "animations");

	//SET UP XML PARSER
	XMLParser * parser = sys.getXMLParser();
	if( parser->isPropertyPresent(  "SchemaDefaultResourceGroup" ) )
	{
		parser->setProperty ("SchemaDefaultResourceGroup", "schemas" );
	}
}

void GUI::injectInput()
{
	using namespace CEGUI;
	System &sys = System::getSingleton();
	SDL_Event e;

	while(SDL_PollEvent(&e))
	{
		switch(e.type)
		{
//*******MOUSE********************
			case SDL_MOUSEMOTION:
				{
					//we inject the mouse position directly
					float x = static_cast<float>(e.motion.x);
					float y = static_cast<float>(e.motion.y);
					//sys.injectMousePosition( x, y ); //THIS FECKS RENDERING!
				}
				break;

			case SDL_MOUSEBUTTONDOWN:
				injectMouseDown(e.button.button);
				break;

			case SDL_MOUSEBUTTONUP:
				injectMouseUp(e.button.button);
				break;

//*******KEYS**********************
			case SDL_KEYDOWN:
				{
					//to tell CEGUI that a key was pressed, we inject the scancode
					sys.injectKeyDown(e.key.keysym.scancode);
					//as for the character its a little more complicated. we'll use for translated unicode value
					if(e.key.keysym.unicode != 0)
					{
						sys.injectChar(e.key.keysym.unicode);					
					}
				}
				break;

			case SDL_KEYUP:
				sys.injectKeyUp(e.key.keysym.scancode);
				if(e.key.keysym.unicode != 0)
				{
					sys.injectChar(e.key.keysym.unicode);
				}
				break;
			case SDL_QUIT:
				exit(0);
				break;

			default:
				//std::cout<<"DEFAULT CASE: you suck boaby! \n";
				break;
		}
	}
}

void	GUI::injectMouseDown( Uint8 input )
{
	using namespace CEGUI;
	System &sys = System::getSingleton();
	switch( input )
	{
//***BUTTTONS****************
		case SDL_BUTTON_LEFT:
			sys.injectMouseButtonDown(CEGUI::LeftButton);
			std::cout<<"LMB Down\n";
			break;
		
		case SDL_BUTTON_MIDDLE:
			sys.injectMouseButtonDown(CEGUI::MiddleButton);
			std::cout<<"MMB Down\n";
			break;

		case SDL_BUTTON_RIGHT:
			sys.injectMouseButtonDown(CEGUI::RightButton);
			std::cout<<"RMB Down\n";
			break;

//****WHEEL***************** - currently not recognised :S 
		case SDL_BUTTON_WHEELDOWN:
			sys.injectMouseWheelChange( - 1 );
			std::cout<<"MW Up\n";
			break;

		case SDL_BUTTON_WHEELUP:
			sys.injectMouseWheelChange( + 1 );
			std::cout<<"MW Down\n";
			break;
		default:
			std::cout << "handle_mouse_down ignored '" << static_cast<int>( input ) << "'\n" ;
			break;
	}
}

void	GUI::injectMouseUp( Uint8 input )
{
	using namespace CEGUI;

	System &sys = System::getSingleton();

	switch( input )
	{
//***BUTTTONS****************
		case SDL_BUTTON_LEFT:
			sys.injectMouseButtonUp(CEGUI::LeftButton);
			std::cout<<"LMB Up\n";
			break;
		
		case SDL_BUTTON_MIDDLE:
			sys.injectMouseButtonUp(CEGUI::MiddleButton);
			std::cout<<"MMB Up\n";
			break;

		case SDL_BUTTON_RIGHT:
			sys.injectMouseButtonUp(CEGUI::RightButton);
			std::cout<<"RMB Up\n";
			break;
		default:
			std::cout << "handle_mouse_up ignored '" << static_cast<int>( input ) << "'\n" ;
			break;
	}
}

void	GUI::injectTimePulse(double &last_t_pulse)
{
	using namespace CEGUI;

	System &sys = System::getSingleton();

	//get current "run time" in seconds
	double curr_t_pulse = 0.001*SDL_GetTicks();
	//inject the time that passed since the last call
	sys.injectTimePulse(float(curr_t_pulse - last_t_pulse) );
	last_t_pulse = curr_t_pulse;
}

void	GUI::captureGUI()
{
	using namespace CEGUI;
	Window &win = *System::getSingleton().getWindowContainingMouse();//track motion over windows
	
	if( win.getType() == "OgreTray/Button" )//Push button
		capturePButton( win );
	if(win.getType() == "OgreTray/HorizontalScrollbar")
		captureScrollbar( win );
}

void	GUI::capturePButton(CEGUI::Window &win)
{
	using namespace CEGUI;
	PushButton &PB = *static_cast<PushButton*>(&win);
	if(PB.isPushed())
	{		std::cout<<"Win Name: "<<win.getName()<<" was clicked!\n";	}
}

void	GUI::captureScrollbar(CEGUI::Window &win)
{
	using namespace CEGUI;
	Scrollbar &scrollbar = *static_cast<Scrollbar*>(&win);
	std::cout<<"Number of Scrollbar Children: "<<scrollbar.getChildCount()<<"\n";

	Window &thumb = *static_cast<Scrollbar*>(scrollbar.getChild(0));//get slider child
	std::cout<<"Slide scrollbar: "<<scrollbar.getName()<<"\n";
	std::cout<<"Slide Thumb: "<<thumb.getName()<<"\n";
}


