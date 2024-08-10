#ifndef D_BUFFER_H
#define D_BUFFER_H


#include <boost/circular_buffer.hpp>
#include <vector>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <list>



namespace sob{

/**
 *  @brief  Adaptor Pattern just like std::queue/std::stack
 *          Use different Engine type to construct the interface of a dBuffer: constant time for insertion/deletion on both ends and also automatically resizes itself when it overflows
 *          This will reallocated on itself, even if the engine is boost::circular_buffer
 */
template <typename T,
            template <typename EleT, typename AllocT = std::allocator<EleT>> 
                class EngineT = boost::circular_buffer >
class dBuffer
{
private:
    mutable EngineT<T> mBuffer;
public:
    dBuffer() = default;
    template <typename... Args>
    dBuffer( Args&&... args ): mBuffer( std::forward<Args...>( args... ) )
    {}

    void push_back( const T& val )
    {
        mBuffer.push_back( val );
    }

    void push_front( const T& value )
    {
        mBuffer.push_front( value );
    }

    using iterator = typename EngineT<T>::iterator;
    using const_iterator = typename EngineT<T>::const_iterator;
    using value_type = typename EngineT<T>::value_type;

    size_t size() const
    {
        return mBuffer.size();
    }

    iterator erase( iterator it )
    {
        return mBuffer.erase( it );
    }

    void pop_front()
    {
        return mBuffer.pop_front();
    }

    T& front() const
    {
        return mBuffer.front();
    }

    iterator begin() const
    {
        return mBuffer.begin();
    }

    iterator end() const
    {
        return mBuffer.end();
    }

    const_iterator cbegin() const
    {
        return mBuffer.cbegin();
    }

    const_iterator cend() const
    {
        return mBuffer.cend();
    }

    friend iterator begin( dBuffer& buf )
    {
        return buf.begin();
    }

    friend const_iterator begin( const dBuffer& buf )
    {
        return buf.cbegin();
    }

    friend iterator end( dBuffer& buf )
    {
        return buf.end();
    }

    friend const_iterator cend( const dBuffer& buf )
    {
        return buf.cend();
    }

}; // class dBuffer

/**
 *  @brief  This is a specialisation on EngineT = boost::circular_buffer
 */
template <typename T>
class dBuffer< T, boost::circular_buffer>
{
private:
    mutable boost::circular_buffer<T> mBuffer;
public:
    dBuffer()
    : mBuffer( 4 )
    {
    }
    template <typename... Args>
    dBuffer( Args&&... args ): mBuffer( std::forward<Args...>( args... ) )
    {}

    void push_back( const T& val )
    {
        if (mBuffer.full()) {
            boost::circular_buffer<T> tmp( mBuffer.size() * 2 );
            std::copy( mBuffer.begin(), mBuffer.end(), std::back_inserter( tmp ) );
            mBuffer.swap( tmp );
        }
        mBuffer.push_back( val );
    }

    void push_front( const T& val )
    {
        if (mBuffer.full()) {
            boost::circular_buffer<T> tmp( mBuffer.size() * 2 );
            std::copy( mBuffer.begin(), mBuffer.end(), std::back_inserter( tmp ) );
            mBuffer.swap( tmp );
        }
        mBuffer.push_front( val );
    }

    using iterator = typename boost::circular_buffer<T>::iterator;
    using const_iterator = typename boost::circular_buffer<T>::const_iterator;
    using value_type = typename boost::circular_buffer<T>::value_type;

    size_t size() const
    {
        return mBuffer.size();
    }

    iterator erase( iterator it )
    {
        return mBuffer.erase( it );
    }

    void pop_front()
    {
        return mBuffer.pop_front();
    }

    T& front() const
    {
        return mBuffer.front();
    }

    // iterator begin()
    // {
    //     return mBuffer.begin();
    // }
    //
    // iterator end()
    // {
    //     return mBuffer.end();
    // }

    iterator begin() const
    {
        return mBuffer.begin();
    }

    iterator end() const
    {
        return mBuffer.end();
    }

    const_iterator cbegin() const
    {
        return mBuffer.cbegin();
    }

    const_iterator cend() const
    {
        return mBuffer.cend();
    }

    friend iterator begin( dBuffer& buf )
    {
        return buf.begin();
    }

    friend const_iterator begin( const dBuffer& buf )
    {
        return buf.cbegin();
    }

    friend iterator end( dBuffer& buf )
    {
        return buf.end();
    }

    friend const_iterator cend( const dBuffer& buf )
    {
        return buf.cend();
    }

}; // class dBuffer

} // namespace sob



#endif
