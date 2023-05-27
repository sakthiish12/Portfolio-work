#ifndef FSM_H
#define FSM_H

namespace BE
{
    using StateID = unsigned;

    template <typename T>
    class State
    {
    public:
        virtual void Enter(T&) {};
        virtual void Update(T&, double dt) {};
        virtual void Leave(T&) {};
        inline void SetActive(bool in) { active = in; }
        inline bool GetActive() { return active; }
        //inline void SetID(StateID in) { id = in; }
        //inline StateID GetID() { return id; }
    private:
        //StateID id = 0;
        bool active = true;
    };

    template <typename T>
    struct PersonalState
    {
        PersonalState(T* in) {
            behaviour = in;
        }
        T* behaviour;
        State<T>* state = nullptr;
        double timer = 0;
    };

    template <typename T>
    class FSM
    {
        using ConditionFunc = bool (T::*)() const;
        using VoidFunc = void (T::*)();
    public:
        FSM();
        ~FSM();

        void RegisterState(State<T>* statesin);
        void RegisterStates(std::initializer_list<State<T>*> statesin);
        void AddTransition(State<T>* state_from, State<T>* state_to, ConditionFunc cond,
            VoidFunc on_transition = nullptr);

        void AddTimedTransition(State<T>* state_from, State<T>* state_to,
            double interval, VoidFunc on_transition = nullptr);

        //void SetInitialState(State<T>* state);
        void Init(PersonalState<T>& curr, State<T>*& prev);
        void Update(PersonalState<T>& curr, State<T>*& prev, double dt);

        void ForceTransition(PersonalState<T>& curr, State<T>* state_to, VoidFunc on_transition = nullptr);

    private:
        struct Transition
        {
            State<T>* state_from = nullptr;
            State<T>* state_to = nullptr;
            ConditionFunc condition = nullptr;
            VoidFunc uponTransition = nullptr;
        };
        struct TimedTransition
        {
            State<T>* state_from = nullptr;
            State<T>* state_to = nullptr;
            double interval = 0;
            VoidFunc uponTransition = nullptr;
        };

        Transition create_transition(State<T>* state_from, State<T>* state_to,
            ConditionFunc cond, VoidFunc on_transition);
        TimedTransition create_timed_transition(State<T>* state_from, State<T>* state_to,
            double interval, VoidFunc on_transition);

        void DoTransition(PersonalState<T>& curr, State<T>* const state_to, VoidFunc const uponTransition);
        void CheckTransitions(PersonalState<T>& curr, State<T>*& prev);
        void CheckTimeTransitions(PersonalState<T>& curr, State<T>*& prev, double dt);

    private:
        std::vector<State<T>*> States;
        std::vector<Transition> transitions;
        std::vector<TimedTransition> timed_transitions;

        /*State<T>* current_state = nullptr;*/
        //StateID total_states = 0;
    };

    template <typename T>
    FSM<T>::FSM()
    {
    }

    template <typename T>
    FSM<T>::~FSM()
    {
    }

    template <typename T>
    void FSM<T>::RegisterState(State<T>* statesin)
    {
        if (std::any_of(States.begin(), States.end(), [&](auto const& a) {return statesin == a; }))
            AssertCore(false, "Registering the same State address into the FSM.");
        States.emplace_back(statesin);
    }

    template <typename T>
    void FSM<T>::RegisterStates(std::initializer_list<State<T>*> statesin)
    {
        for (auto& a : statesin) {
            if (std::any_of(States.begin(), States.end(), [&](auto const& b) {return a == b; }))
                AssertCore(false, "Registering the same State address into the FSM.");
            States.emplace_back(a);
        }
    }

    template <typename T>
    void FSM<T>::AddTransition(State<T>* state_from, State<T>* state_to, ConditionFunc cond, VoidFunc on_transition)
    {
       /* if (state_to == nullptr)
            AssertCore(false, "Adding a nullptr state to transit to, into FSM");*/
        if (state_to != nullptr && std::none_of(States.begin(), States.end(), [&](auto const& a) {return state_to == a; }))
            AssertCore(false, "Adding a State to transit to without registering state in FSM");
        if (state_from != nullptr && std::none_of(States.begin(), States.end(), [&](auto const& a) {return state_from == a; }))
            AssertCore(false, "Adding a State to transit from without registering state in FSM");

        Transition transition = create_transition(state_from, state_to, cond, on_transition);
        transitions.emplace_back(transition);
    }

    template <typename T>
    void FSM<T>::AddTimedTransition(State<T>* state_from, State<T>* state_to,
        double interval, VoidFunc on_transition)
    {
       /* if (state_to == nullptr)
            AssertCore(false, "Adding a nullptr state to transit to, into FSM");*/
        if (state_to != nullptr && std::none_of(States.begin(), States.end(), [&](auto const& a) {return state_to == a; }))
            AssertCore(false, "Adding a State to transit to without registering state in FSM");
        if (state_from != nullptr && std::none_of(States.begin(), States.end(), [&](auto const& a) {return state_from == a; }))
            AssertCore(false, "Adding a State to transit from without registering state in FSM");

        TimedTransition transition = create_timed_transition(state_from, state_to, interval, on_transition);
        timed_transitions.emplace_back(transition);
        std::sort(timed_transitions.begin(), timed_transitions.end(), [](const TimedTransition& a, const TimedTransition& b) {return a.interval < b.interval; });
    }

    //template <typename T>
    //void FSM<T>::SetInitialState(State<T>* state) {
    //    current_state = state;
    //}
    template <typename T>
    void FSM<T>::Init(PersonalState<T>& curr, State<T>*& prev)
    {
        if (!States.empty())
            curr.state = States.front();
        prev = curr.state;
        curr.state->Enter(*(curr.behaviour));
    }

    template <typename T>
    void FSM<T>::Update(PersonalState<T>& curr, State<T>*& prev, double dt)
    {
        CheckTransitions(curr, prev);
        CheckTimeTransitions(curr, prev, dt);   

        if (curr.state->GetActive())
            curr.state->Update(*(curr.behaviour), dt);
    }

    template <typename T>
    void FSM<T>::ForceTransition(PersonalState<T>& curr, State<T>* state_to, VoidFunc on_transition)
    {
        if (std::none_of(States.begin(), States.end(), [&](auto const& a) {return state_to == a; }))
            AssertCore(false, "Force transiting to a State that is not registered in FSM");

        DoTransition(curr, state_to, on_transition);
    }

    template <typename T>
    FSM<T>::Transition FSM<T>::create_transition(State<T>* state_from, State<T>* state_to, ConditionFunc cond, VoidFunc on_transition)
    {
        Transition t{};
        t.state_from = state_from;
        t.state_to = state_to;
        t.condition = cond;
        t.uponTransition = on_transition;

        return t;
    }
    template <typename T>
    FSM<T>::TimedTransition FSM<T>::create_timed_transition(State<T>* state_from, State<T>* state_to,
        double _interval, VoidFunc on_transition)
    {
        TimedTransition t{};
        t.state_from = state_from;
        t.state_to = state_to;
        t.interval = _interval;
        t.uponTransition = on_transition;

        return t;
    }

    template <typename T>
    void FSM<T>::CheckTransitions(PersonalState<T>& curr, State<T>*& prev)
    {
        for (Transition& trans : transitions)
        {
            State<T>* to_state = trans.state_to == nullptr ? prev : trans.state_to;
            if (to_state->GetActive() && ((trans.state_from == nullptr && to_state != curr.state)
                || trans.state_from == curr.state))
            {
                ConditionFunc check = trans.condition;
                if (check != nullptr && (curr.behaviour->*check)()) {
                    prev = curr.state;
                    DoTransition(curr, to_state, trans.uponTransition);
                    break;
                }
            }
        }
    }

    template <typename T>
    void FSM<T>::CheckTimeTransitions(PersonalState<T>& curr, State<T>*& prev, double dt)
    {
        curr.timer += dt;
        for (TimedTransition& trans : timed_transitions)
        {
            State<T>* to_state = trans.state_to == nullptr ? prev : trans.state_to;
            if (to_state->GetActive() && to_state != curr.state &&
                (trans.state_from == curr.state || trans.state_from == nullptr))
            {
                if (curr.timer >= trans.interval) {
                    prev = curr.state;
                    DoTransition(curr, to_state, trans.uponTransition);
                    break;
                }
            }
        }
    }



    template <typename T>
    void FSM<T>::DoTransition(PersonalState<T>& curr, State<T>* const state_to, VoidFunc const uponTransition)
    {
        // Execute the handlers in the correct order.

        curr.state->Leave(*(curr.behaviour));

        if (uponTransition != nullptr)
            (curr.behaviour->*uponTransition)();

        curr.state = state_to;
        curr.state->Enter(*(curr.behaviour));

        //Initialice all timed transitions from m_current_state
        curr.timer = 0;
    }

}

#endif