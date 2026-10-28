#ifndef STUDENT_CODE_H
#define STUDENT_CODE_H

#include "halfEdgeMesh.h"
#include "bezierPatch.h"
#include "bezierCurve.h"

using namespace std;

namespace CGL {

  class MeshResampler{

  public:

    MeshResampler(){};
    ~MeshResampler(){}

    void upsample(HalfedgeMesh& mesh);

    /**
     * Butterfly subdivision. The topology is the same 4-1 split used by
     * Loop subdivision (split every edge, then flip new old-new edges),
     * but the edge points use the classical Butterfly stencil and the
     * original vertices are interpolated (kept fixed).
     */
    void butterfly(HalfedgeMesh& mesh);
  };
}

#endif // STUDENT_CODE_H
