#ifndef STUDENT_CODE_H
#define STUDENT_CODE_H

#include <functional>

#include "halfEdgeMesh.h"
#include "bezierPatch.h"
#include "bezierCurve.h"

using namespace std;

namespace CGL {

  /**
   * Weight function for the Butterfly edge stencil.
   *
   * ring 0: the two endpoints of the edge (classic weight 1/2)
   * ring 1: the two vertices opposite the edge (classic weight 1/8)
   * ring 2: the four "wing" vertices (classic weight -1/16)
   *
   * A custom function may be passed to MeshResampler::butterfly(); its
   * weights should satisfy 2*w(0) + 2*w(1) + 4*w(2) == 1.
   */
  typedef std::function<double(int)> ButterflyWeightFunction;

  /// Classical (non-modified) Butterfly stencil weights.
  ButterflyWeightFunction classicButterflyWeights();

  class MeshResampler{

  public:

    MeshResampler(){};
    ~MeshResampler(){}

    void upsample(HalfedgeMesh& mesh);

    /**
     * Butterfly subdivision. The topology is the same 4-1 split used by
     * Loop subdivision (split every edge, then flip new old-new edges),
     * but the edge points are computed with the Butterfly stencil and the
     * original vertices are interpolated (kept fixed).
     *
     * @param weights stencil weights, see ButterflyWeightFunction
     */
    void butterfly(HalfedgeMesh& mesh,
                   const ButterflyWeightFunction& weights = classicButterflyWeights());
  };
}

#endif // STUDENT_CODE_H
