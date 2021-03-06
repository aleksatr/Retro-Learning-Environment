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
#include "../object_model/monte_carlo.h"

#ifdef __USE_SDL
#include <SDL.h>
#include "SDL/SDL_rotozoom.h"

#endif

#define SNAPSHOT_INTERVAL 50
#define FRAME_SKIP 1

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
    rle.setFloat("repeat_action_probability", 0.0f);

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
    Policy *policy;

    char *filename = 0;

    if(argc >= 4 && argv[3][0]=='m')
        policy = new MonteCarlo();
    else
        policy = new Sarsa();


    if (argc >= 5)
        filename = argv[4];

    double epsilon;

    if (argc >= 6)
    {
        epsilon = atof(argv[5]);
        policy->SetEpsilon(epsilon);
    }
    policy->LoadFromDisk(filename);

    for (int episode = 0;; episode++)
    {
        if (argc >= 4 && episode && !(episode % SNAPSHOT_INTERVAL))
            policy->FlushToDisk(filename);

        if (argc < 5)
            policy->SetEpsilon(1.0 / (EXPLORATION + (double)episode));

        float totalReward = 0;

        int r[] = {5, 5, 5};
        int w[] = {0, 0};
        byte_t s;
        byte_t y;
        byte_t p;

        policy->PrintHistory();
        policy->ClearHistory();
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

            unsigned int h = 85 + 15 * r[2];

            int relative_height = r[2] == 5 ? 200 + (y / 2) : ((y - h + 200) % 200); 

            State currentState(relative_height, direction);

            rle::Action a = policy->GetAction(currentState);

            double reward = rle.act(a);

            policy->EvaluateAndImprovePolicy(reward, rle.game_over());
            policy->AddRewardToHistory(reward);
            totalReward += reward;
        }

        cout << "Episode " << episode << " ended with score: " << totalReward << endl;
        rle.reset_game();
    }

    delete policy;
    return 0;
}
