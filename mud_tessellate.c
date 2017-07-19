#include <string.h>
#include "opengl.h"
#include "mud.h"
#include "llist.h"

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#define MUD_TESS_FAN 1
#define MUD_TESS_STRIP 2
#define MUD_TESS_TRI 3
#define MUD_TESS_DB 1

static t_mud *MUD = NULL;
static mud_vertex *a;
static mud_vertex *b;
static mud_vertex *c;

static int db = 0;
static int TESS;
static mud_vertex *fan_start = NULL;
static mud_vertex *fan_a = NULL;
static mud_vertex *fan_b = NULL;
static int TESS_EVEN = 1;
static double tess_tolerance = 0;
static GLenum tess_winding_rule = GLU_TESS_WINDING_NONZERO;

void mud_tess_debug( int val)
{
	db = val;
}

void mud_tess_set( const char *what, void *data)
{
	if( strcmp( what, "tolerance") == 0)
	{
		tess_tolerance = *((double *) data);
	}
	else if( strcmp( what, "winding") == 0)
	{
		char *which = (char *) data;
		if( strcmp( which, "odd") != -1) tess_winding_rule = GLU_TESS_WINDING_ODD;
		else if( strcmp( which, "nonzero") != -1) tess_winding_rule = GLU_TESS_WINDING_NONZERO;
		else if( strcmp( which, "positive") != -1) tess_winding_rule = GLU_TESS_WINDING_POSITIVE;
		else if( strcmp( which, "negative") != -1) tess_winding_rule = GLU_TESS_WINDING_NEGATIVE;
		else if( strcmp( which, "abs") != -1) tess_winding_rule = GLU_TESS_WINDING_ABS_GEQ_TWO;
		else printf("Mud tesselate error: can't set winding rule\n");
	}
}

static const char *get_rule( void)
{
	static const char *odd = "odd";
	static const char *nonzero = "nonzero";
	static const char *positive = "positive";
	static const char *negative = "negative";
	static const char *abs = "abs";
	static const char *unknown = "unknown";
	switch(tess_winding_rule)
	{
		case GLU_TESS_WINDING_ODD: return odd; break;
		case GLU_TESS_WINDING_NONZERO: return nonzero; break;
		case GLU_TESS_WINDING_POSITIVE: return positive; break;
		case GLU_TESS_WINDING_NEGATIVE: return negative; break;
		case GLU_TESS_WINDING_ABS_GEQ_TWO: return abs; break;
		default: return unknown; break;
	}
}

const char* getPrimitiveType(GLenum type)
{
    switch(type)
    {
    case 0x0000:
        return "GL_POINTS";
        break;
    case 0x0001:
        return "GL_LINES";
        break;
    case 0x0002:
        return "GL_LINE_LOOP";
        break;
    case 0x0003:
        return "GL_LINE_STRIP";
        break;
    case 0x0004:
        return "GL_TRIANGLES";
        break;
    case 0x0005:
        return "GL_TRIANGLE_STRIP";
        break;
    case 0x0006:
        return "GL_TRIANGLE_FAN";
        break;
    case 0x0007:
        return "GL_QUADS";
        break;
    case 0x0008:
        return "GL_QUAD_STRIP";
        break;
    case 0x0009:
        return "GL_POLYGON";
        break;
    default:
	return "???";
		break;
    }
}

void mud_tess_begin( GLenum type)
{
	if( db) printf("begin %s\n", getPrimitiveType(type));

	if( type == 0x0006) TESS = MUD_TESS_FAN;
	else if (type == 0x0005) TESS = MUD_TESS_STRIP;
	else if (type == 0x0004) TESS = MUD_TESS_TRI;
	else TESS = 999;

	a = NULL;
	b = NULL;
	c = NULL;

	fan_start = NULL;
	fan_a = NULL;
	fan_b = NULL;

	TESS_EVEN = 1;
}

void mud_tess_reset( void)
{
	fan_start = NULL;
	fan_a = NULL;
	fan_b = NULL;

	a = NULL;
	b = NULL;
	c = NULL;


}

void mud_tesmud_vertex( void *vertex)
{
	//if(db) printf("point\n");
	if( TESS == MUD_TESS_FAN)
	{
		if( fan_start == NULL)
		{
			fan_start = (mud_vertex *) vertex;
		}
		else if( fan_a == NULL)
		{
			fan_a = ( mud_vertex *) vertex;
		}
		else
		{
			fan_b = ( mud_vertex *) vertex;
			if(db) printf("add face %d %d %d\n", fan_start->indice, fan_a->indice, fan_b->indice);
			mud_add_face( MUD, fan_start, fan_a, fan_b, NULL);
			fan_a = vertex;
		}
	}
	else if( TESS == MUD_TESS_STRIP)
	{
		if( fan_start == NULL)
		{
			fan_start = (mud_vertex *) vertex;
		}
		else if( fan_a == NULL)
		{
			fan_a = ( mud_vertex *) vertex;
		}
		// fisrt
		else if( fan_b == NULL)
		{
			fan_b = ( mud_vertex *) vertex;
			if(db) printf("add face %d %d %d\n", fan_start->indice, fan_a->indice, fan_b->indice);
			mud_add_face( MUD, fan_start, fan_a, fan_b, NULL);
		}
		// next
		else
		{
			if( TESS_EVEN)
			{
				TESS_EVEN = 0;
				fan_start = fan_b;
				fan_a = fan_a;
				fan_b = ( mud_vertex *) vertex;
				mud_add_face( MUD, fan_start, fan_a, fan_b, NULL);
				if(db) printf("add face even %d %d %d\n", fan_start->indice, fan_a->indice, fan_b->indice);
			}
			else
			{
				TESS_EVEN = 1;
				fan_start = fan_start;
				fan_a = fan_b;
				fan_b = ( mud_vertex *) vertex;
				mud_add_face( MUD, fan_start, fan_a, fan_b, NULL);
				if(db) printf("add face odd %d %d %d\n", fan_start->indice, fan_a->indice, fan_b->indice);
			}
		}

	}
	else if( TESS == MUD_TESS_TRI)
	{
		if( fan_start == NULL)
		{
			fan_start = (mud_vertex *) vertex;
		}
		else if( fan_a == NULL)
		{
			fan_a = ( mud_vertex *) vertex;
		}
		else
		{
			fan_b = ( mud_vertex *) vertex;
			if(db) printf("add face %d %d %d\n", fan_start->indice, fan_a->indice, fan_b->indice);
			mud_add_face( MUD, fan_start, fan_a, fan_b, NULL);
		}
	}
	else
	{
		if(db) printf("tessellation not implemented\n");

	}

}

void mud_tess_end( void)
{
	if(db) printf("tess end\n");
}

void mud_tess_combine( void)
{
	if(db) printf("tess combine ???\n");
}

void mud_tess_error( GLenum err)
{
	if(db) printf("tess ERROR: %s\n\n", gluErrorString(err));
}

void mud_tessellate( t_mud *mud)
{
	mud_tess_reset();
	GLUtesselator* t = gluNewTess();

	gluTessCallback(t, GLU_TESS_BEGIN_DATA, (GLvoid (*)()) mud_tess_begin);
	gluTessCallback(t, GLU_TESS_VERTEX_DATA, (GLvoid (*)()) mud_tesmud_vertex);
	gluTessCallback(t, GLU_TESS_COMBINE_DATA, (GLvoid (*)()) mud_tess_combine);
	gluTessCallback(t, GLU_TESS_END_DATA, (GLvoid (*)()) mud_tess_end);
	gluTessCallback(t, GLU_TESS_ERROR_DATA, (GLvoid (*)()) mud_tess_error);

	if(db)
	{
		printf("[mud] tess tolerance:%f\n", tess_tolerance);
		printf("[mud] tess rule:%s\n", get_rule());
	}

	gluTessProperty(t, GLU_TESS_WINDING_RULE, tess_winding_rule);

	gluTessProperty(t, GLU_TESS_TOLERANCE, tess_tolerance);
	gluTessNormal(t, 0.0f, 0.0f, 1.0f);

	gluTessBeginPolygon(t, NULL);
	gluTessBeginContour(t);

	mud_vertex *vertex;
	t_lnode *node;
	MUD = mud;

	for( node = mud->vertex->first;node;node=node->next)
	{
		vertex = ( mud_vertex *) node->data;
		if(db) printf("add point indice:%d %f %f %f\n", vertex->indice, vertex->co[0],vertex->co[1],0.0);
		double p[3] = {(double)vertex->co[0],(double)vertex->co[1],0};
		gluTessVertex(t,p,vertex);
	}

	gluTessEndContour(t);
	gluTessEndPolygon(t);
	gluDeleteTess(t);
}

