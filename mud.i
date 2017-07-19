%module mud
%{
        #include "mud.h"
%}

%rename (Stone) mud_new;

%include "mud.h"
%extend Stone
{
        void add_vertex( float x, float y, float z)
        {
                mud_add_vertex($self, x, y, z);
        }
};








