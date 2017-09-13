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
#include <unistd.h>
#include <limits>
#include <fstream>
#include <sstream>

#ifdef __USE_SDL
#include <SDL.h>
#include "SDL/SDL_rotozoom.h"
#endif

#define SNAPSHOT_INTERVAL 100
#define FRAME_SKIP 1

#define NUMBER_OF_STATES 46875//101277//11520//12659625ul
#define NUMBER_OF_ACTIONS 4
#define EXPLORATION 10

double epsilon = 1.0 / EXPLORATION;
double alpha = 1.0;
const double discount = 0.99;
unsigned long iteration = 0;

using namespace std;
using namespace rle;

float *q_table;

uint32_t BestActionForState(unsigned long stateIndex);
Action UnshiftAction(uint32_t sa);
uint32_t ShiftAction(Action a);
void FlushToDisk(char *filename);
void LoadFromDisk(char *filename);
void IncrementQ(unsigned long prevStateActionIndex, unsigned long nextStateIndex, float reward, bool isFinal);

enum ShiftedAction
{
    NOOP = 0,
    FIRE = 1,
    UP = 2,
    DOWN = 3
};

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        cerr << "Usage: " << argv[0] << " rom_file core_file" << endl;
        return 1;
    }

    srand(time(NULL));

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
    
    q_table = new float[NUMBER_OF_STATES * NUMBER_OF_ACTIONS];
    if (!q_table)
    {
        std::cout << "Failed to allocate enough memory for q table!\n";
        return -1;
    }

    //initialize q_table to all zeros
    for (unsigned long i = 0; i < NUMBER_OF_STATES * NUMBER_OF_ACTIONS; ++i)
    {
        q_table[i] = 0.0f;
    }

    char *filename = 0;
    if(argc >= 4)
    {
        filename = argv[3];
        LoadFromDisk(filename);
    }
    
    bool justPlay = false;
    if(argc >= 5 && argv[4][0] == 'p')
    {
        justPlay = true;
        epsilon = 0.0;
    }

    //ram values
    //0x31 ball x position 32-CD
    //0x36 ball y position (player's was 26-cB)
    //0x38 ball y velocity -5 to 5
    //0x3A ball x velocity -5 to 5
    //0x3c player y 26-CB

    const RLERAM &ram = rle.getRAM();
    unsigned int ball_x_pos_addr = 0x31;
    unsigned int ball_y_pos_addr = 0x36;
    unsigned int ball_x_vel_addr = 0x3A;
    unsigned int ball_y_vel_addr = 0x38;
    unsigned int player_y_pos_addr = 0x3C;
    int ball_x_pos = 0;
    int ball_y_pos = 0;
    int ball_x_vel = 0;
    int ball_y_vel = 0;
    int player_y_pos = 0;
    
    for (int episode = 0;; episode++)
    {
        if (argc >= 4 && episode && !(episode % SNAPSHOT_INTERVAL))
        {
            FlushToDisk(filename);
        }
        
        float totalReward = 0;

	    std::cout << "Episode no: " << episode << std::endl;
        //unsigned long lastStateActionIndex = NUMBER_OF_STATES * NUMBER_OF_ACTIONS;
        float reward = 0.0f;
        unsigned long stateIndex;
        unsigned long stateActionIndex = NUMBER_OF_STATES * NUMBER_OF_ACTIONS;

        while (!rle.game_over())
        {
            ++iteration;

            ball_x_pos = (ram.get(ball_x_pos_addr) % 250) / 10;
            ball_y_pos = (ram.get(ball_y_pos_addr) % 250) / 10;
            ball_x_vel = (char) ram.get(ball_x_vel_addr);              //let's just use values -1, 0, 1 for now
            ball_y_vel = (char) ram.get(ball_y_vel_addr);              //we'll ignore this for now
            player_y_pos = (ram.get(player_y_pos_addr) % 250) / 10;
            
            if (ball_x_vel > 0)
            {
                ball_x_vel = 1;
            }
            else if (ball_x_vel < 0)
            {
                ball_x_vel = -1;
            }

            //ok, lets make it positive
            ++ball_x_vel;
            
            //Action a = legal_actions[rand() % legal_actions.size()];
            
            //stateIndex = ball_x_pos * 81675 + ball_y_pos * 495 + player_y_pos * 3 + ball_x_vel; //prepravi!!!
            //stateIndex = ball_x_pos * 768 + ball_y_pos * 48 + player_y_pos * 3 + ball_x_vel;
            stateIndex = ball_x_pos * 1875 + ball_y_pos * 75 + player_y_pos * 3 + ball_x_vel;
            if(!justPlay)
            {
                IncrementQ(stateActionIndex, stateIndex, reward, false);
            }
            uint32_t chosenAction = 0;

            //pick action
            //epsilon-greedy
            double r = ((double)rand() / RAND_MAX);
            if (r < epsilon)
            {
                chosenAction = (uint32_t) (rand() % NUMBER_OF_ACTIONS);
                
                //std::cout << "Random action taken. Action = " << (int)chosenAction << std::endl;
            }
            else
            {
                chosenAction = BestActionForState(stateIndex);
                //std::cout << "Best action taken. Action = " << (int)chosenAction << std::endl;
            }
            
            stateActionIndex = chosenAction * NUMBER_OF_STATES + stateIndex;
            
            Action a = UnshiftAction(chosenAction);
            // Apply the action and get the resulting reward
            
            reward = rle.act(a);
            //std::cout << "Action: " << a << "Reward: " << reward << std::endl;
            //std::cout << "ChosenAction: " << (int)chosenAction << "Reward: " << reward << std::endl;
            
            //std::cout << "ball_x_pos[" << ball_x_pos << "] ball_y_pos[" << ball_y_pos << "] ball_x_vel[" 
            //    << ball_x_vel << "] ball_y_vel[" << ball_y_vel 
            //    << "] player_y_pos[" << player_y_pos << "]\n";
            
            //lastStateActionIndex = stateActionIndex;

            // if (reward != 0)
            // {
            //     std::cout << "StateIndex: " << stateIndex << std::endl;
            //     for (int i = 0; i < NUMBER_OF_ACTIONS; ++i)
            //     {
            //         unsigned long actionIndex = i * NUMBER_OF_STATES + stateIndex;
            //         std::cout << q_table[actionIndex] << " ";
            //     }
            //     std::cout << endl;
            // }

            totalReward += reward;
        }

        if(!justPlay)
        {
            IncrementQ(stateActionIndex, stateIndex, reward, true);
        }

        cout << "Episode " << episode << " ended with score: " << totalReward << endl;
        cout << "Alpha = " << alpha << endl;

        //
        std::ofstream log("logfile.txt", std::ios_base::app | std::ios_base::out);
        log << totalReward << "\n";
        //

        rle.reset_game();
        //break;
    }

    if (q_table)
    {
        delete [] q_table;
    }

    return 0;
}


uint32_t ShiftAction(Action a)
{
    switch (a) 
    {
        case  PLAYER_A | JOYPAD_NOOP:
            return NOOP;
        case  PLAYER_A | JOYPAD_FIRE:
            return FIRE;
        case  PLAYER_A | JOYPAD_UP:
            return UP;
        case  PLAYER_A | JOYPAD_DOWN:
            return DOWN;
        default:
            throw "Unsupported Action for Pong!";
    } 
}

Action UnshiftAction(uint32_t sa)
{
    switch (sa) 
    {
        case NOOP:
            return PLAYER_A | JOYPAD_NOOP;
        case FIRE:
            return PLAYER_A | JOYPAD_FIRE;
        case UP:
            return PLAYER_A | JOYPAD_UP;
        case DOWN:
            return PLAYER_A | JOYPAD_DOWN;
        default:
            throw "Unsupported ShiftedAction for Pong!";
    } 
}

void IncrementQ(unsigned long prevStateActionIndex, unsigned long nextStateIndex, float reward, bool isFinal)
{
    //this is initial pass, we do not update
    //NUMBER_OF_STATES * NUMBER_OF_ACTIONS is here used as a special case designator
    if (prevStateActionIndex == NUMBER_OF_STATES * NUMBER_OF_ACTIONS)
    {
        return;
    }
    std::cout<<"updating q"<<std::endl;
    alpha = pow(iteration, -0.14);

    if (isFinal)
    {
        q_table[prevStateActionIndex] = q_table[prevStateActionIndex] + 
            alpha * (reward - q_table[prevStateActionIndex]);

        std::cout << "q[" << prevStateActionIndex << "] updated to value " << q_table[prevStateActionIndex] << std::endl;
    }
    else
    {
        uint32_t bestAction = BestActionForState(nextStateIndex);
        unsigned long bestNextStateActionIndex = bestAction * NUMBER_OF_STATES + nextStateIndex;

        //q_table[prevStateActionIndex] = (1 - alpha) * q_table[prevStateActionIndex] + ...
        
        q_table[prevStateActionIndex] = q_table[prevStateActionIndex] + 
            alpha * (reward + discount * q_table[bestNextStateActionIndex] - q_table[prevStateActionIndex]);
    }
}

uint32_t BestActionForState(unsigned long stateIndex)
{
    float maxQ = -std::numeric_limits<float>::max();
    int maxIndex = 0;

    for (int i = 0; i < NUMBER_OF_ACTIONS; ++i)
    {
        unsigned long actionIndex = i * NUMBER_OF_STATES + stateIndex;
        //std::cout << q_table[actionIndex] << " ";

        if (q_table[actionIndex] > maxQ)
        {
            maxQ = q_table[actionIndex];
            maxIndex = i;
        }
    }
    //std::cout << endl;

    return (uint32_t) maxIndex;
}

void FlushToDisk(char *filename)
{
    ofstream f(filename);

    if (!f)
        return;

    ostringstream o;
    for (unsigned long i = 0; i < NUMBER_OF_STATES * NUMBER_OF_ACTIONS; i++)
    {
        o << q_table[i] << endl;
    }
    f << o.str();

    f.close();
    
    cout << "Data saved to file: " << filename << endl;
}

void LoadFromDisk(char *filename)
{
    ifstream f(filename);

    if (!f || !f.is_open())
        return;

    unsigned long i = 0;
    while (!f.eof() && i < NUMBER_OF_STATES * NUMBER_OF_ACTIONS)
    {
        string line;
        getline(f, line);

        double q_val = atof(line.c_str());

        q_table[i++] = q_val;
        // if (i >= NUMBER_OF_STATES)
        //     _actions[i - NUMBER_OF_STATES] = q[i] > q[i - NUMBER_OF_STATES] ? JUMP : NOOP;
    }

    f.close();

    cout << "Data loaded from file: " << filename << endl;
}

