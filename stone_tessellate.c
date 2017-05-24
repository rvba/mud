#include "opengl.h"
#include "stone.h"
#include "llist.h"

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#define STONE_TESS_FAN 1
#define STONE_TESS_STRIP 2
#define STONE_TESS_TRI 3
#define STONE_TESS_DB 1

static t_stone *STONE = NULL;
static s_vertex *a;
static s_vertex *b;
static s_vertex *c;

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

static int db = 0;
static int TESS;
static s_vertex *fan_start = NULL;
static s_vertex *fan_a = NULL;
static s_vertex *fan_b = NULL;
static int TESS_EVEN = 1;

void stone_tess_begin( GLenum type)
{
	if( db) printf("begin %s\n", getPrimitiveType(type));

	if( type == 0x0006) TESS = STONE_TESS_FAN;
	else if (type == 0x0005) TESS = STONE_TESS_STRIP;
	else if (type == 0x0004) TESS = STONE_TESS_TRI;
	else TESS = 999;

	a = NULL;
	b = NULL;
	c = NULL;
}

void stone_tess_vertex( void *vertex)
{
	if(db) printf("point\n");
	if( TESS == STONE_TESS_FAN)
	{
		if( fan_start == NULL)
		{
			fan_start = (s_vertex *) vertex;
		}
		else if( fan_a == NULL)
		{
			fan_a = ( s_vertex *) vertex;
		}
		else
		{
			fan_b = ( s_vertex *) vertex;
			if(db) printf("add face %d %d %d\n", fan_start->indice, fan_a->indice, fan_b->indice);
			stone_add_face( STONE, fan_start, fan_a, fan_b, NULL);
			fan_a = vertex;
		}
	}
	else if( TESS == STONE_TESS_STRIP)
	{
		if( fan_start == NULL)
		{
			fan_start = (s_vertex *) vertex;
		}
		else if( fan_a == NULL)
		{
			fan_a = ( s_vertex *) vertex;
		}
		// fisrt
		else if( fan_b == NULL)
		{
			fan_b = ( s_vertex *) vertex;
			if(db) printf("add face %d %d %d\n", fan_start->indice, fan_a->indice, fan_b->indice);
			stone_add_face( STONE, fan_start, fan_a, fan_b, NULL);
		}
		// next
		else
		{
			if( TESS_EVEN)
			{
				TESS_EVEN = 0;
				fan_start = fan_b;
				fan_a = fan_a;
				fan_b = ( s_vertex *) vertex;
				stone_add_face( STONE, fan_start, fan_a, fan_b, NULL);
			}
			else
			{
				TESS_EVEN = 1;
				fan_start = fan_start;
				fan_a = fan_b;
				fan_b = ( s_vertex *) vertex;
				stone_add_face( STONE, fan_start, fan_a, fan_b, NULL);
			}
		}

	}
	else if( TESS == STONE_TESS_TRI)
	{
		if( fan_start == NULL)
		{
			fan_start = (s_vertex *) vertex;
		}
		else if( fan_a == NULL)
		{
			fan_a = ( s_vertex *) vertex;
		}
		else
		{
			fan_b = ( s_vertex *) vertex;
			stone_add_face( STONE, fan_start, fan_a, fan_b, NULL);
		}
	}
	else
	{
		if(db) printf("tessellation not implemented\n");

	}

}

void stone_tess_end( void)
{
	if(db) printf("tess end\n");
}

void stone_tessellate( t_stone *stone)
{
	GLUtesselator* t = gluNewTess();

	gluTessCallback(t, GLU_TESS_BEGIN_DATA, (GLvoid (*)()) stone_tess_begin);
	gluTessCallback(t, GLU_TESS_VERTEX_DATA, (GLvoid (*)()) stone_tess_vertex);
	//gluTessCallback(t, GLU_TESS_COMBINE_DATA, (GLvoid (*)()) cb_combine);
	gluTessCallback(t, GLU_TESS_END_DATA, (GLvoid (*)()) stone_tess_end);
	//gluTessCallback(t, GLU_TESS_ERROR_DATA, (GLvoid (*)()) cb_error);

	gluTessProperty(t, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_NONZERO);

	gluTessProperty(t, GLU_TESS_TOLERANCE, 0);
	gluTessNormal(t, 0.0f, 0.0f, 1.0f);

	gluTessBeginPolygon(t, NULL);
	gluTessBeginContour(t);

	s_vertex *vertex;
	t_lnode *node;
	STONE = stone;

	for( node = stone->vertex->first;node;node=node->next)
	{
		vertex = ( s_vertex *) node->data;
		if(db) printf("add point %d\n", vertex->indice);
		double p[3] = {(double)vertex->co[0],(double)vertex->co[1],0};
		gluTessVertex(t,p,vertex);
	}

	gluTessEndContour(t);
	gluTessEndPolygon(t);
	gluDeleteTess(t);
}

