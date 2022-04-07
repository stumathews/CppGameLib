#include "pch.h"
#include "ai/FSM.h"
#include "ai/FSMTransition.h"
#include "ai/FSMState.h"

using namespace std;
using namespace gamelib;


class StateOne : public FSMState
{
public:
	int updateTicks = 0;
	StateOne()
	{
		OnUpdate = [&]() {updateTicks++; };
	}
};

class StateTwo : public FSMState
{
public:
	int updateTicks = 0;
	StateTwo()
	{
		OnUpdate = [&]() { updateTicks++; };
	}
};

class FSMTests: public testing::Test
{
 protected:
	 FSM finiteStateMachine;
	 StateOne stateOne;
	 StateTwo stateTwo;

	 bool transitioningToOne;
	 bool transitioningToTwo;
	 bool doTransitionToOne;
	 bool doTransitionToTwo;
	 bool stateTwoEntered;
	 bool stateTwoExited;
	 bool stateOneEntered;
	 bool stateOneExited;

	 void TransitionToOne()
	 {
		 doTransitionToOne = true;
		 doTransitionToTwo = false;
	 }

	 void TransitionToTwo()
	 {
		 doTransitionToOne = false;
		 doTransitionToTwo = true;
	 }

	 void SetUp() override
	 {		 		 
		 transitioningToOne = false;
		 transitioningToTwo = false;
		 doTransitionToOne = false;
		 doTransitionToTwo = false;

		 stateTwoEntered = false;
		 stateTwoExited = false;
		 stateOneEntered = false;
		 stateOneExited = false;		 

		 stateOne.OnEnter = [&]() { stateOneEntered = true; };
		 stateOne.OnExit = [&]() { stateOneExited = true; };
		 
		 stateTwo.OnEnter = [&]() { stateTwoEntered = true; };
		 stateTwo.OnExit = [&]() { stateTwoExited = true; };
		 finiteStateMachine.States.push_back(stateOne);
		 finiteStateMachine.States.push_back(stateTwo);
		 finiteStateMachine.InitialState = &stateOne;

		 // one -> two
		 FSMTransition transitionToTwo;
		 transitionToTwo.IsValid = [&]() { return doTransitionToTwo;  };
		 transitionToTwo.GetNextState = [&]() { return &stateTwo; };
		 transitionToTwo.OnTransition = [&]() { transitioningToTwo = true; transitioningToOne = false;  };

		 // two -> one
		 FSMTransition transitionToOne;
		 transitionToOne.IsValid = [&]() { return doTransitionToOne;  };
		 transitionToOne.GetNextState = [&]() { return &stateOne; };
		 transitionToOne.OnTransition = [&]() { transitioningToOne = true; transitioningToTwo = false;  };

		 // State one can transition to Two
		 stateOne.Transitions.push_back(transitionToTwo);

		 // State two can transition to One
		 stateTwo.Transitions.push_back(transitionToOne);
	 }

	 void TearDown() override 
	 {
		 transitioningToOne = false;
		 transitioningToTwo = false;
		 doTransitionToOne = false;
		 doTransitionToTwo = false;
		 finiteStateMachine.States.clear();
		 finiteStateMachine.ActiveState = finiteStateMachine.InitialState = nullptr;
		 stateOne.Transitions.clear();
		 stateTwo.Transitions.clear();

	 }

};


TEST_F(FSMTests, Basic)
{
	// FSM should have two states loaded
	EXPECT_EQ(finiteStateMachine.States.size(), 2);
		
	// Update the FSM
	finiteStateMachine.Update();
		
	// Should increment update tick of initial state
	EXPECT_EQ(stateOne.updateTicks, 1);

	// Should not increment update tick of inactive state
	EXPECT_EQ(stateTwo.updateTicks, 0);

	// Two more ticks for update state
	finiteStateMachine.Update(); 
	finiteStateMachine.Update();

	// Should show up as incrments to current state
	EXPECT_EQ(stateOne.updateTicks, 3);

	// but not the other state
	EXPECT_EQ(stateTwo.updateTicks, 0);

	EXPECT_FALSE(transitioningToOne) << "Did not expect any transitions to occur";
	EXPECT_FALSE(transitioningToTwo) << "Did not expect any transitions to occur";
}

TEST_F(FSMTests, TestBasicTransition)
{	
	// FSM should have two states loaded
	EXPECT_EQ(finiteStateMachine.States.size(), 2);

	// Update the FSM
	finiteStateMachine.Update();

	// Should increment update tick of active state
	EXPECT_EQ(stateOne.updateTicks, 1);

	// Should not increment update tick of inactive state
	EXPECT_EQ(stateTwo.updateTicks, 0);

	TransitionToTwo();
	finiteStateMachine.Update();
	finiteStateMachine.Update();

	EXPECT_TRUE(transitioningToTwo);
	EXPECT_FALSE(transitioningToOne);
	
	// Update two should have transition and increased its update ticks accordingly
	EXPECT_EQ(stateTwo.updateTicks, 2);

	// other state should not have changed/recieved a tick
	EXPECT_EQ(stateOne.updateTicks, 1);

	// Transition back to one
	TransitionToOne();
	finiteStateMachine.Update();
	finiteStateMachine.Update();

	EXPECT_TRUE(transitioningToOne);
	EXPECT_FALSE(transitioningToTwo);
	EXPECT_EQ(stateOne.updateTicks, 3);

	// should remain at last tick count
	EXPECT_EQ(stateTwo.updateTicks, 2);

	// Transition back to two
	TransitionToTwo();
	finiteStateMachine.Update();
	finiteStateMachine.Update();
	finiteStateMachine.Update();
	EXPECT_EQ(stateTwo.updateTicks, 5);
	EXPECT_TRUE(transitioningToTwo);
	EXPECT_FALSE(transitioningToOne);
}

TEST_F(FSMTests, TestOnEntryExit)
{
	TransitionToTwo();

	finiteStateMachine.Update();

	EXPECT_TRUE(stateOneExited);
	EXPECT_TRUE(stateTwoEntered);	
}