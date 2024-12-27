#ifndef BVH_H
#define BVH_H

#include <algorithm>
#include <memory>

#include "aabb.h"
#include "hittable.h"
#include "hittable_list.h"
#include "vec3.h"

class bvh_node : public hittable {
    private:
        std::shared_ptr<hittable> left;
        std::shared_ptr<hittable> right;
        aabb bbox;

        static bool box_compare(
            const std::shared_ptr<hittable> a,
            const std::shared_ptr<hittable> b,
            int axis_index
        ) {
            auto a_axis_interval = a->bounding_box()[axis_index];
            auto b_axis_interval = b->bounding_box()[axis_index];
            return a_axis_interval.min < b_axis_interval.min;
        }

    public:
        bvh_node(hittable_list list)
        : bvh_node(list.objects, 0, list.objects.size()) {}

        bvh_node(
            std::vector<std::shared_ptr<hittable>>& objects,
            size_t start,
            size_t end
        ) {
            bbox = aabb::empty;
            for (const auto& object : objects) {
                bbox = aabb(bbox, object->bounding_box());
            }

            const int axis = bbox.longest_axis();

            const auto comparator = [&axis](
                const std::shared_ptr<hittable> a,
                const std::shared_ptr<hittable> b
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
                left = std::make_shared<bvh_node>(objects, start, mid);
                right = std::make_shared<bvh_node>(objects, mid, end);
            }

            bbox = aabb(left->bounding_box(), right->bounding_box());
        }

        bool hit(const ray& r, interval_d t, hit_record& rec) const override {
            if (!bbox.hit(r, t)) return false;

            const bool hit_left = left->hit(r, t, rec);
            const auto rt = interval_d{t.min, hit_left ? rec.t : t.max};
            const bool hit_right = right->hit(r, rt, rec);

            return hit_left || hit_right;
        }

        aabb bounding_box() const override { return bbox; }
};

#endif
