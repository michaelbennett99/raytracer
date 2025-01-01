#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.h"

#include <memory>
#include <vector>

class HittableList : public Hittable {
    private:
        AABB bbox;
    public:
        std::vector<std::shared_ptr<Hittable>> objects;

        HittableList() {}
        HittableList(std::shared_ptr<Hittable> object) { add(object); }

        void clear() { objects.clear(); }
        void add(std::shared_ptr<Hittable> object) {
            objects.push_back(object);
            bbox = AABB(bbox, object->bounding_box());
        }

        bool hit(
            const ray& r,
            interval_d t,
            HitRecord& rec
        ) const override {
            HitRecord temp_rec;
            bool hit_anything = false;
            auto closest_so_far = t.max();

            for (const auto& object : objects) {
                auto interval = interval_d{t.min(), closest_so_far};
                if (object->hit(r, interval, temp_rec)) {
                    hit_anything = true;
                    closest_so_far = temp_rec.t;
                    rec = temp_rec;
                }
            }

            return hit_anything;
        }

        AABB bounding_box() const override { return bbox; }
};

#endif
