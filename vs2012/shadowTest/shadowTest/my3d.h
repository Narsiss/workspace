#include <math3d.h>
/////////////////////////////////////////////////////////////////////////////
// Planar and 1/w shadow Matrix
void m3dMakeShadowMatrix(M3DMatrix44f proj, const M3DVector4f planeEq, const M3DVector3f vLightPos);
void m3dMakeShadowMatrix(M3DMatrix44d proj, const M3DVector4d planeEq, const M3DVector3d vLightPos);
void m3dMatrixMultiply14(float* product,const float* a,const M3DMatrix44f b);
void m3dMatrixMultiply14(double* product,const double* a,const M3DMatrix44d b);
