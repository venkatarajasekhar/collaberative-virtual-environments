/**
 * The Interface manager class manages the interface
 */

#ifndef INTERFACEMANAGER_H
#define INTERFACEMANAGER_H

#include "../utilities/types.h"
#include "../utilities/singleton.h"

#include <list>
#include <map>
#include <string>
#include <SDL.h>
#include <agar/core.h>
#include <agar/gui.h>

struct Event
{
    int button;
    int x;
    int y;
    int type;
};

struct Mouse
{
    Point cursorPos;
};

typedef void (*myfunc)(Event* evt);

class XMLFile;

SINGLETON_BEGIN( InterfaceManager )
public:
	InterfaceManager();
	~InterfaceManager();

	/**
	* Reset interface
	*/
    void reset();

	/**
	* Load GUI Sheet
	* Loads a gui from an xml file
	*/
	bool loadGuiSheet(const std::string &filename);

	/**
	* Unload GUI Sheet
	*/
	void unloadGuiSheet();

	/**
	* Add a new window
	*/
	void addWindow(AG_Window *window);

	/**
	* Get Window by name
	*/
    AG_Window* getWindow(const std::string &name);

	/**
	* Remove a window
	*/
	void removeWindow(const std::string &name);

	/**
	* Remove all windows currently added
	*/
	void removeAllWindows();

	/**
	* Show window
	* @param name The name of the window to show
	* @param value Whether to show it or not
	*/
    void showWindow(const std::string &name, bool value);

	/**
	* Draw windows
	*/
	void drawWindows();

	/**
	* Draw name
	*/
    void drawName(const std::string &name, const Point &pt, bool draw);

	/**
	* Get a child
	*/
    AG_Widget* getChild(AG_Widget *parent, const std::string &name);

	/**
	* Set Error Message
	*/
	void setErrorMessage(const std::string &msg);

	/**
	* Show Error Window
	*/
	void showErrorWindow(bool show);

	/**
	* Send to chat window
	*/
	void sendToChat(const std::string &msg);

	/**
	* Add mouse listener
	*/
    void addMouseListener(myfunc func);
    void removeMouseListeners();

    /**
    * Handle mouse event
    * @param button The button clicked
    * @param x The X position of the mouse cursor
    * @param y The Y position of the mouse cursor
    * @param type 0 for mouseup, 1 for mousedown, and 2 for motion
    */
    void handleMouseEvent(int button, int x, int y, int type);

    /**
    * Get Mouse
    * Returns mouse
    * @return Returns pointer to mouse
    */
    Mouse* getMouse() { return mouse; }

private:
	std::list<AG_Window*> mWindows;
	typedef std::list<AG_Window*>::iterator WindowItr;
	std::list<myfunc> mListeners;
	typedef std::list<myfunc>::iterator ListenerItr;
	std::map<std::string, AG_Window*> mNames;
	typedef std::map<std::string, AG_Window*>::iterator NameItr;
	XMLFile *mGuiSheet;
	AG_Window *mErrorWindow;
	AG_Label *mErrorCaption;
	AG_Window *mPlayerWindow;
    Mouse *mouse;

SINGLETON_END()

#endif
