#pragma once

#include "Common.h"

class EventDispatcherBase : public IDestructible
{
  public:
    ~EventDispatcherBase() override = default;

    virtual void RemoveListenersWithContext(void* p_Context) = 0;

  protected:
    struct EventListenerRegistration
    {
        void* m_Context;
        void* m_Listener;
    };

  protected:
    virtual void AddListenerInternal(void* p_Context, void* p_Listener) = 0;
    virtual void RemoveListenerInternal(void* p_Listener) = 0;
    virtual EventListenerRegistration** GetRegistrations() = 0;
    virtual void LockForCall() = 0;
    virtual void UnlockForCall() = 0;

    friend class EventDispatcherRegistry;
};

/**
 * A thread-safe event listener registry.
 * NOTE: Registering a listener from a listener callback will result in a deadlock.
 */
template<class... Args> class EventDispatcher : public EventDispatcherBase
{
  public:
    using EventListener = void (*)(void*, Args...);

    EventListener AddListener(void* p_Context, EventListener p_Listener)
    {
        AddListenerInternal(p_Context, p_Listener);
        return p_Listener;
    }

    void RemoveListener(EventListener p_Listener)
    {
        RemoveListenerInternal(p_Listener);
    }

    void Call(Args... p_Args)
    {
        LockForCall();

        const auto* registrations = GetRegistrations();

        auto* registration = *registrations;

        while (registration != nullptr)
        {
            const auto listener = static_cast<EventListener>(registration->m_Listener);
            listener(registration->m_Context, p_Args...);
            registration = *++registrations;
        }

        UnlockForCall();
    }
};

template<> class EventDispatcher<void> : public EventDispatcherBase
{
  public:
    using EventListener = void (*)(void*);

    EventListener AddListener(void* p_Context, EventListener p_Listener)
    {
        AddListenerInternal(p_Context, p_Listener);
        return p_Listener;
    }

    void RemoveListener(EventListener p_Listener)
    {
        RemoveListenerInternal(p_Listener);
    }

    void Call()
    {
        LockForCall();

        const auto* registrations = GetRegistrations();

        auto* registration = *registrations;

        while (registration != nullptr)
        {
            const auto listener = static_cast<EventListener>(registration->m_Listener);
            listener(registration->m_Context);
            registration = *++registrations;
        }

        UnlockForCall();
    }
};
