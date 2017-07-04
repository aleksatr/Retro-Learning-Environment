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
#include "../object_model/sarsa.h"

#ifdef __USE_SDL
#include <SDL.h>
#include "SDL/SDL_rotozoom.h"

#endif

#define SNAPSHOT_INTERVAL 50
#define FRAME_SKIP 2

using namespace std;
using namespace rle;
using namespace object_model;

#include <unistd.h>
int main(int argc, char **argv)
{
    if (argc < 2)
    {
        cerr << "Usage: " << argv[0] << " rom_file core_file" << endl;
        return 1;
    }

    RLEInterface rle;

    // Get & Set the desired settings
    rle.setInt("random_seed", 123);
    rle.setInt("frame_skip", FRAME_SKIP);
    rle.setFloat("repeat_action_probability", 0);

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
    Sarsa sarsa;

    char *filename = 0;

    if (argc >= 4)
        filename = argv[3];

    double epsilon;

    if (argc >= 5)
    {
        epsilon = atof(argv[4]);
        sarsa.SetEpsilon(epsilon);
    }
    sarsa.LoadFromDisk(filename);

    for (int episode = 0;; episode++)
    {
        if (argc >= 4 && !(episode % SNAPSHOT_INTERVAL))
            sarsa.FlushToDisk(filename);

        if (argc < 5)
            sarsa.SetEpsilon(1.0 / (EXPLORATION + (double)episode));

        float totalReward = 0;

        int r[] = {5, 5, 5};
        int w[] = {0, 0};
        byte_t s;
        byte_t y;
        byte_t p;
        sarsa.ClearHistory();
        while (!rle.game_over())
        {
            s = ram.get(sRAM);
            y = ram.get(yRAM);
            p = ram.get(pRAM);

            if (w[0] > 0)
                w[0] -= FRAME_SKIP;

            if (w[1] > 0)
                w[1] -= FRAME_SKIP;

            if (p != r[0])
            {
                w[0] = 110;
                r[0] = p;
            }

            if (w[0] <= 0 && r[1] != r[0])
            {
                r[1] = r[0];
                w[1] = 20;
            }

            if (w[1] <= 0 && r[2] != r[1])
                r[2] = r[1];

            byte_t direction = (byte_t)(s + 2);
            // cout << "y " << (int)y << " pipe " << (int)r[2] << " direction " << (int)direction << endl;
            State currentState(y, r[2], direction);

            rle::Action a = sarsa.GetAction(currentState);

            double reward = rle.act(a);

            sarsa.EvaluateAndImprovePolicy(reward, rle.game_over());
            //   cout << "Action: " << a << " Reward: " << reward << std::endl;
            totalReward += reward;
        }

        cout << "Episode " << episode << " ended with score: " << totalReward << endl;
        //cout << "Y = " << (int)y << "; H = " << (int)h << "; S = " << (int)s << ";" << endl;
        rle.reset_game();
    }
    return 0;
}
