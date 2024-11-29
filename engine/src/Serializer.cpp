#include "../include/containers.hpp"
#include "../include/serializer.hpp"
#include <raylib.h>
#include <iostream>

#include <../include/toml.hpp>

/**
 * @brief Create TOML of loaded items
 *
 * @note layout:
 * obj = [elm1, elm2, elms...] # where each elm is structure of key-value pairs
 * etc...
 * ```toml
 * [obj]
 * elm_field_a = "value at elm1"
 * elm_field_b = "value in elm1"
 *
 * [obj]
 * elm_field_a = "value at elm2"
 * elm_field_b = "value in elm2"
 *
 * #elms...
 * ```
 * References:
 * - https://marzer.github.io/tomlplusplus/#mainpage-example-serialization
 * - https://godbolt.org/z/jbG1o4sd1
 */
void Serializer::ToToml(GameObjectContainer& g_obj_cont, IconContainer& icon_cont /*, &ShaderIcon shade_cont*/) {
    auto doc = toml::table{};     // Final TOML document
    auto go_arr = toml::array{};  // List of GameObject items, can reference tx_arr[] or im_arr[] item indexes
    auto tx_arr = toml::array{};  // List of texture instances and metadata, can reference im_arr[] item indexes
    auto ic_arr = toml::array{};  // List of TextureIcon items, can reference tx_arr items
    int  tx_count = 0;
    auto im_arr = toml::array{};  // Deduplicated list of texture resources
    int  im_count = 0;

    typedef struct {
        std::string t_name;                   // Texture file/object location name
        std::vector<unsigned int> tid_arr;    // Texture2D .id field
    } TextSerList;
    std::vector<TextSerList> texture_list;  // Used for deduplicating textures

    {   // For each GameObject, capture lambda scope and possibly deduplicate while inserting
        auto go_arr_ref   = &go_arr;
        auto tx_arr_ref   = &tx_arr;
        auto tex_arr_ref  = &texture_list;
        auto im_count_ref = &im_count;
        auto tx_count_ref = &tx_count;
        g_obj_cont.foreach ([&go_arr_ref, &tx_arr_ref, &tex_arr_ref, &im_count_ref, &tx_count_ref](GameObject *obj) {
            auto elm = toml::table{};
            {   // .matrix (Rectangle)
                auto mat_arr = toml::array{};
                mat_arr.emplace_back(obj->matrix.x);
                mat_arr.emplace_back(obj->matrix.y);
                mat_arr.emplace_back(obj->matrix.width);
                mat_arr.emplace_back(obj->matrix.height);
                elm.insert("matrix", mat_arr);
            }
            {   // .scale (Vector2)
                auto sc_arr = toml::array{};
                sc_arr.emplace_back(obj->scale.x);
                sc_arr.emplace_back(obj->scale.y);
                elm.insert("scale", sc_arr);
            }
            {   // .rotation (f32)
                elm.insert("rotation", obj->rotation);
            }
            {   // .collider (Collider)
                auto coll_obj = toml::table{};
                coll_obj.insert("type", obj->collider.type);
                switch(obj->collider.type) {
                    case ColliderType::POLYGON: {
                        auto vert_arr = toml::array{};
                        for (auto v : ((Polygon*)&obj->collider)->vertecies) {
                            auto vert = toml::table{};
                            vert.insert("x", v.x);
                            vert.insert("y", v.y);
                            vert_arr.emplace_back(vert);
                        }
                        coll_obj.insert("vertecies", vert_arr);
                        break;
                    }
                    case ColliderType::CIRCLE: {
                        coll_obj.insert("center.x", ((Circle*)&obj->collider)->center.x);
                        coll_obj.insert("center.y", ((Circle*)&obj->collider)->center.y);
                        coll_obj.insert("radius",   ((Circle*)&obj->collider)->radius);
                        break;
                    }
                    case ColliderType::RECTANGLE: {
                        coll_obj.insert("top",    ((rectangle*)&obj->collider)->top);
                        coll_obj.insert("left",   ((rectangle*)&obj->collider)->left);
                        coll_obj.insert("width",  ((rectangle*)&obj->collider)->width);
                        coll_obj.insert("height", ((rectangle*)&obj->collider)->height);
                        break;
                    }
                    case ColliderType::NONE:
                    default:
                        break;
                }
                elm.insert("collider", coll_obj);
            }
            {   // .texture_path and .texture
                // If texture path populated in engine structure
                if (obj->texture_path.size() > 0) {
                    long im_ind = 0;
                    {   // Handle inserting/deduplicating texture resources
                        // Search for item, if not contained append it
                        for (; im_ind < (long)tex_arr_ref->size(); im_ind++) {
                            if ((*tex_arr_ref)[im_ind].t_name == obj->texture_path) {
                                break;
                            }
                        }

                        if (im_ind < (long)tex_arr_ref->size()) {
                            // Reference texture resource index
                            elm.insert("texture_imid", im_ind);
                            (*tex_arr_ref)[im_ind].tid_arr.push_back(im_ind);
                        } else {
                            // Insert new item into list of texture resources
                            elm.insert("texture_imid", im_ind);  // text_arr_ref->size()
                            tex_arr_ref->push_back({obj->texture_path, {}});
                            (*im_count_ref)++;
                        }
                    }

                    // If associated texture instance exists, assume only exists when resource reference exists (texture_path != None)
                    if (obj->texture) {
                        // At least one Texture{} struct also has position metadata in refrence to the populated texture_list[] resource
                        // This is assumed to be a new Texture{} instance, so for now no dedup implemented on these types

                        // First, insert texture metadata ref/index into current GameObject
                        elm.insert("texture_txid", (*tx_count_ref));

                        // Second, insert new texture metadata instance into seperate serialization section for later ref of im_ind
                        {
                            // Given Texture{} raylib.h
                            auto tex_arr = toml::table{};
                            tex_arr.insert("texture_txid", (*tx_count_ref));   // Debug info
                            tex_arr.insert("texture_imid", im_ind);
                            tex_arr.insert("gl_id",        obj->texture->id);  // Debug info
                            tex_arr.insert("width",        obj->texture->width);
                            tex_arr.insert("height",       obj->texture->width);
                            tex_arr.insert("mipmaps",      obj->texture->mipmaps);
                            tex_arr.insert("format",       obj->texture->format);
                            tx_arr_ref->emplace_back(tex_arr);
                        }

                        // Increment last populated index, into new total count
                        (*tx_count_ref)++;
                    }
                }
            }
            {   // .layer_id (u8)
                elm.insert("layer_id", obj->layer_id);
            }
            {   // .ObjectListKey.next != None (ref to GameObject)
                if (obj->ObjectListKey) {
                    if (obj->ObjectListKey->next) {
                        if (obj->ObjectListKey->next->data) {
                            // If next object specified, then reference its TOML UID (which may be re-written during TOML injest loading)
                            GameObject const * obj_next = obj->ObjectListKey->next->data;

                            i64 uid_packed;
                            static_assert(sizeof(uid_packed) == sizeof(obj_next->UID));
                            memcpy(&uid_packed, &obj_next->UID, sizeof(obj_next->UID));

                            elm.insert("ObjectListKey.next.data.UID", uid_packed);
                        }
                    }
                }
            }
            go_arr_ref->emplace_back(elm);
        });
    }

    {   // For each TextureIcon, capture lambda scope and possibly deduplicate while inserting
        auto ic_arr_ref = &ic_arr;
        auto im_count_ref = &im_count;
        auto tx_count_ref = &tx_count;
        auto tex_arr_ref  = &texture_list;
        auto tx_arr_ref   = &tx_arr;
        icon_cont.foreach ([&ic_arr_ref, &im_count_ref, &tx_count_ref, &tex_arr_ref, &tx_arr_ref](TextureIcon *ico) {
            auto elm = toml::table{};

            {   // .scale (Rectangle)
                auto sc_arr = toml::array{};
                sc_arr.emplace_back(ico->scale.x);
                sc_arr.emplace_back(ico->scale.y);
                sc_arr.emplace_back(ico->scale.width);
                sc_arr.emplace_back(ico->scale.height);
                elm.insert("scale", sc_arr);
            }
            // handle .text later with .texture, as second to last
            {   // .fontsize (i32)
                elm.insert("fontsize", ico->fontsize);
            }
            {   // .border_thickness (i32)
                elm.insert("border_thickness", ico->border_thickness);
            }
            {   // .text and .texture
                // If texture path populated in engine structure
                if (ico->text.size() > 0) {
                    long im_ind = 0;
                    {   // Handle inserting/deduplicating texture resources
                        // Search for item, if not contained append it
                        for (; im_ind < (long)tex_arr_ref->size(); im_ind++) {
                            if ((*tex_arr_ref)[im_ind].t_name == ico->text) {
                                break;
                            }
                        }

                        if (im_ind < (long)tex_arr_ref->size()) {
                            // Reference texture resource index
                            elm.insert("texture_imid", im_ind);
                            (*tex_arr_ref)[im_ind].tid_arr.push_back(im_ind);
                        } else {
                            // Insert new item into list of texture resources
                            elm.insert("texture_imid", im_ind);  // text_arr_ref->size()
                            tex_arr_ref->push_back({ico->text, {}});
                            (*im_count_ref)++;
                        }
                    }

                    // If associated texture instance exists, assume only exists when resource reference exists (text != None)
                    if (&ico->texture) {  // Always true as in contained
                        // At least one Texture{} struct also has position metadata in refrence to the populated texture_list[] resource
                        // This is assumed to be a new Texture{} instance, so for now no dedup implemented on these types

                        // First, insert texture metadata ref/index into current GameObject
                        elm.insert("texture_txid", (*tx_count_ref));

                        // Second, insert new texture metadata instance into seperate serialization section for later ref of im_ind
                        {
                            // Given Texture{} raylib.h
                            auto tex_arr = toml::table{};
                            tex_arr.insert("texture_txid", (*tx_count_ref));  // Debug info
                            tex_arr.insert("texture_imid", im_ind);
                            tex_arr.insert("gl_id",        ico->texture.id);  // Debug info
                            tex_arr.insert("width",        ico->texture.width);
                            tex_arr.insert("height",       ico->texture.width);
                            tex_arr.insert("mipmaps",      ico->texture.mipmaps);
                            tex_arr.insert("format",       ico->texture.format);
                            tx_arr_ref->emplace_back(tex_arr);
                        }

                        // Increment last populated index, into new total count
                        (*tx_count_ref)++;
                    }
                }
            }
            // handle ._.next last
            {   // .TextureIcon.next != None (ref to TextureIcon)
                if (ico->ref) {
                    if (ico->ref->next) {
                        if (ico->ref->next->data) {
                            // If next object specified, then reference its TOML UID (which may be re-written during TOML injest loading)
                            TextureIcon const * ico_next = ico->ref->next->data;
                            // Does not have .UID like GameObject, so for (debug) info show next texture resource location contained by linked list
                            elm.insert("TextureIcon.next.data.text", ico_next->text);
                        }
                    }
                }
            }
            ic_arr_ref->emplace_back(elm);
        });
    }

    {   // For each possibly deduplicated resource (texture_list/im_arr) insert into seralization queue now
        int count = 0;
        for (auto t : texture_list) {
            auto t_and_tids = toml::table{};
            t_and_tids.insert("texture_imid", count);  // Debug info
            t_and_tids.insert("t_name", t.t_name);
            {
                auto tid_arr = toml::array{};
                for (auto r : t.tid_arr) {
                    tid_arr.emplace_back(r);
                }
                t_and_tids.insert("tid_arr", tid_arr);
            }
            count++;
            im_arr.emplace_back(t_and_tids);
        }
    }

    // Be nice and list/serialize raw external resources first before anything that might reference them
    // But the toml++ serializer doesn't respect insertion order???
    // ...
    doc.insert("go_arr", go_arr);
    doc.insert("ic_arr", ic_arr);
    doc.insert("tx_arr", tx_arr);
    doc.insert("im_arr", im_arr);
    std::cout << doc << "\n\n";
    std::ofstream fout("engine_out_state.toml");
    fout << doc;
    fout << std::endl;
    fout.close();

    return;
}

void Serializer::FromToml(void) {
    // Walk inputs and append if not already found
    return;
}
