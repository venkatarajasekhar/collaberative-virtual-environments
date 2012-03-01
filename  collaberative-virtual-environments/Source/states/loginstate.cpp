#include "loginstate.h"

#include "connectstate.h"
#include "scenestate.h"
//#include "optionsstate.h"
#include "../utilities/input.h"
#include "../game.h"

//#include "net/networkrkmanager.h"
//#include "net/packet.h"
//#include "net/protocol.h"

#include "../interface/interfacemanager.h"
#include "../graphics/graphics.h"
//#include "utilities/crypt.h"
#include "../utilities/xml.h"
#include "../utilities/types.h"

#include <SDL.h>
#include <iostream>

void goto_options(AG_Event *event)
{
  //  GameState *state = new OptionsState;
 //   game->changeState(state);
}

void submit_login(AG_Event *event)
{
	SINGLETON_GET(InterfaceManager, interfaceManager)
	SINGLETON_GET(Game, game)

    std::string username;
	std::string password;

	AG_Textbox *txtName = static_cast<AG_Textbox*>(AG_PTR(1));
	AG_Textbox *txtPass = static_cast<AG_Textbox*>(AG_PTR(2));
	AG_Checkbox *checkBox = static_cast<AG_Checkbox*>(AG_PTR(3));
	XMLFile *file = static_cast<XMLFile*>(AG_PTR(4));

	if (txtName && txtPass)
	{
	    username = AG_TextboxDupString(txtName);
	    password = AG_TextboxDupString(txtPass);
	}

    if (!username.empty() && !password.empty())
    {
        //player->setName(username);
        //Packet *packet = new Packet(PAMSG_LOGIN);
        //packet->setString(username);
        //// use encrypted password
        //packet->setString(encryptPassword(username+password));
        //netxtPassrkManager->sendPacket(packet);

		game.mName = username;

		if (checkBox)
		{
			file->changeInt("login", "save", checkBox->state);
			if (checkBox->state != 0)
			{
				file->changeString("login", "username", username);
			}
			file->save();
		}

        interfaceManager.setErrorMessage("");
        interfaceManager.showErrorWindow(false);

		GameState *state = new TestState;
		game.changeState(state);
    }
    else
    {
        interfaceManager.setErrorMessage("Invalid username or password entered.");
        interfaceManager.showErrorWindow(true);
    }
}

void submit_register(AG_Event *event)
{
    std::string username;
	std::string password;

	AG_Textbox *txtName = static_cast<AG_Textbox*>(AG_PTR(1));
	AG_Textbox *txtPass = static_cast<AG_Textbox*>(AG_PTR(2));

	if (txtName && txtPass)
	{
	    username = AG_TextboxDupString(txtName);
	    password = AG_TextboxDupString(txtPass);
	}
/*
    if (!username.empty() && !password.empty())
    {
        player->setName(username);
        Packet *packet = new Packet(PAMSG_REGISTER);
        packet->setString(username);
        packet->setString(encryptPassword(username+password));
        netxtPassrkManager->sendPacket(packet);

        interfaceManager->setErrorMessage("");
        interfaceManager->showErrorWindow(false);
    }
    else
    {
        interfaceManager->setErrorMessage("Invalid username or password entered.");
        interfaceManager->showErrorWindow(true);
    }*/
}

void switch_login_window(AG_Event *event)
{
    AG_Window *txtName = static_cast<AG_Window*>(AG_PTR(1));
    if (txtName)
        AG_WindowHide(txtName);

    AG_Window *txtPass = static_cast<AG_Window*>(AG_PTR(2));
    if (txtPass)
        AG_WindowShow(txtPass);
}

LoginState::LoginState()
{
    file = 0;
}

void LoginState::enter()
{
	SINGLETON_GET(GraphicsEngine, graphicsEngine);
	SINGLETON_GET(InterfaceManager, interfaceManager);

	int screenWidth = graphicsEngine.getScreenWidth();
	int screenHeight = graphicsEngine.getScreenHeight();
	mHalfScreenWidth = screenWidth / 2;
	mHalfScreenHeight = screenHeight / 2;

	GET_PATH("\\Data\\Settings\\", path)

	file = new XMLFile();
    file->load(path + "omb.cfg");

	AG_Window *win = AG_WindowNew(AG_WINDOW_PLAIN|AG_WINDOW_DENYFOCUS|AG_WINDOW_KEEPBELOW);
	AG_WindowShow(win);
	AG_WindowMaximize(win);

	interfaceManager.addWindow(win);

	mLoginWindow = AG_WindowNewNamed(AG_WINDOW_NOBUTTONS, "LoginWindow");
	mRegisterWindow = AG_WindowNewNamed(AG_WINDOW_NOBUTTONS, "Register");

	createLoginWidgets();
	createRegisterWidgets();
}

void LoginState::exit()
{
	SINGLETON_GET(InterfaceManager, interfaceManager);

	interfaceManager.removeAllWindows();
	delete file;
}

bool LoginState::update()
{
	SINGLETON_GET(InputManager, inputManager);

	// Check for input, if escape pressed, exit
	if (inputManager.getKey(AG_KEY_ESCAPE))
	{
	//	networkManager->disconnect();
		return false;
	}

	SDL_Delay(0);

	return true;
}

void LoginState::createLoginWidgets()
{
	SINGLETON_GET(Game, game)
	SINGLETON_GET(InterfaceManager, interfaceManager)

	// read from config file whether username was saved from last time
	file->setElement("login");
	int loginState = file->readInt("login", "save");
	std::string savedUser;

	if (loginState == 1)
	{
		savedUser = file->readString("login", "username");
	}

	// Load windows from file
    XMLFile guifile;
    std::string filename;
    std::string name;
    std::string title;
    std::string userText;
    std::string passText;
    std::string saveText;
    std::string submitText;
    std::string regText;
    std::string optionsText;
    int w;
    int h;

    filename = "login.";
    filename.append(game.getLanguage());
    filename.append(".xml");

	GET_PATH("\\Data\\Settings\\", path);

    if (guifile.load(path + filename))
    {
        guifile.setElement("window");
        name = guifile.readString("window", "name");
        title = guifile.readString("window", "title");
        w = guifile.readInt("window", "width");
        h = guifile.readInt("window", "height");
        guifile.setSubElement("input");
		userText = guifile.readString("input", "text");
		guifile.nextSubElement("input");
		passText = guifile.readString("input", "text");
		guifile.nextSubElement("input");
		saveText = guifile.readString("input", "text");
		guifile.setSubElement("button");
		submitText = guifile.readString("button", "text");
		guifile.nextSubElement("button");
		regText = guifile.readString("button", "text");
		guifile.nextSubElement("button");
		optionsText = guifile.readString("button", "text");
		guifile.close();

        AG_WindowSetCaption(mLoginWindow, title.c_str());
        AG_WindowSetSpacing(mLoginWindow, 12);
        AG_WindowSetGeometry(mLoginWindow, mHalfScreenWidth - (w / 2) , mHalfScreenHeight - (h / 2), w, h);

        AG_Textbox *username = AG_TextboxNew(mLoginWindow, 0, userText.c_str());
        AG_TextboxSizeHint(username, "XXXXXXXXXXXXXXXX");
        if (!savedUser.empty())
        {
            AG_TextboxSetString(username, savedUser.c_str());
        }
        AG_Textbox *password = AG_TextboxNew(mLoginWindow, AG_TEXTBOX_PASSWORD, passText.c_str());
        AG_TextboxSizeHint(password, "XXXXXXXXXXXXXXXX");
        AG_Checkbox *save = AG_CheckboxNew(mLoginWindow, 0, saveText.c_str());
        if (loginState == 1)
        {
            AG_CheckboxToggle(save);
        }
        AG_SetEvent(password, "textbox-return", submit_login, "%p%p%p%p", username, password, save, file);

        AG_HBox *box = AG_HBoxNew(mLoginWindow, 0);
        AG_Button *button = AG_ButtonNewFn(box, 0, submitText.c_str(), submit_login, "%p%p%p%p",
                                            username, password, save, file);
        AG_ButtonJustify(button, AG_TEXT_CENTER);
        AG_Button *register_button = AG_ButtonNewFn(box, 0, regText.c_str(),
                                                    switch_login_window, "%p%p",
                                                    mLoginWindow, mRegisterWindow);
        AG_ButtonJustify(register_button, AG_TEXT_CENTER);

        AG_ButtonNewFn(mLoginWindow, 0, optionsText.c_str(), goto_options, 0);

        AG_WindowShow(mLoginWindow);
        interfaceManager.addWindow(mLoginWindow);
    }
    else
    {
        std::cerr << "XML file not found\n";

        // create the login window's widgets
        AG_WindowSetCaption(mLoginWindow, "Login");
        AG_WindowSetSpacing(mLoginWindow, 12);
        AG_WindowSetGeometry(mLoginWindow, mHalfScreenWidth - 125, mHalfScreenHeight - 100, 225, 200);

        AG_Textbox *username = AG_TextboxNew(mLoginWindow, 0, "Username: ");
        AG_TextboxSizeHint(username, "XXXXXXXXXXXXXXXX");
        if (!savedUser.empty())
        {
            AG_TextboxSetString(username, savedUser.c_str());
        }
        AG_Textbox *password = AG_TextboxNew(mLoginWindow, AG_TEXTBOX_PASSWORD, "Password: ");
        AG_TextboxSizeHint(password, "XXXXXXXXXXXXXXXX");
        AG_Checkbox *save = AG_CheckboxNew(mLoginWindow, 0, "Save username for next time");
        if (loginState == 1)
        {
            AG_CheckboxToggle(save);
        }
        AG_SetEvent(password, "textbox-return", submit_login, "%p%p%p%p", username, password, save, file);

        AG_HBox *box = AG_HBoxNew(mLoginWindow, 0);
        AG_Button *button = AG_ButtonNewFn(box, 0, "Submit", submit_login, "%p%p%p%p",
                                            username, password, save, file);
        AG_ButtonJustify(button, AG_TEXT_CENTER);
        AG_Button *register_button = AG_ButtonNewFn(box, 0, "Register",
                                                    switch_login_window, "%p%p",
                                                    mLoginWindow, mRegisterWindow);
        AG_ButtonJustify(register_button, AG_TEXT_CENTER);

        AG_ButtonNewFn(mLoginWindow, 0, "Options", goto_options, 0);

        AG_WindowShow(mLoginWindow);
        interfaceManager.addWindow(mLoginWindow);
    }
}

void LoginState::createRegisterWidgets()
{
	SINGLETON_GET(Game, game)
	SINGLETON_GET(InterfaceManager, interfaceManager)

	// Load windows from file
    XMLFile guifile;
    std::string filename;
    std::string name;
    std::string title;
    std::string userText;
    std::string passText;
    std::string submitText;
    std::string loginText;
    int w;
    int h;

    filename = "register.";
    filename.append(game.getLanguage());
    filename.append(".xml");

	GET_PATH("\\Data\\Settings\\", path)

    if (guifile.load(path + filename))
    {
        guifile.setElement("window");
        name = guifile.readString("window", "name");
        title = guifile.readString("window", "title");
        w = guifile.readInt("window", "width");
        h = guifile.readInt("window", "height");
        guifile.setSubElement("input");
		userText = guifile.readString("input", "text");
		guifile.nextSubElement("input");
		passText = guifile.readString("input", "text");
		guifile.setSubElement("button");
		submitText = guifile.readString("button", "text");
		guifile.nextSubElement("button");
		loginText = guifile.readString("button", "text");
		guifile.close();

        // create registration widgets
        AG_WindowSetCaption(mRegisterWindow, title.c_str());
        AG_WindowSetSpacing(mRegisterWindow, 12);
        AG_WindowSetGeometry(mRegisterWindow, mHalfScreenWidth - (w / 2) , mHalfScreenHeight - (h / 2), w, h);

        AG_Textbox *reg_user = AG_TextboxNew(mRegisterWindow, 0, userText.c_str());
        AG_Textbox *reg_pass = AG_TextboxNew(mRegisterWindow, AG_TEXTBOX_PASSWORD, passText.c_str());
        AG_SetEvent(reg_pass, "textbox-return", submit_register, "%p%p", reg_user, reg_pass);
        AG_ExpandHoriz(reg_user);
        AG_ExpandHoriz(reg_pass);

        AG_HBox *reg_box = AG_HBoxNew(mRegisterWindow, 0);
        AG_Button *reg_button = AG_ButtonNewFn(reg_box, 0, submitText.c_str(), submit_register, "%p%p",
                                                reg_user, reg_pass);
        AG_ButtonJustify(reg_button, AG_TEXT_CENTER);
        AG_Button *back_button = AG_ButtonNewFn(reg_box, 0, loginText.c_str(),
                                                switch_login_window, "%p%p", mRegisterWindow, mLoginWindow);
        AG_ButtonJustify(back_button, AG_TEXT_CENTER);

        AG_WindowHide(mRegisterWindow);
        interfaceManager.addWindow(mRegisterWindow);
    }
    else
    {
        std::cerr << "XML file not found. \n";

        // create registration widgets
        AG_WindowSetCaption(mRegisterWindow, "Registration");
        AG_WindowSetSpacing(mRegisterWindow, 12);
        AG_WindowSetGeometry(mRegisterWindow, mHalfScreenWidth - 125, mHalfScreenHeight - 45, 225, 135);

        AG_Textbox *reg_user = AG_TextboxNew(mRegisterWindow, 0, "Username: ");
        AG_Textbox *reg_pass = AG_TextboxNew(mRegisterWindow, AG_TEXTBOX_PASSWORD, "Password: ");
        AG_SetEvent(reg_pass, "textbox-return", submit_register, "%p%p", reg_user, reg_pass);
        AG_ExpandHoriz(reg_user);
        AG_ExpandHoriz(reg_pass);

        AG_HBox *reg_box = AG_HBoxNew(mRegisterWindow, 0);
        AG_Button *reg_button = AG_ButtonNewFn(reg_box, 0, "Submit", submit_register, "%p%p",
                                                reg_user, reg_pass);
        AG_ButtonJustify(reg_button, AG_TEXT_CENTER);
        AG_Button *back_button = AG_ButtonNewFn(reg_box, 0, "Back",
                                                switch_login_window, "%p%p", mRegisterWindow, mLoginWindow);
        AG_ButtonJustify(back_button, AG_TEXT_CENTER);

        interfaceManager.addWindow(mRegisterWindow);
    }
}