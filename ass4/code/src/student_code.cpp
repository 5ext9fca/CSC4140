#include "student_code.h"
#include "mutablePriorityQueue.h"

using namespace std;

namespace CGL
{

  template <typename T>
  T lerp( const T& a, const T& b, double f )
  {
    return (1.0-f)*a + f*b;
  }

  /**
   * Evaluates one step of the de Casteljau's algorithm using the given points and
   * the scalar parameter t (class member).
   *
   * @param points A vector of points in 2D
   * @return A vector containing intermediate points or the final interpolated vector
   */
  std::vector<Vector2D> BezierCurve::evaluateStep(std::vector<Vector2D> const &points)
  { 
    // DONE Task 1.
    if (points.size() < 2) {
      return points;
    }

    std::vector<Vector2D> newPoints;
    for (size_t i = 0; i < points.size() - 1; i++) {
      Vector2D newPoint = lerp(points[i], points[i + 1], t);
      
      newPoints.push_back(newPoint);
    }

    return newPoints;
  }

  /**
   * Evaluates one step of the de Casteljau's algorithm using the given points and
   * the scalar parameter t (function parameter).
   *
   * @param points    A vector of points in 3D
   * @param t         Scalar interpolation parameter
   * @return A vector containing intermediate points or the final interpolated vector
   */
  std::vector<Vector3D> BezierPatch::evaluateStep(std::vector<Vector3D> const &points, double t) const
  {
    // DONE Task 2.
    if (points.size() < 2) {
      return points;
    }

    std::vector<Vector3D> newPoints;
    for (size_t i = 0; i < points.size() - 1; i++) {
      Vector3D newPoint = lerp(points[i], points[i + 1], t);
      newPoints.push_back(newPoint);
    }
    return newPoints;
  }

  /**
   * Fully evaluates de Casteljau's algorithm for a vector of points at scalar parameter t
   *
   * @param points    A vector of points in 3D
   * @param t         Scalar interpolation parameter
   * @return Final interpolated vector
   */
  Vector3D BezierPatch::evaluate1D(std::vector<Vector3D> const &points, double t) const
  {
    // DONE Task 2.
    if (points.size() == 0) {
      return Vector3D();
    }

    std::vector<Vector3D> pointsCopy = points;
    while (pointsCopy.size() > 1) {
      std::vector<Vector3D> newPoints = evaluateStep(pointsCopy, t);
      pointsCopy = newPoints;
    }
    return pointsCopy[0];
  }

  /**
   * Evaluates the Bezier patch at parameter (u, v)
   *
   * @param u         Scalar interpolation parameter
   * @param v         Scalar interpolation parameter (along the other axis)
   * @return Final interpolated vector
   */
  Vector3D BezierPatch::evaluate(double u, double v) const 
  {  
    // DONE Task 2.
    std::vector<Vector3D> verticalPoints;
    for (size_t i = 0; i < controlPoints.size(); i++) {
      verticalPoints.push_back(evaluate1D(controlPoints[i], u));
    }
    return evaluate1D(verticalPoints, v);
  }

  /**
   * Get the area vector of a face.
   * \param face The face for which to compute the area vector.
   * \returns the area vector of the face.
   */
  static Vector3D area(const Face& face)
  {
    // DONE Task 3.
    Vector3D areaVector(0, 0, 0);
    HalfedgeCIter h = face.halfedge();
    do {
      Vector3D v0 = h->vertex()->position;
      Vector3D v1 = h->next()->vertex()->position;
      areaVector += cross(v0, v1);
      h = h->next();
    } while (h != face.halfedge());
    return areaVector * 0.5;
  }

  Vector3D Vertex::normal( void ) const
  {
    // DONE Task 3.
    // Returns an approximate unit normal at this vertex, computed by
    // taking the area-weighted average of the normals of neighboring
    // triangles, then normalizing.
    Vector3D normalVector(0, 0, 0);
    HalfedgeCIter h = halfedge();
    do {
      if (!h->face()->isBoundary()) {
        normalVector += area(*h->face());
      }
      h = h->twin()->next();
    } while (h != halfedge());

    if (normalVector.norm() < 1e-12) return Vector3D();
    return normalVector.unit();
  }

  EdgeIter HalfedgeMesh::flipEdge( EdgeIter e0 )
  {
    // DONE Task 4.
    // This method should flip the given edge and return an iterator to the flipped edge.
    if (e0->isBoundary()) {
      return e0;
    }

    HalfedgeIter h0, h1, h2, h3, h4, h5;
    h0 = e0->halfedge();
    h1 = h0->next();
    h2 = h1->next();
    VertexIter v0 = h0->vertex();
    FaceIter f0 = h0->face();
    h3 = h0->twin();
    h4 = h3->next();
    h5 = h4->next();
    VertexIter v3 = h3->vertex();
    FaceIter f1 = h3->face();

    h0->next() = h2;
    h0->vertex() = h5->vertex();

    h1->next() = h3;
    h1->face() = f1;

    h2->next() = h4;

    v0->halfedge() = h4;

    f0->halfedge() = h0;

    h3->next() = h5;
    h3->vertex() = h2->vertex();

    h4->next() = h0;
    h4->face() = f0;

    h5->next() = h1;

    v3->halfedge() = h1;

    f1->halfedge() = h3;

    return e0;
  }

  VertexIter HalfedgeMesh::splitEdge( EdgeIter e0 )
  {
    // DONE Task 5.
    // This method should split the given edge and return an iterator to the newly inserted vertex.
    // The halfedge of this vertex should point along the edge that was split, rather than the new edges.
    if (e0->isBoundary()) {
      HalfedgeIter h0 = e0->halfedge()->isBoundary() ? e0->halfedge()->twin() : e0->halfedge();
      HalfedgeIter h1 = h0->next();
      HalfedgeIter h2 = h1->next();

      HalfedgeIter h3 = newHalfedge();
      HalfedgeIter h4 = newHalfedge();
      HalfedgeIter h5 = newHalfedge();

      HalfedgeIter h6 = h0->twin();
      HalfedgeIter h7 = newHalfedge();

      VertexIter v0 = newVertex();
      v0->position = (h0->vertex()->position + h0->twin()->vertex()->position) / 2.0;

      EdgeIter e1 = newEdge();
      EdgeIter e2 = newEdge();

      FaceIter f0 = h0->face();
      FaceIter f1 = newFace();

      h0->next() = h3;
      h3->next() = h2;

      h1->next() = h4;
      h4->next() = h5;
      h5->next() = h1;

      h7->next() = h6->next();
      h6->next() = h7;

      h0->twin() = h7; h7->twin() = h0;
      h3->twin() = h4; h4->twin() = h3;
      h5->twin() = h6; h6->twin() = h5;

      h3->vertex() = v0;
      h4->vertex() = h2->vertex();
      h5->vertex() = v0;
      h7->vertex() = v0;

      v0->halfedge() = h5;

      h3->edge() = e2; h4->edge() = e2;
      h5->edge() = e1; h6->edge() = e1;
      h7->edge() = e0;

      e0->halfedge() = h0;
      e1->halfedge() = h5;
      e2->halfedge() = h3;

      h3->face() = f0;
      h1->face() = f1;
      h4->face() = f1;
      h5->face() = f1;
      h7->face() = h6->face();

      f0->halfedge() = h0;
      f1->halfedge() = h5;

      return v0;
    }

    HalfedgeIter h0, h1, h2, h3, h4, h5;
    h0 = e0->halfedge();
    h1 = h0->next();
    h2 = h1->next();
    FaceIter f0 = h0->face();

    h3 = h0->twin();
    h4 = h3->next();
    h5 = h4->next();
    FaceIter f1 = h3->face();

    HalfedgeIter h6, h7, h8, h9, h10, h11;
    h6 = newHalfedge();
    h7 = newHalfedge();
    h8 = newHalfedge();
    h9 = newHalfedge();
    h10 = newHalfedge();
    h11 = newHalfedge();

    VertexIter v0 = newVertex();
    v0->position = (h0->vertex()->position + h3->vertex()->position) / 2.0;

    EdgeIter e1 = newEdge(), e2 = newEdge(), e3 = newEdge();

    FaceIter f2 = newFace(), f3 = newFace();

    h0->next() = h6;
    h6->next() = h2;

    h7->next() = h1;
    h1->next() = h8;
    h8->next() = h7;

    h3->next() = h9;
    h9->next() = h5;

    h10->next() = h4;
    h4->next() = h11;
    h11->next() = h10;

    h0->twin() = h10; h10->twin() = h0;
    h3->twin() = h7; h7->twin() = h3;
    h6->twin() = h8; h8->twin() = h6;
    h9->twin() = h11; h11->twin() = h9;

    h6->vertex() = v0;
    h7->vertex() = v0;
    h8->vertex() = h2->vertex();

    h9->vertex() = v0;
    h10->vertex() = v0;
    h11->vertex() = h5->vertex();

    v0->halfedge() = h7;

    e0->halfedge() = h0;
    h10->edge() = e0;

    e1->halfedge() = h3;
    h3->edge() = e1;
    h7->edge() = e1;

    e2->halfedge() = h6;
    h6->edge() = e2;
    h8->edge() = e2;

    e3->halfedge() = h9;
    h9->edge() = e3;
    h11->edge() = e3;

    f0->halfedge() = h0;
    h6->face() = f0;

    f1->halfedge() = h3;
    h9->face() = f1;

    f2->halfedge() = h7;
    h7->face() = f2;
    h1->face() = f2;
    h8->face() = f2;

    f3->halfedge() = h10;
    h10->face() = f3;
    h4->face() = f3;
    h11->face() = f3;

    return v0;
  }

  std::vector<HalfedgeIter> halfedgesAround(HalfedgeIter start)
  {
    std::vector<HalfedgeIter> edges;
    HalfedgeIter h = start;
    do {
      edges.push_back(h);
      h = h->twin()->next();
    } while (h != start);
    return edges;
  }

  void Vertex::computeCentroid( void )
  {
    // DONE Task 6.
    // computes the average of the neighboring vertex positions and stores it in Vertex::centroid
    if (degree() == 0) {
      return;
    }
    
    centroid = Vector3D(0, 0, 0);

    int count = 0;
    for (HalfedgeIter h : halfedgesAround(halfedge())) {
      centroid += h->twin()->vertex()->position;
      count++;
    }
    centroid /= (double)count;
  }

  void computeOriginalVertexPositions(HalfedgeMesh& mesh)
  {
    // DONE Task 6.
    // This routine should compute the new vertex positions for all vertices in the mesh, using the Loop subdivision rule,
    // and store them in Vertex::newPosition. At this point, we also want to mark each vertex as being a vertex of the original mesh.
    for (VertexIter vertex = mesh.verticesBegin(); vertex != mesh.verticesEnd(); vertex++) {
      if (vertex->isBoundary()) {
        VertexIter v0 = mesh.verticesEnd(), v1 = mesh.verticesEnd();
        for (HalfedgeIter h : halfedgesAround(vertex->halfedge())) {
          if (h->isBoundary()) {
            v0 = h->twin()->vertex();
            if (v1 != mesh.verticesEnd()) {
              break;
            }
          }
          if (h->twin()->isBoundary()) {
            v1 = h->twin()->vertex();
            if (v0 != mesh.verticesEnd()) {
              break;
            }
          }
        }
        if (v0 == mesh.verticesEnd() || v1 == mesh.verticesEnd()) {
          vertex->newPosition = vertex->position;
          vertex->isNew = false;
          continue;
        }
        vertex->newPosition = (v0->position + v1->position) / 8.0 + (vertex->position * 3.0 / 4.0);
        vertex->isNew = false;
        continue;
      }

      vertex->computeCentroid();
      size_t n = vertex->degree();

      if (n < 3) {
        vertex->newPosition = vertex->position;
        vertex->isNew = false;
        continue;
      }

      double u = (n == 3) ? 3.0 / 16.0 : 3.0 / (8.0 * n);
      vertex->newPosition = lerp(vertex->position, vertex->centroid, n * u);
      vertex->isNew = false;
    }
  }

  void computeNewVertexPositions(HalfedgeMesh& mesh)
  {
    // DONE Task 6.
    // This routine should compute the new vertex positions for all edges in the mesh, using the Loop subdivision rule,
    // and store them in Edge::newPosition. At this point, we also want to mark each edge as being an edge of the original mesh.
    for (EdgeIter edge = mesh.edgesBegin(); edge != mesh.edgesEnd(); edge++) {
      if (edge->isBoundary()) {
        HalfedgeIter h = edge->halfedge();
        VertexIter v0 = h->vertex();
        VertexIter v1 = h->twin()->vertex();
        edge->newPosition = (v0->position + v1->position) / 2.0;
        edge->isNew = false;
        continue;
      }

      HalfedgeIter h0 = edge->halfedge();
      HalfedgeIter h1 = h0->twin();
      VertexIter v0 = h0->vertex();
      VertexIter v1 = h1->vertex();
      VertexIter v2 = h0->next()->next()->vertex();
      VertexIter v3 = h1->next()->next()->vertex();

      edge->newPosition = (v0->position + v1->position) * 3.0 / 8.0 + (v2->position + v3->position) / 8.0;
      edge->isNew = false;
    }
  }

  void splitAllEdges(HalfedgeMesh& mesh)
  {
    // DONE Task 6.
    // This routine should split every edge in the mesh, in any order. For future reference, we're also going to store some
    // information about which subdivided edges come from splitting an edge in the original mesh, and which edges
    // are new, by setting the flag Edge::isNew. Note that in this loop, we only want to iterate over edges of
    // the original mesh---otherwise, we'll end up splitting edges that we just split (and the loop will never end!)
    std::vector<EdgeIter> originalEdges;
    for (EdgeIter edge = mesh.edgesBegin(); edge != mesh.edgesEnd(); edge++) {
      if (!edge->isNew) {
        originalEdges.push_back(edge);
      }
    }

    for (EdgeIter edge: originalEdges) {
      if (edge->isNew) {
        continue;
      }

      VertexIter a = edge->halfedge()->vertex();
      VertexIter b = edge->halfedge()->twin()->vertex();

      VertexIter newVertex = mesh.splitEdge(edge);
      newVertex->newPosition = edge->newPosition;
      newVertex->isNew = true;

      for (HalfedgeIter h : halfedgesAround(newVertex->halfedge())) {
        VertexIter other = h->twin()->vertex();
        h->edge()->isNew = (other != a && other != b);
      }
      edge->isNew = false;
    }
  }

  void flipNewEdges(HalfedgeMesh& mesh)
  {
    // DONE Task 6.
    // This routine should flip any new edge that connects an old and new vertex.
    for (EdgeIter edge = mesh.edgesBegin(); edge != mesh.edgesEnd(); edge++) {
      if (!edge->isNew) {
        continue;
      }
      HalfedgeIter h0 = edge->halfedge();
      HalfedgeIter h1 = h0->twin();
      VertexIter v0 = h0->vertex();
      VertexIter v1 = h1->vertex();
      if (v0->isNew != v1->isNew) {
        mesh.flipEdge(edge);
      }
    }
  }

  void MeshResampler::upsample( HalfedgeMesh& mesh )
  {
    // DONE Task 6.
    // This routine should increase the number of triangles in the mesh using Loop subdivision.
    // One possible solution is to break up the method as listed below.

    // 1. Compute new positions for all the vertices in the input mesh, using the Loop subdivision rule,
    // and store them in Vertex::newPosition. At this point, we also want to mark each vertex as being
    // a vertex of the original mesh.
    computeOriginalVertexPositions(mesh);

    // 2. Compute the updated vertex positions associated with edges, and store it in Edge::newPosition.
    computeNewVertexPositions(mesh);

    // 3. Split every edge in the mesh, in any order. For future reference, we're also going to store some
    // information about which subdivided edges come from splitting an edge in the original mesh, and which edges
    // are new, by setting the flag Edge::isNew. Note that in this loop, we only want to iterate over edges of
    // the original mesh---otherwise, we'll end up splitting edges that we just split (and the loop will never end!)
    splitAllEdges(mesh);

    // 4. Flip any new edge that connects an old and new vertex.
    flipNewEdges(mesh);

    // 5. Copy the new vertex positions into final Vertex::position.
    for (VertexIter vertex = mesh.verticesBegin(); vertex != mesh.verticesEnd(); vertex++) {
      vertex->position = vertex->newPosition;
    }
  }
}
