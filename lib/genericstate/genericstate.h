#pragma once

#include <memory>

#ifndef GENERICSTATE_H
#define GENERICSTATE_H

template <class State>
using StateRef = std::unique_ptr<State>;

template <typename StateMachine, class State>
class GenericState
{
public:
    explicit GenericState(StateMachine &stm, StateRef<State> &state) :
        stm(stm), state(state) {}

    template <class ConcreteState>
    static void init(StateMachine &stm, StateRef<State> &state) {
        state = StateRef<State>(new ConcreteState(stm, state));
        state->entry();
    }

protected:
    template <class ConcreteState>
    void change() {
        exit();
        init<ConcreteState>(stm, state);
    }

    void reenter() {
        exit();
        entry();
    }

private:
    virtual void entry() {}
    virtual void exit() {}

protected:
    StateMachine &stm;

private:
    StateRef<State> &state;
};

#endif // GENERICSTATE_H
