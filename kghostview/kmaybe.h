#ifndef KMAYBE_H
#define KMAYBE_H

#include <assert.h>
#include <iostream.h>

/**
 * Maybe template class. It is used instead of a pointer to model variables 
 * which can have either no or exactly one value assigned to them.
 */
template< class Type > 
class KMaybe
{
public:
    /**
     * Constructs an empty container.
     */
    KMaybe() { 
	_dataPtr = 0; 
    }
    
    /**
     * Constructs a container containing a copy of t.
     */
    KMaybe( const Type& t ) { 
	_dataPtr = new Type( t ); 
    }
    
    /**
     * Constructs a deep copy of m.
     */
    KMaybe( const KMaybe< Type >& m ) {
	if( m.isNull() )
	    _dataPtr = 0;
	else
	    _dataPtr = new Type( m.data() );
    }

    ~KMaybe() { 
	setNull();
    }
 
    KMaybe< Type >& operator = ( const Type& t ) {
	if( isNull() ) 
	    _dataPtr = new Type( t );
	else 
	    *_dataPtr = t;
	return (*this); 
    }
    
    KMaybe< Type >& operator = ( const KMaybe< Type >& m ) {
	if( *this != m )
	    if( m.isNull() ) 
		setNull();
	    else
		operator = ( m.data() );
	return (*this);
    }

    bool operator == ( const Type& t ) const { 
	if( isNull() ) return false;
	else return ( data() == t ); 
    }
    
    bool operator != ( const Type& t ) const {
	if( isNull() ) return true;
	else return ( data() != t ); 
    }
    
    bool operator == ( const KMaybe< Type >& t ) const {
	if( isNull() && t.isNull() ) return true;
	else if( isNull() || t.isNull() ) return false;
	else return ( data() == t.data() ); 
    }
    
    bool operator != ( const KMaybe< Type >& t ) const {
	if( isNull() && t.isNull() ) return false;
	else if( isNull() || t.isNull() ) return true;
	else return ( data() != t.data() );
    }
    
    /**
     * Returns true if the container doesn't contain a value, false otherwise.
     */
    bool operator ! () const { 
	return isNull(); 
    }
    
    /**
     * Returns the contents of the container. You should check first if 
     * the container isn't empty with isNull().
     */
    Type data() const { 
	assert( _dataPtr != 0 ); return *_dataPtr; 
    }
    
    /**
     * Operator which can be used instead of data().
     */
    Type& operator * () { 
	return data(); 
    }
    const Type& operator * () const { 
	return data(); 
    }

    Type* operator -> () {
	assert( _dataPtr != 0 ); return _dataPtr; 
    }
    const Type* operator -> () const {
	assert( _dataPtr != 0 ); return _dataPtr; 
    }
    
    /**
     * Returns true if the container doesn't contain a value, false otherwise.
     */
    bool isNull() const { 
	return _dataPtr == 0;
    }
   
    /**
     * Reset the contents of the container to Null.
     */
    void setNull() {
	if( _dataPtr ) {
	    delete _dataPtr;
	    _dataPtr = 0;
	}
    }
   
private:
    Type* _dataPtr;
};


template< class Type > 
ostream& operator << ( ostream& os, const KMaybe< Type >& source ) 
{
    if( source.isNull() )
	os << "Null";
    else
	os << source.value();
}

template< class Type > 
istream& operator >> ( istream& is, KMaybe< Type >& destination ) 
{
    Type t;	
    is >> t;
    destination = t;
}

#endif
