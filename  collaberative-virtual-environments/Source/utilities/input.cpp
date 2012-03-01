#include "input.h"

#include "../graphics/graphics.h"
#include "../interface/interfacemanager.h"

#include <SDL.h>
#include <algorithm>

void InputManager::getEvents()
{
    AG_DriverEvent dev;

	InterfaceManager& interfaceManager = InterfaceManager::GetSingleton();

    if (AG_PendingEvents(NULL) > 0)
    {
        do
        {
            if (AG_GetNextEvent(NULL, &dev) == 1)
            {
                switch (dev.type)
                {
                case AG_DRIVER_MOUSE_BUTTON_DOWN:
                    interfaceManager.handleMouseEvent(dev.data.button.which,
                            dev.data.button.x, dev.data.button.y,
                            0);
                    break;
                case AG_DRIVER_MOUSE_BUTTON_UP:
                    interfaceManager.handleMouseEvent(dev.data.button.which,
                            dev.data.button.x, dev.data.button.y,
                            1);
                    break;
                case AG_DRIVER_MOUSE_MOTION:
                    interfaceManager.handleMouseEvent(0, dev.data.motion.x, dev.data.motion.y, 0);
                    break;
                case AG_DRIVER_KEY_DOWN:
                    keysDown.push_back(dev.data.key.ks);
                    break;
                case AG_DRIVER_CLOSE:
                    keysDown.push_back(AG_KEY_ESCAPE);
                    break;
                default:
                    break;
                }

                /* Forward the event to Agar. */
                if (AG_ProcessEvent(NULL, &dev) == -1)
                    break;
            }
        } while (AG_PendingEvents(NULL) > 0);
    }
}

bool InputManager::getKey(AG_KeySym key)
{
	std::list<AG_KeySym>::iterator itr;
	itr = std::find(keysDown.begin(), keysDown.end(), key);
	if (itr != keysDown.end())
	{
		keysDown.erase(itr);
		return true;
	}

	return false;
}