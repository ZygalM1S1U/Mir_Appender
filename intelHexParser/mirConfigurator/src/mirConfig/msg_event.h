#ifndef _MSG_EVENT_H_
#define _MSG_EVENT_H_

#include <map>

typedef int MsgEventHandler;

template <typename ParamT>
class EventHandlerBase
{
public:
    virtual void notify(ParamT param) = 0;
};

template <typename ParamT, typename ParamT2>
class EventHandlerBase2
{
public:
    virtual void notify(ParamT param, ParamT2 param1) = 0;
};

template <typename ListenerT,typename ParamT>
class EventHandler : public EventHandlerBase<ParamT>
{
    typedef void (ListenerT::*PtrMember)(ParamT);
    ListenerT* m_object;
    PtrMember m_member;

public:

    EventHandler(ListenerT* object, PtrMember member)
        : m_object(object), m_member(member)
    {}

    void notify(ParamT param)
    {
        return (m_object->*m_member)(param);
    }
};

template <typename ListenerT,typename ParamT,typename ParamT2>
class EventHandler2 : public EventHandlerBase2<ParamT,ParamT2>
{
    typedef void (ListenerT::*PtrMember)(ParamT,ParamT2);
    ListenerT* m_object;
    PtrMember m_member;

public:

    EventHandler2(ListenerT* object, PtrMember member)
        : m_object(object), m_member(member)
    {}

    void notify(ParamT param, ParamT2 param2)
    {
        return (m_object->*m_member)(param, param2);
    }
};

///@brief  This is class is used to provide a way for other objects to "subscribe" to notify events with
/// specified callback methods (mainly used for subscribing to network message types).
/// This class provides a method to pass one parameter.  See MsgEvent2 for passing two parameters.
template <typename ParamT>
class MsgEvent
{
    typedef std::map<int,EventHandlerBase<ParamT> *> HandlersMap;
    HandlersMap m_handlers;
    int m_count;

public:

    MsgEvent()
        : m_count(0) {}

    template <typename ListenerT>
    MsgEventHandler subscribe(ListenerT* object,void (ListenerT::*member)(ParamT))
    {
        typedef void (ListenerT::*PtrMember)(ParamT);
        m_handlers[m_count] = (new EventHandler<ListenerT,ParamT>(object,member));
        m_count++;
        return m_count-1;
    }

    bool unsubscribe(MsgEventHandler id)
    {
        typename HandlersMap::iterator it = m_handlers.find(id);

        if(it == m_handlers.end())
            return false;

        delete it->second;
        m_handlers.erase(it);
        return true;
    }

    void notify(ParamT param)
    {
        if (m_handlers.size() > 0)
        {
            typename HandlersMap::iterator it = m_handlers.begin();
            for(; it != m_handlers.end(); it++)
            {
                it->second->notify(param);
            }
        }
    }
};

///@brief  This is class is used to provide a way for other objects to "subscribe" to notify events with
/// specified callback methods (mainly used for subscribing to network message types).
/// This class provides a method to pass two parameters.  See MsgEvent for passing one parameter.
template <typename ParamT,typename ParamT2>
class MsgEvent2
{
    typedef std::map<int,EventHandlerBase2<ParamT,ParamT2> *> HandlersMap;
    HandlersMap m_handlers;
    int m_count;

public:

    MsgEvent2()
        : m_count(0) {}

    template <typename ListenerT>
    MsgEventHandler subscribe(ListenerT* object,void (ListenerT::*member)(ParamT,ParamT2))
    {
        typedef void (ListenerT::*PtrMember)(ParamT,ParamT2);
        m_handlers[m_count] = (new EventHandler2<ListenerT,ParamT,ParamT2>(object,member));
        m_count++;
        return m_count-1;
    }

    bool unsubscribe(MsgEventHandler id)
    {
        typename HandlersMap::iterator it = m_handlers.find(id);

        if(it == m_handlers.end())
            return false;

        delete it->second;
        m_handlers.erase(it);
        return true;
    }

    void notify(ParamT param, ParamT2 param2)
    {
        if (m_handlers.size() == 1)
        {
            m_handlers[0]->notify(param, param2);
        }
        else
        {
            typename HandlersMap::iterator it = m_handlers.begin();
            for(; it != m_handlers.end(); it++)
            {
                it->second->notify(param, param2);
            }
        }
    }
};

#endif
