#include "languagestate.h"

#include "../utilities/input.h"
#include "../game.h"
#include "../graphics/graphics.h"
#include "../interface/interfacemanager.h"
//#include "net/networkmanager.h"
#include "../utilities/xml.h"
#include "../utilities/types.h"
#include "connectstate.h"

#include <agar/core.h>
#include <agar/gui.h>

void submit_language(AG_Event *event)
{
	SINGLETON_GET(Game, game)

    std::string language;
    char *lang = AG_STRING(1);

    XMLFile file;

	GET_PATH("\\Data\\Settings\\", path)

    if (file.load(path + "omb.cfg"))
    {
        language = lang;
        file.addString("language", "value", language);
        file.save();
    }

	// TODO : Does lang get cleaned up? Possible mem leak.

    game.setLanguage(language);
    GameState *state = new ConnectState;
    game.changeState(state);
}

LanguageState::LanguageState()
{
}

void LanguageState::enter()
{
	SINGLETON_GET(GraphicsEngine, graphicsEngine)
	SINGLETON_GET(InterfaceManager, interfaceManager)

	int screenWidth = graphicsEngine.getScreenWidth();
	int screenHeight = graphicsEngine.getScreenHeight();
	int halfScreenWidth = screenWidth / 2;
	int halfScreenHeight = screenHeight / 2;

	const int numberOfLanguages = 3;	// Change this as we add more languages

	// create window for entering username and password
	AG_Window *win = AG_WindowNew(AG_WINDOW_PLAIN|AG_WINDOW_KEEPBELOW);
	AG_WindowShow(win);
	AG_WindowMaximize(win);
    interfaceManager.addWindow(win);

    // Load windows from file
    XMLFile file;
    std::string name;
    std::string title;
    int w, h;

	GET_PATH("\\Data\\Settings\\", path)

    if (file.load(path + "language.xml"))
    {
        std::string lang[numberOfLanguages];
        std::string value[numberOfLanguages];
        std::string icon[numberOfLanguages];

        file.setElement("window");
        name = file.readString("window", "name");
        title = file.readString("window", "title");
        w = file.readInt("window", "width");
        h = file.readInt("window", "height");

        file.setSubElement("button");
		lang[0] = file.readString("button", "text");
		value[0] = file.readString("button", "value");
		icon[0] = file.readString("button", "icon");
		//file.nextSubElement("button");
		//lang[1] = file.readString("button", "text");
		//value[1] = file.readString("button", "value");
		//icon[1] = file.readString("button", "icon");
		//file.nextSubElement("button");
		//lang[2] = file.readString("button", "text");
		//value[2] = file.readString("button", "value");
		//icon[2] = file.readString("button", "icon");

		for( int i = 1; i < numberOfLanguages; i++)
		{
			file.nextSubElement("button");
			lang[i] = file.readString("button", "text");
			value[i] = file.readString("button", "value");
			icon[i] = file.readString("button", "icon");
		}

		file.close();

		AG_Window *test = AG_WindowNewNamed(AG_WINDOW_NOBUTTONS|AG_WINDOW_KEEPABOVE, name.c_str());
        AG_WindowSetCaption(test, title.c_str());
        AG_WindowSetSpacing(test, 12);
        AG_WindowSetGeometry(test, halfScreenWidth - (w / 2) , halfScreenHeight - (h / 2), w, h);

        AG_Button *button[numberOfLanguages];

        AG_HBox *box = AG_HBoxNew(test, 0);
        for (int i = 0; i < numberOfLanguages; ++i)
        {
            button[i] = AG_ButtonNewFn(box, 0, lang[0].c_str(), submit_language, "%s", strdup(value[i].c_str()));
            AG_Surface *surface;
            SDL_Surface *s = graphicsEngine.loadSDLTexture(path + icon[i].c_str());
            SDL_LockSurface(s);
			#if SDL_BYTEORDER == SDL_BIG_ENDIAN
				surface = AG_SurfaceFromPixelsRGBA(s->pixels, s->w, s->h, s->format->BitsPerPixel, s->format->Rmask, s->format->Bmask, s->format->Gmask, s->format->Amask);
			#else
				surface = AG_SurfaceFromPixelsRGBA(s->pixels, s->w, s->h, s->format->BitsPerPixel, s->format->Rmask, s->format->Gmask, s->format->Bmask, s->format->Amask);
			#endif
			SDL_UnlockSurface(s);
            if (surface)
                AG_ButtonSurface(button[i], surface);
        }

        AG_WindowShow(test);
        interfaceManager.addWindow(test);
    }
    else
    {
        // XML file wasnt found, load default
        AG_Window *test = AG_WindowNewNamed(AG_WINDOW_NOBUTTONS|AG_WINDOW_KEEPABOVE, "Language");
        AG_WindowSetCaption(test, "Choose Language");
        AG_WindowSetSpacing(test, 12);
        AG_WindowSetGeometry(test, halfScreenWidth - 125, halfScreenHeight - 45, 225, 135);

        AG_Button *button[3];

        button[0] = AG_ButtonNewFn(test, 0, "English", submit_language, "%s", "en");
        button[1] = AG_ButtonNewFn(test, 0, "Portugues", submit_language, "%s", "pt");
        button[2] = AG_ButtonNewFn(test, 0, "Espa\xc3\xb1ol", submit_language, "%s", "es");

        AG_WindowShow(test);
        interfaceManager.addWindow(test);
    }

}

void LanguageState::exit()
{
	SINGLETON_GET(InterfaceManager, interfaceManager)

	interfaceManager.removeAllWindows();
}

bool LanguageState::update()
{
	SINGLETON_GET(InputManager, inputManager)

    // Check for input, if escape pressed, exit
	if (inputManager.getKey(AG_KEY_ESCAPE))
		return false;

	SDL_Delay(0);

	return true;
}