#ifndef BVH_H
#define BVH_H

#include <algorithm>
#include <memory>

#include "aabb.h"
#include "hittable.h"
#include "hittable_list.h"
#include "vec3.h"

class BVHNode : public Hittable {
    private:
        std::shared_ptr<Hittable> left {};
        std::shared_ptr<Hittable> right {};
        AABB bbox {};

        static bool box_compare(
            const std::shared_ptr<Hittable> a,
            const std::shared_ptr<Hittable> b,
            int axis_index
        ) {
            const auto a_axis_interval { a->bounding_box()[axis_index] };
            const auto b_axis_interval { b->bounding_box()[axis_index] };
            return a_axis_interval.min() < b_axis_interval.min();
        }

    public:
        BVHNode() = default;
        explicit BVHNode(HittableList list)
            : BVHNode {list.objects, 0, list.objects.size()} {}
        explicit BVHNode(
            std::vector<std::shared_ptr<Hittable>> objects,
            size_t start,
            size_t end
        ) {
            bbox = AABB::empty;
            for (const auto& object : objects) {
                bbox = AABB(bbox, object->bounding_box());
            }

            const int axis = bbox.longest_axis();

            const auto comparator = [&axis](
                const std::shared_ptr<Hittable> a,
                const std::shared_ptr<Hittable> b
            ) -> bool {
                return box_compare(a, b, axis);
            };

            const size_t object_span = end - start;

            if (object_span == 1) {
                left = right = objects[start];
            } else if (object_span == 2) {
                left = objects[start];
                right = objects[start+1];
            } else {
                std::sort(
                    std::begin(objects) + start,
                    std::begin(objects) + end,
                    comparator
                );

                const auto mid = start + object_span / 2;
                left = std::make_shared<BVHNode>(objects, start, mid);
                right = std::make_shared<BVHNode>(objects, mid, end);
            }

            bbox = AABB(left->bounding_box(), right->bounding_box());
        }

        bool hit(const Ray& r, interval_d t, HitRecord& rec) const override {
            if (!bbox.hit(r, t)) return false;

            const auto hit_left { left->hit(r, t, rec) };
            const auto rt { interval_d{t.min(), hit_left ? rec.t : t.max()} };
            const auto hit_right { right->hit(r, rt, rec) };

            return hit_left || hit_right;
        }

        AABB bounding_box() const override { return bbox; }
};

#endif
