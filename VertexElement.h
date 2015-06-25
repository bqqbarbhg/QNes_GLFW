#define VERTEX_ELEMENT_USAGE_POSITION 'P'
#define VERTEX_ELEMENT_USAGE_NORMAL 'N'
#define VERTEX_ELEMENT_USAGE_TEXTURE 'T'

struct VertexElement
{
	char usage;
	int index;
	int offset;
};