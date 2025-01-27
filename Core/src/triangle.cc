//
// Created by creeper on 22-11-20.
//
#include <Core/utils.h>
#include <Core/interaction.h>
#include <Core/mesh.h>
#include <Core/sampler.h>

namespace rdcraft {
static bool insideTriangle(const Vec3& p, const Vec3& a, const Vec3& b,
                           const Vec3& c) {
  Vec3 ab = b - a;
  Vec3 bc = c - b;
  Vec3 ca = a - c;
  Vec3 ap = p - a;
  Vec3 bp = p - b;
  Vec3 cp = p - c;
  Vec3 n1 = cross(ab, ap);
  Vec3 n2 = cross(bc, bp);
  Vec3 n3 = cross(ca, cp);
  return dot(n1, n2) >= 0 && dot(n2, n3) >= 0 && dot(n3, n1) >= 0;
}

void Triangle::intersect(const Ray& ray,
                         std::optional<SurfaceInteraction>& interaction) const {
  const auto& a = mesh->vertices[idx[0]];
  const auto& b = mesh->vertices[idx[1]];
  const auto& c = mesh->vertices[idx[2]];
  Vec3 ab = b - a;
  Vec3 ac = c - a;
  Vec3 p_vec = glm::cross(ray.dir, ac);
  Real det = dot(ab, p_vec);
  if (det <= 0.0)
    return static_cast<void>(interaction = std::nullopt);
  Real inv_det = 1.0 / det;
  Vec3 t_vec = ray.orig - a;
  Real u = dot(t_vec, p_vec) * inv_det;
  if (u < 0.0 || u > 1.0) return static_cast<void>(interaction = std::nullopt);
  Vec3 q_vec = glm::cross(t_vec, ab);
  Real v = dot(ray.dir, q_vec) * inv_det;
  if (v < 0.0 || u + v > 1.0)
    return static_cast<void>(interaction = std::nullopt);
  Vec3 pos = u * b + v * c + (1.0 - u - v) * a;
  Vec3 normal = u * mesh->normals[idx[1]] + v * mesh->normals[idx[2]] +
                (1.0 - u - v) * mesh->normals[idx[0]];
  Vec2 uv = u * mesh->uvs[idx[1]] + v * mesh->uvs[idx[2]] + (
              1.0 - u - v) * mesh->uvs[idx[0]];
  interaction.emplace(uv, pos, normal);
}

bool Triangle::intersect(const Ray& ray) const {
  const auto& a = mesh->vertices[idx[0]];
  const auto& b = mesh->vertices[idx[1]];
  const auto& c = mesh->vertices[idx[2]];
  Vec3 ab = b - a;
  Vec3 ac = c - a;
  Vec3 p_vec = glm::cross(ray.dir, ac);
  Real det = dot(ab, p_vec);
  if (det <= 0.0) return false;
  Real inv_det = 1.0 / det;
  Vec3 t_vec = ray.orig - a;
  Real u = dot(t_vec, p_vec) * inv_det;
  if (u < 0.0 || u > 1.0) return false;
  Vec3 q_vec = glm::cross(t_vec, ab);
  if (Real v = dot(ray.dir, q_vec) * inv_det; v < 0.0 || u + v > 1.0) return
      false;
  return true;
}

static Vec3 uniformSampleTriangle(const Vec3& a, const Vec3& b, const Vec3& c,
                                  Sampler& sampler) {
  Real u = std::sqrt(sampler.sample());
  Real v = sampler.sample();
  return (1.0 - u) * a + u * (1.0 - v) * b + u * v * c;
}

ShapeSampleRecord Triangle::sample(Sampler& sampler) const {
  Vec3 a = mesh->vertices[idx[0]];
  Vec3 b = mesh->vertices[idx[1]];
  Vec3 c = mesh->vertices[idx[2]];
  Vec3 p = uniformSampleTriangle(a, b, c, sampler);
  Real pdf = 1.0 / surfaceArea();
  return {{p, normalize(cross(b - a, c - a))}, pdf};
}
} // namespace rdcraft