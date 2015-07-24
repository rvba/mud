%module stone
%{
        #include "stone.h"
%}

%rename (Stone) stone_new;

%include "stone.h"
%extend Stone
{
        void add_vertex( float x, float y, float z)
        {
                stone_add_vertex($self, x, y, z);
        }
};








