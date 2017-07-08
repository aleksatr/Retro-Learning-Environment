#include <iostream>
#include <vector>
#include <memory>
#include <cassert>
#include <string>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include "policy.h"
#include <sstream>

using namespace std;

namespace object_model
{

Policy::Policy()
{
    for (int i = 0; i < NUMBER_OF_STATES; i++)
    {
        _actions[i] = NOOP;
        q[i] = q[i + NUMBER_OF_STATES] = 0;
        times_visited[i] = times_visited[i + NUMBER_OF_STATES] = 0;
    }

    srand(time(NULL));
}

void Policy::ClearHistory()
{
    _history.clear();
    reward_history.clear();
}

void Policy::PrintHistory()
{
    for (int i = _history.size() - 1; i >= 0 && i >= _history.size() - PRINT_LAST_STATEACTIONS; i--)
    {
        int stateIndex = ((State)_history[i].first).ToIndex() % NUMBER_OF_STATES;

        std::cout << "State " << ((State)_history[i].first).height_difference << ";" << ((State)_history[i].first).bird_direction << ";"
                  << " Action " << _history[i].second << " Qs " << q[stateIndex] << ";" << q[stateIndex + NUMBER_OF_STATES] << endl;
    }
}

Policy::~Policy()
{
}

Action Policy::GetAction(State s)
{
    int index = s.ToIndex(); //calculate index from state
    Action a = _actions[index];

    double r = ((double)rand() / (RAND_MAX));

    if (r < epsilon)
    {
        double randAction = ((double)rand() / (RAND_MAX));
        a = randAction > 0.5 ? JUMP : NOOP;

        cout << "Random action taken. Action = " << a << endl;
    }

    times_visited[NUMBER_OF_STATES * a + index]++;
    _history.push_back(pair<State, Action>(s, a));
    return a;
}

void Policy::AddRewardToHistory(double reward)
{
    reward_history.push_back(reward);
}

void Policy::FlushToDisk(char *filename)
{
    ofstream f(filename);

    if (!f)
        return;

    ostringstream o;
    for (int i = 0; i < NUMBER_OF_STATES * NUMBER_OF_ACTIONS; i++)
    {
        o << q[i] << endl;
    }
    f << o.str();

    f.close();

    cout << "Data saved to file: " << filename << endl;
}

void Policy::LoadFromDisk(char *filename)
{
    ifstream f(filename);
    if (!f || !f.is_open())
        return;

    int i = 0;
    while (!f.eof() && i < NUMBER_OF_STATES * NUMBER_OF_ACTIONS)
    {
        string line;

        getline(f, line);

        double q_val = atof(line.c_str());

        q[i++] = q_val;
        if (i >= NUMBER_OF_STATES)
            _actions[i - NUMBER_OF_STATES] = q[i] > q[i - NUMBER_OF_STATES] ? JUMP : NOOP;
    }

    f.close();

    cout << "Data loaded from file: " << filename << endl;
}
}