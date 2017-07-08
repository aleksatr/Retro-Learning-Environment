#include <iostream>
#include <vector>
#include "monte_carlo.h"
#include <sstream>
#include <unordered_map>

using namespace std;

namespace object_model
{
    bool MonteCarlo::EvaluateAndImprovePolicy(double reward, bool isFinal)
    {
        bool actionChanged = false;
        reward = 0;

        unordered_map<int, int> first_occurances;

        for (int i = 0; i < _history.size(); ++i)
        {
            pair<State, Action> currStateAction = _history[i];
            Action currAction = (Action)currStateAction.second;
            State currState = (State)currStateAction.first;

            int stateIndex = currState.ToIndex();
            if (first_occurances.count(stateIndex) == 0)
            {
                first_occurances.insert(pair<int, int>(stateIndex, i));
            }
        }

        for (int i = _history.size() - 1; i >= 0; --i)
        {
            pair<State, Action> currStateAction = _history[i];
            Action currAction = (Action)currStateAction.second;
            State currState = (State)currStateAction.first;
            reward = reward_history[i] + discount * reward;

            int stateIndex = currState.ToIndex();
            int qIndex = currAction * NUMBER_OF_STATES + stateIndex;

            if (first_occurances[stateIndex] != i)
            {
                //this needs to be done for First Visit Monte Carlo!
                //cout<<"not first i = " << i <<endl;
                times_visited[qIndex]--;
            }
            else
            {
                //eval
                q[qIndex] = q[qIndex] + 1.0 / times_visited[qIndex] * (reward - q[qIndex]);
            }
            
            //improve
            Action previousAction = _actions[stateIndex];
            _actions[stateIndex] = q[stateIndex] >= q[stateIndex + NUMBER_OF_STATES] ? NOOP : JUMP;

            actionChanged = actionChanged || previousAction != _actions[stateIndex];
        }
        
        return actionChanged;
    }

    void MonteCarlo::FlushToDisk(char *filename)
{
    ofstream f(filename);

    if (!f)
        return;

    ostringstream o;
    for (int i = 0; i < NUMBER_OF_STATES * NUMBER_OF_ACTIONS; i++)
    {
        o << q[i] << endl;
    }
    for (int i = 0; i < NUMBER_OF_STATES * NUMBER_OF_ACTIONS; i++)
    {
        o << times_visited[i] << endl;
    }
    f << o.str();

    f.close();

    cout << "Data saved to file: " << filename << endl;
}

void MonteCarlo::LoadFromDisk(char *filename)
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

    i = 0;
    while (!f.eof() && i < NUMBER_OF_STATES * NUMBER_OF_ACTIONS)
    {
        string line;

        getline(f, line);

        int t_visited = atoi(line.c_str());

        times_visited[i++] = t_visited;
    }

    f.close();

    cout << "Data loaded from file: " << filename << endl;
}
}
