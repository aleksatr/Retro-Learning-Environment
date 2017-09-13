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

#define SNAPSHOT_INTERVAL 50
#define FRAME_SKIP 1

#define NUMBER_OF_ACTIONS 4
#define NUMBER_OF_FEATURES 2 //including the bias feature
#define EXPLORATION 5

double epsilon = 1.0 / EXPLORATION;
double alpha = 1.0; //0.00025;
const double discount = 0.99;
unsigned long iteration = 10;
float weights[NUMBER_OF_ACTIONS][NUMBER_OF_FEATURES];

using namespace std;
using namespace rle;

uint32_t BestActionForState(float features[]);
Action UnshiftAction(uint32_t sa);
uint32_t ShiftAction(Action a);
void FlushToDisk(char *filename);
void LoadFromDisk(char *filename);
void IncrementQ(unsigned long prevStateActionIndex, unsigned long nextStateIndex, float reward, bool isFinal);
void UpdateWeights(float reward, float oldFeatures[], uint32_t chosenAction, float features[], bool isFinal);
void PrintWeights()
{
    for(int a = 0; a < NUMBER_OF_ACTIONS; ++a)
    {
        std::cout << "Weights [" << a << "] are: [" << weights[a][0];
        
        for(int i = 1; i < NUMBER_OF_FEATURES; ++i)
        {
            std::cout << ", " << weights[a][i];
        }
        std::cout << "]" << std::endl;
    }
}

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
    // Load the ROM file. (Also resets the system for new settings to take effect.)
    rle.loadROM(argv[1], argv[2]);

    // Get the vector of minimal actions
    ActionVect legal_actions = rle.getMinimalActionSet();

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

    float features[NUMBER_OF_FEATURES] = {1}; //0th is always 1
    float oldFeatures[NUMBER_OF_FEATURES] = {1}; //0th is always 1
    features[0] = 0.01f;
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
    PrintWeights();
    if (!justPlay)
    {
        std::cout << "Training the champion..." << std::endl;
    }
    else
    {
        std::cout << "Just playing..." << std::endl;
    }

    for (int episode = 0;; episode++)
    {
        if (argc >= 4 && episode && !(episode % SNAPSHOT_INTERVAL))
        {
            FlushToDisk(filename);
        }
        
        float totalReward = 0;

        std::cout << "Episode no: " << episode << std::endl;
        
        float reward = 0.0f;
        bool startOfEpisode = true;
        uint32_t chosenAction = 0;

        while (!rle.game_over())
        {
            ++iteration;

            ball_x_pos = (ram.get(ball_x_pos_addr));
            ball_y_pos = (ram.get(ball_y_pos_addr));
            ball_x_vel = (char) ram.get(ball_x_vel_addr);
            ball_y_vel = (char) ram.get(ball_y_vel_addr);
            player_y_pos = (ram.get(player_y_pos_addr));
            
            //we scale features in [-1, 1] range
            //features[1] = (ball_x_pos - 127) / 127.0f;
            //features[2] = (ball_y_pos - 127) / 127.0f;
            //features[3] = ball_x_vel / 5.0f;
            //features[4] = ball_y_vel / 5.0f;
            //features[5] = (player_y_pos - 127) / 127.0f;

            int hdiff = player_y_pos - ball_y_pos;

            features[1] = hdiff / 255.0f;

            if(!justPlay && !startOfEpisode)
            {
                UpdateWeights(reward, oldFeatures, chosenAction, features, false);
            }

            //pick action
            //epsilon-greedy
            double r = ((double)rand() / RAND_MAX);
            if (!justPlay)
            {
                epsilon = pow(iteration, -0.15);
            }

            if (r < epsilon)
            {
                chosenAction = (uint32_t) (rand() % NUMBER_OF_ACTIONS);
                //std::cout << "Random action taken. Action = " << (int)chosenAction << std::endl;
            }
            else
            {
                chosenAction = BestActionForState(features);
                //std::cout << "Best action taken. Action = " << (int)chosenAction << std::endl;
            }

            Action a = UnshiftAction(chosenAction);
        	// Apply the action and get the resulting reward
            reward = rle.act(a);

            for(int of = 0; of < NUMBER_OF_FEATURES; ++of)
            {
                oldFeatures[of] = features[of];
            }

            startOfEpisode = false;
            totalReward += reward;
        }

        if(!justPlay)
        {
            UpdateWeights(reward, oldFeatures, chosenAction, features, true);
        }

        cout << "Episode " << episode << " ended with score: " << totalReward << endl;
        cout << "Alpha = " << alpha << endl;
        cout << "Epsilon = " << epsilon << endl;
        PrintWeights();

        //
        std::ofstream log("logfile.txt", std::ios_base::app | std::ios_base::out);
        log << totalReward << "\n";
        //

        rle.reset_game();
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

void UpdateWeights(float reward, float oldFeatures[], uint32_t chosenAction, float features[], bool isFinal)
{
    alpha = pow(iteration, -0.15);
    //todo: avoid double calculations like this!
    float q = 0.0f;
    for(int of = 0; of < NUMBER_OF_FEATURES; ++of)
    {
        q += weights[chosenAction][of] * features[of];
    }
    float diff = 0.0f;

    if (isFinal)
    {
        diff = reward - q;
        for (int i = 0; i < NUMBER_OF_FEATURES; ++i)
        {
            weights[chosenAction][i] +=
                alpha * diff * oldFeatures[i];
        }
    }
    else
    {
        //todo: avoid double calculations like this!
        uint32_t bestAction = BestActionForState(features);
        float q_ = 0.0f;
        for(int of = 0; of < NUMBER_OF_FEATURES; ++of)
        {
            q_ += weights[bestAction][of] * features[of];
        }

        diff = reward + discount * q_ - q;
        for (int i = 0; i < NUMBER_OF_FEATURES; ++i)
        {
            weights[chosenAction][i] +=
                alpha * diff * oldFeatures[i];
        }
        
        //std::cout << "diff = " << diff << std::endl;
    }
}

uint32_t BestActionForState(float features[])
{
    float maxQ = -std::numeric_limits<float>::max();
    int maxIndex = 0;

    for (int i = 0; i < NUMBER_OF_ACTIONS; ++i)
    {
        float q = 0.0f;
        for(int of = 0; of < NUMBER_OF_FEATURES; ++of)
        {
            q += weights[i][of] * features[of];
        }

        if (q > maxQ)
        {
            maxQ = q;
            maxIndex = i;
        }
    }

    return (uint32_t) maxIndex;
}

void FlushToDisk(char *filename)
{
    ofstream f(filename);

    if (!f)
        return;

    ostringstream o;
    for (int i = 0; i < NUMBER_OF_ACTIONS; i++)
    {
        for (int j = 0; j < NUMBER_OF_FEATURES; ++j)
        {
            o << weights[i][j] << endl;
        }
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

    int i = 0;
    int j = 0;
    while (!f.eof() && i < NUMBER_OF_FEATURES * NUMBER_OF_ACTIONS)
    {
        string line;

        getline(f, line);

        double w = atof(line.c_str());

        weights[i][j++] = (float) w;

        if (j == NUMBER_OF_FEATURES)
        {
            j = 0;
            ++i;
        }
    }

    f.close();

    cout << "Data loaded from file: " << filename << endl;
}

