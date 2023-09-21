//
// Created by creeper on 23-1-6.
//

#ifndef RENDERCRAFT_TRANSFORM_H
#define RENDERCRAFT_TRANSFORM_H
#include <Core/core.h>
#include <utility>
#include <Core/maths.h>

namespace rdcraft {
struct Transform {
  Mat3 rot = Mat3::identity();
  Real scale = 1.0; ///< Use a real for scale so that this is a conformal map
  Vec3 translate{0.0, 0.0, 0.0};
  Transform() = default;
  Transform(const Transform &_transform) = default;
  Transform(Mat3 _rot, Real _scale, Vec3 _translate) : rot(_rot), scale(_scale), translate(std::move(_translate)) {}
  explicit Transform(Mat3 _rot) : rot(_rot) {}
  Transform inv() const { return {inverse(rot), 1.0 / scale, -inverse(rot) / scale * translate}; }
  Vec3 invTrans(const Vec3 &v) const { return inverse(rot) / scale * (v - translate); };
  Vec3 operator()(const Vec3 &v) const { return scale * rot * v + translate; }
  Ray invTrans(const Ray &ray) const {
    return {invTrans(ray.orig), invTrans(ray.dir)};
  }
  Ray operator()(const Ray &ray) const {
    return {scale * rot * ray.orig + translate, scale * rot * ray.dir + translate};
  }
  Vec3 transNormal(const Vec3 &N) const { return rot * N; }
  Vec3 invTransNormal(const Vec3 &N) const { return inverse(rot) * N; }
  Real scaleDist(Real dis) const { return scale * dis; }
  Real invScaleDist(Real dis) const { return dis / scale; }
};
}
#endif //RENDERCRAFT_TRANSFORM_H
