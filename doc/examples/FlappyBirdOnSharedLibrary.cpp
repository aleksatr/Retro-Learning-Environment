/* *****************************************************************************
 * A.L.E (Arcade Learning Environment)
 * Copyright (c) 2009-2013 by Yavar Naddaf, Joel Veness, Marc G. Bellemare,
 *  Matthew Hausknecht, and the Reinforcement Learning and Artificial Intelligence 
 *  Laboratory
 * Released under the GNU General Public License; see License.txt for details. 
 *
 * Based on: Stella  --  "An Atari 2600 VCS Emulator"
 * Copyright (c) 1995-2007 by Bradford W. Mott and the Stella team
 *
 * *****************************************************************************
 *  sharedLibraryInterfaceExample.cpp 
 *
 *  Sample code for running an agent with the shared library interface. 
 **************************************************************************** */

#include <iostream>
#include <rle_interface.hpp>
#include <DebugMacros.h>
#include <algorithm>

#ifdef __USE_SDL
 #include <SDL.h>
#include "SDL/SDL_rotozoom.h"

#endif

using namespace std;
using namespace rle;

#include <unistd.h>
int main(int argc, char** argv) 
{
    if (argc < 2) 
    {
        std::cerr << "Usage: " << argv[0] << " rom_file core_file" << std::endl;
        return 1;
    }

    RLEInterface rle;

    // Get & Set the desired settings
    rle.setInt("random_seed", 123);
    //The default is already 0.25, this is just an example
    rle.setFloat("repeat_action_probability", 0.25);

#ifdef __USE_SDL
    rle.setBool("display_screen", true);
    rle.setBool("sound", true);
#endif
    // Load the ROM file. (Also resets the system for new settings to
    // take effect.)
    rle.loadROM(argv[1], argv[2]);

    // Get the vector of minimal actions
    ActionVect legal_actions = rle.getMinimalActionSet();

    const RLERAM &ram = rle.getRAM();
    unsigned int sRAM = 0x0037;
    unsigned int yRAM = 0x003F;
    unsigned int pRAM = 0x003B;

    // Play 10 episodes
    for (int episode = 0; episode < 10; episode++) 
    {
        float totalReward = 0;
	    std::cout << "Episode no: " << episode << std::endl;

        int r[] = {0, 0, 0};
        int w[] = {0, 0};
        byte_t s;
        byte_t y;
        byte_t p;
        unsigned int h;
        
        while (!rle.game_over()) 
        {
            s = ram.get(sRAM);
            y = ram.get(yRAM);
            p = ram.get(pRAM);
            
            if (w[0] > 0)
                --w[0];

            if (w[1] > 0)
                --w[1];

            if (p != r[0])
            {
                w[0] = 110;
                r[0] = p;
            }
            
            if (w[0] == 0 && r[1] != r[0])
            {
                r[1] = r[0];
                w[1] = 20;
            }

            if (w[1] == 0 && r[2] != r[1])
                r[2] = r[1];

            h = 85 + 15 * r[2];
            
            Action a = (y > h && s != 255) ? legal_actions[1] : legal_actions[0];
        	// Apply the action and get the resulting reward
            float reward = rle.act(a);
		    //std::cout << "Action: " << a << "Reward: " << reward << std::endl;
            totalReward += reward;
        }
        
        cout << "Episode " << episode << " ended with score: " << totalReward << endl;
        cout << "Y = " << y << "; H = " << h << "; S = " << s << ";" << endl;
        rle.reset_game();
    }
    return 0;
}

