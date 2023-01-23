
#include "BaseMesh.h"

class TPlane : public BaseMesh
{

public:
	// Slightly adapted from the original plane mesh, to allow for Tessellation

	/** \brief Initialises and builds a plane mesh
	*
	* Can specify resolution of plane, this deteremines how many subdivisions of the plane.
	* @param device is the renderer device
	* @param device context is the renderer device context
	* @param resolution is a int for subdivision of the plane. The number of unit quad on each axis. Default is 100.
	*/
	TPlane(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int resolution = 100);
	~TPlane();

protected:
	void initBuffers(ID3D11Device* device);
	int resolution;
};