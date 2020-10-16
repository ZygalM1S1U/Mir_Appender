#ifndef _SINGLETON_H__
#define _SINGLETON_H__

#include <assert.h>

///@brief Template class for creating single-instance global classes.
template < typename T > class Singleton
{
private:
    /** \brief Explicit private copy constructor. This is a forbidden operation.*/
    Singleton (const Singleton < T > &);

    /** \brief Private operator= . This is a forbidden operation. */
    Singleton & operator= (const Singleton < T > &);

protected:

    static T *aSingleton;

public:

    /// @brief: Constructor: Exit program if object already exists.
    Singleton (void)
    {
        assert (!aSingleton);
        aSingleton = static_cast < T * >(this);
    }

    /// @brief: Destructor: Exit program if object does not already exist.
    virtual ~Singleton (void)
    {
        assert (aSingleton);
        aSingleton = 0;
    }

    /// @brief: Return Pointer to single-instance class,
    /// exit program with message if object doesn't exist.
    static T & getSingleton (void)
    {
        assert (aSingleton);
        return (*aSingleton);
    }

    /// @brief: Return Pointer to single-instance class.
    static T *getSingletonPtr (void)
    {
        return aSingleton;
    }
};

#endif
