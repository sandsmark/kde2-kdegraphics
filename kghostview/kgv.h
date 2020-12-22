#ifndef KGV_H
#define KGV_H

#include "kmaybe.h"

class KGV {
public:
    enum PaletteType {
	MONOCHROME, 
	GRAYSCALE, 
	COLOR 
    };
   
    /**
     * PORTRAIT are LANDSCAPE are defined in the DSC specs. UPSIDEDOWN
     * and SEASCAPE are added to use for viewing.
     */
    enum Orientation {
	PORTRAIT = 0, 
	LANDSCAPE = 90, 
	UPSIDEDOWN = 180, 
	SEASCAPE = 270 
    };

    /**
     * PageOrder as defined in the DSC specs.
     */
    enum PageOrder {
	ASCEND,
	DESCEND,
	SPECIAL
    };

    /**
     * Convenience BoundingBox class to be used instead of an array of
     * size 4. Using this type makes it a lot easier to pass boundingboxes
     * around.
     */
    class BoundingBox
    {
    public:
	BoundingBox() { 
	    _llx = 0; _lly = 0; _urx = 0; _ury = 0; 
	}
	BoundingBox( const BoundingBox& b ) { 
	    _llx = b._llx; _lly = b._lly; _urx = b._urx; _ury = b._ury; 
	}
	BoundingBox( int llx, int lly, int urx, int ury ) { 
	    _llx = llx; _lly = lly; _urx = urx; _ury = ury; 
	}
	BoundingBox( int bbox[4] ) { 
	    _llx = bbox[0]; _lly = bbox[1]; 
	    _urx = bbox[2]; _ury = bbox[3]; 
	}

	BoundingBox& operator = ( const BoundingBox& b ) { 
	    _llx = b._llx; _lly = b._lly; _urx = b._urx; _ury = b._ury;
	    return *this; 
	}
	
	bool operator == ( const BoundingBox& b ) { 
	    return ( _llx == b._llx && _lly == b._lly 
		  && _urx == b._urx && _ury == b._ury ); 
	}
	bool operator != ( const BoundingBox& b ) { 
	    return !( *this == b ); 
	}
	
	int llx() const { return _llx; }
	int lly() const { return _lly; }
	int urx() const { return _urx; }
	int ury() const { return _ury; }

	int width()  const { return _urx - _llx; }
	int height() const { return _ury - _lly; }

    private:
	int _llx, _lly, _urx, _ury;
    };

    class FileOffset
    {
    public:
	FileOffset() { 
	    _begin = 0; _end = 0; _length = 0; 
	}
	FileOffset( long begin, long end, unsigned int length ) { 
	    _begin = begin; _end = end; _length = length; 
	}
	
	long begin() const	    { return _begin; }
	long end() const	    { return _end; }
	unsigned int length() const { return _length; }
	
    private:
	long _begin, _end;
	unsigned int _length;
    };

    /**
     * Some typedefs which wrap the types used in the DSC scanner in a
     * KMaybe container. They are postfixed with an 'M' (of Maybe). Note
     * that QString is also a kind of Maybe type, since it can be a Null 
     * string.
     */
    typedef KMaybe< Orientation >   OrientationM;
    typedef KMaybe< PageOrder >	    PageOrderM;
    typedef KMaybe< BoundingBox >   BoundingBoxM;
    typedef KMaybe< FileOffset >    FileOffsetM;
};

#endif
