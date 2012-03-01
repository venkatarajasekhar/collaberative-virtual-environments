#include "interfacemanager.h"

#include "../utilities/input.h"
//#include "../graphics/camera.h"
#include "../graphics/graphics.h"
#include "../utilities/xml.h"

#include <agar/core.h>
#include <agar/gui.h>

#include <iostream>


InterfaceManager::InterfaceManager()
{
	GraphicsEngine& graphicsEngine = GraphicsEngine::GetSingleton();

    if (AG_InitCore("Oh My Buddha!", 0) == -1)
		std::cerr << "Error: Unable to init agar.\n";

    if (AG_InitVideoSDL(graphicsEngine.getSurface(), AG_VIDEO_OPENGL_OR_SDL) == -1)
		std::cerr << "Error: Unable to init video.\n";

	mGuiSheet = 0;
	int halfScreenWidth = (int)(graphicsEngine.getScreenWidth() * 0.5);
	mErrorWindow = AG_WindowNewNamed(0, "Error");
	AG_WindowSetCaption(mErrorWindow, "Error");
	AG_WindowSetGeometry(mErrorWindow, halfScreenWidth - 150, 50, 300, 75);

	mErrorCaption = AG_LabelNewString(mErrorWindow, 0, "");
	AG_LabelSizeHint(mErrorCaption, 1, "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
	AG_LabelJustify(mErrorCaption, AG_TEXT_CENTER);

	mPlayerWindow = AG_WindowNewNamed(AG_WINDOW_NOMOVE|AG_WINDOW_PLAIN|AG_WINDOW_NOBUTTONS, "Player");
	AG_Label *label = AG_LabelNewString(mPlayerWindow, 0, "");
	AG_LabelSizeHint(label, 1, "XXXXXXXXXXXX");
	AG_LabelJustify(label, AG_TEXT_CENTER);

    mouse = new Mouse;
	mouse->cursorPos.x = 0;
	mouse->cursorPos.y = 0;

	reset();
}

InterfaceManager::~InterfaceManager()
{
	delete mouse;
	AG_ObjectDetach(mErrorWindow);
	removeAllWindows();
	AG_DestroyVideo();
	AG_Destroy();
}

void InterfaceManager::reset()
{
	GraphicsEngine& graphicsEngine = GraphicsEngine::GetSingleton();

    AG_ResizeDisplay(graphicsEngine.getScreenWidth(), graphicsEngine.getScreenHeight());
}

bool InterfaceManager::loadGuiSheet(const std::string &filename)
{
	if (mGuiSheet == 0)
		mGuiSheet = new XMLFile;

    // Check it opened successfully
    if (!mGuiSheet->load(filename))
    {
        return false;
    }

    return true;
}

void InterfaceManager::unloadGuiSheet()
{
}

void InterfaceManager::addWindow(AG_Window *window)
{
	mWindows.push_back(window);
}

AG_Window* InterfaceManager::getWindow(const std::string &name)
{
    return static_cast<AG_Window*>(AG_WidgetFind(agDriverSw, name.c_str()));
}

void InterfaceManager::removeWindow(const std::string &name)
{
	AG_Window *win = getWindow(name);
	AG_ObjectDetach(win);
	mWindows.remove(win);
}

void InterfaceManager::removeAllWindows()
{
	WindowItr itr_end = mWindows.end();
	for (WindowItr itr = mWindows.begin(); itr != itr_end; ++itr)
	{
		AG_ObjectDetach(*itr);
	}
	mWindows.clear();
    NameItr name_itr = mNames.begin(), name_itr_end = mNames.end();
    while (name_itr != name_itr_end)
    {
        AG_ObjectDestroy(name_itr->second);
        ++name_itr;
    }
    mNames.clear();
}

void InterfaceManager::showWindow(const std::string &name, bool value)
{
	AG_Window *win = getWindow(name);
	if (win)
	{
        value ? AG_WindowShow(win) : AG_WindowHide(win);
	}
}

void InterfaceManager::drawWindows()
{
	AG_Window *win;
	AG_FOREACH_WINDOW(win, agDriverSw)
	{
		AG_ObjectLock(win);
		AG_WindowDraw(win);
		AG_ObjectUnlock(win);
	}
}

void InterfaceManager::drawName(const std::string &name, const Point &pt, bool draw)
{
	if (!draw)
	{
	    if (AG_WindowIsVisible(mPlayerWindow))
            AG_WindowHide(mPlayerWindow);
	}

	if (!AG_WindowIsVisible(mPlayerWindow))
        AG_WindowShow(mPlayerWindow);

//    Point camPt = graphicsEngine->getCamera()->getPosition();

//	AG_WindowSetGeometry(mPlayerWindow, pt.x - camPt.x, pt.y - 5 - camPt.y, 75, 20);
	// TODO: Set Label to name
}

AG_Widget* InterfaceManager::getChild(AG_Widget *parent, const std::string &name)
{
	AG_Widget *widget;
	std::string str = agDrivers.archivePath;
	str.append(name);
	AGOBJECT_FOREACH_CHILD(widget, parent, ag_widget)
    {
        char widgetName[64];
        AG_ObjectCopyName(widget, widgetName, 30);
        if (strncmp(widgetName, str.c_str(), str.size()) == 0)
        {
            return widget;
        }
    }

    return NULL;
}

void InterfaceManager::setErrorMessage(const std::string &msg)
{
	AG_LabelString(mErrorCaption, msg.c_str());
}

void InterfaceManager::showErrorWindow(bool show)
{
	if (show)
	{
		AG_WindowShow(mErrorWindow);
		AG_WindowFocus(mErrorWindow);
	}
	else
	{
		AG_WindowHide(mErrorWindow);
	}
}

void InterfaceManager::sendToChat(const std::string &msg)
{
	int lines = 0;
	const unsigned int lineSize = 50;

    // find how many lines are needed
	lines = msg.size() / lineSize;
	if ((msg.size() % lineSize) > 0)
		++lines;

    AG_Console *chat = static_cast<AG_Console*>(AG_WidgetFind(agDriverSw, "/ChatWindow/ChannelsFolder/GlobalChat/Chat"));
    if (chat)
    {
        unsigned int pos = 0;
        unsigned int npos = msg.size();

        // check if the size of the message is greater than the max number of lines allowed
        if (npos > lineSize)
            npos = lineSize;

        // cycle through each line and output to console each line
        for (int line = 0; line < lines; ++line)
        {
            AG_ConsoleAppendLine(chat, msg.substr(pos, npos).c_str());
            pos += npos;
            if (pos + npos > msg.size())
                npos = msg.size() - pos;
        }
    }
}

void InterfaceManager::addMouseListener(myfunc func)
{
	mListeners.push_back(func);
}

void InterfaceManager::removeMouseListeners()
{
	mListeners.clear();
}

void InterfaceManager::handleMouseEvent(int button, int x, int y, int type)
{
    ListenerItr itr = mListeners.begin(), itr_end = mListeners.end();
    while (itr != itr_end)
    {
        Event evt;
        evt.button = button;
        evt.x = x;
        evt.y = y;
        evt.type = type;
        (*(*itr))(&evt);

        ++itr;
    }
}