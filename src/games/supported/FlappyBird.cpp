/* *****************************************************************************
 * A.L.E (Arcade Learning Environment)
 * Copyright (c) 2009-2013 by Yavar Naddaf, Joel Veness, Marc G. Bellemare and
 *   the Reinforcement Learning and Artificial Intelligence Laboratory
 * Released under the GNU General Public License; see License.txt for details.
 *
 * Based on: Stella  --  "An Atari 2600 VCS Emulator"
 * Copyright (c) 1995-2007 by Bradford W. Mott and the Stella team
 *
 * *****************************************************************************
 */
#include "../RomUtils.hpp"
#include <iomanip>

#include "../RomUtils.hpp"
#include "FlappyBird.hpp"

#include "RleSystem.hxx"

using namespace rle;

FlappyBirdSettings::FlappyBirdSettings()
{
	reset();

	minimalActions = {
		JOYPAD_NOOP,
		JOYPAD_START,
	};

	flap = 1000;
}

/* create a new instance of the rom */
RomSettings *FlappyBirdSettings::clone() const
{
	RomSettings *rval = new FlappyBirdSettings();
	*rval = *this;
	return rval;
}

/* process the latest information from ALE */
void FlappyBirdSettings::step(const RleSystem &system)
{
	reward_t score = getDecimalScore(0x0041, &system) + 10 * getDecimalScore(0x0042, &system) 
						+ 100 * getDecimalScore(0x0043, &system) + 1000 * getDecimalScore(0x0044, &system);

	m_reward = score - m_score;
	m_score = score;

	bool ingame = getDecimalScore(0x0049, &system) == 1;	
	bool alive = getDecimalScore(0x003C, &system) == 1;

	m_terminal = !alive && ingame;
}

/* is end of game */
bool FlappyBirdSettings::isTerminal() const
{
	return m_terminal;
};

/* get the most recently observed reward */
reward_t FlappyBirdSettings::getReward() const
{
	return m_reward;
}

/* is an action part of the minimal set? */
bool FlappyBirdSettings::isMinimal(const Action &a) const
{
	if (minimalActions.find(a) == minimalActions.end())
		return false;
	else
		return true;
}

/* reset the state of the game */
void FlappyBirdSettings::reset()
{
	m_reward = 0;
	m_score = 0;
	m_terminal = false;
}

/* saves the state of the rom settings */
void FlappyBirdSettings::saveState(Serializer &ser)
{
	ser.putInt(m_reward);
	ser.putInt(m_score);
	ser.putBool(m_terminal);
}

// loads the state of the rom settings
void FlappyBirdSettings::loadState(Deserializer &des)
{
	m_reward = des.getInt();
	m_score = des.getInt();
	m_terminal = des.getBool();
}

ActionVect FlappyBirdSettings::getStartingActions(const RleSystem &system)
{
	int i;
	ActionVect startingActions;

	for (i = 0; i < 350; i++)
	{
		startingActions.push_back(JOYPAD_NOOP);
	}

	startingActions.push_back(JOYPAD_START);

	return startingActions;
}
