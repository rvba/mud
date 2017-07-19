%module mud
%{
        #include "mud.h"
%}

%rename (Mud) mud_new;

%include "mud.h"
%extend Mud
{
        void add_vertex( float x, float y, float z)
        {
                mud_add_vertex($self, x, y, z);
        }
};








