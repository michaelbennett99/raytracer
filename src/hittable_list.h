#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.h"

#include <memory>
#include <vector>

class hittable_list : public hittable {
    private:
        aabb bbox;
    public:
        std::vector<std::shared_ptr<hittable>> objects;

        hittable_list() {}
        hittable_list(std::shared_ptr<hittable> object) { add(object); }

        void clear() { objects.clear(); }
        void add(std::shared_ptr<hittable> object) {
            objects.push_back(object);
            bbox = aabb(bbox, object->bounding_box());
        }

        bool hit(
            const ray& r,
            interval_d t,
            hit_record& rec
        ) const override {
            hit_record temp_rec;
            bool hit_anything = false;
            auto closest_so_far = t.max;

            for (const auto& object : objects) {
                auto interval = interval_d{t.min, closest_so_far};
                if (object->hit(r, interval, temp_rec)) {
                    hit_anything = true;
                    closest_so_far = temp_rec.t;
                    rec = temp_rec;
                }
            }

            return hit_anything;
        }

        aabb bounding_box() const override { return bbox; }
};

#endif
