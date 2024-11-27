#include "../include/containers.hpp"
#include "../include/serializer.hpp"
#include <raylib.h>
#include <iostream>

#include <../include/toml.hpp>

/**
 * @brief Private sub-helper, due to multiple objects can contain this type
 *
 * @param tx_obj
 * @return Toml::Table
 */
static auto FromGlTextureIntoToml(Texture const & tx_obj, int imid) {
    // Given Texture{} raylib.h
    // Return toml::table{} instance
    auto tex_arr = toml::table{};
    tex_arr.insert("gl_id",   tx_obj.id);  // not needed, just debug
    tex_arr.insert("texture_imid", imid);
    tex_arr.insert("width",   tx_obj.width);
    tex_arr.insert("height",  tx_obj.width);
    tex_arr.insert("mipmaps", tx_obj.mipmaps);
    tex_arr.insert("format",  tx_obj.format);
    return tex_arr;
}

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
enum Serializer::SerializerCode Serializer::ToToml(GameObjectContainer& g_obj_cont, IconContainer& icon_cont /*, &ShaderIcon shade_cont*/) {
    auto doc = toml::table{};     // Final TOML document
    auto go_arr = toml::array{};  // List of GameObject items, can reference tx_arr[] or im_arr[] item indexes
    auto tx_arr = toml::array{};  // List of texture instances and metadata, can reference im_arr[] item indexes
    int  tx_count = 0;
    auto im_arr = toml::array{};  // Deduplicated list of texture resources
    int  im_count = 0;

    typedef struct {
        std::string t_name;                   // Texture file/object location name
        std::vector<unsigned int> tid_arr;    // Texture2D .id field
    } TextSerList;
    std::vector<TextSerList> texture_list;  // Used for deduplicating textures

    {   // Lambda captures
        auto go_arr_ref   = &go_arr;
        auto tx_arr_ref   = &tx_arr;
        auto tex_arr_ref  = &texture_list;
        auto im_count_ref = &im_count;
        auto tx_count_ref = &tx_count;
        g_obj_cont.foreach ([&go_arr_ref, &tx_arr_ref, &tex_arr_ref, &im_count_ref, &tx_count_ref](GameObject *obj) {
            auto elm = toml::table{};
            {   // .matrix
                auto mat_arr = toml::array{};
                mat_arr.emplace_back(obj->matrix.x);
                mat_arr.emplace_back(obj->matrix.y);
                mat_arr.emplace_back(obj->matrix.width);
                mat_arr.emplace_back(obj->matrix.height);
                elm.insert("matrix", mat_arr);
            }
            {   // .scale
                auto sc_arr = toml::array{};
                sc_arr.emplace_back(obj->scale.x);
                sc_arr.emplace_back(obj->scale.y);
                elm.insert("scale", sc_arr);
            }
            {   // .rotation
                elm.insert("rotation", obj->rotation);
            }
            {   // .collider
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
                    default:
                    case ColliderType::NONE:
                        break;
                }
                elm.insert("collider", coll_obj);
            }
            {   // .texture_path and .texture
                // If texture path populated in engine structure
                if (obj->texture_path.size() > 0) {
                    int im_ind = 0;
                    {   // Handle inserting/deduplicating texture resources
                        // Search for item, if not contained append it
                        for (; im_ind < tex_arr_ref->size(); im_ind++) {
                            if ((*tex_arr_ref)[im_ind].t_name == obj->texture_path) {
                                break;
                            }
                        }

                        if (im_ind < tex_arr_ref->size()) {
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
                        tx_arr_ref->emplace_back(FromGlTextureIntoToml((Texture const &)obj->texture, im_ind));

                        // Increment last populated index, into new total count
                        (*tx_count_ref)++;
                    }
                }
            }
            {   // .layer_id
                elm.insert("layer_id", obj->layer_id);
            }
            {   // .ObjectListKey.next != None
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

    icon_cont.foreach ([](TextureIcon *ico) {

        printf("TextureIcon()\n");
        printf(".scale %f %f %f %f\n", ico->scale.x, ico->scale.y, ico->scale.width, ico->scale.height);
        printf(".text `%s`\n", ico->text.c_str());
        printf(".fontsize %d\n", ico->fontsize);
        printf(".texture %d\n", ico->texture.id);

        if ((ico->ref) && (ico->ref->next)) {
            printf(".next %d\n", ico->ref->data->texture.id);  // Texture{} raylib.h
        } else {
            printf(".texture NA\n");
        }

        printf(".BorderThickness %d\n", ico->BorderThickness);
    });

    //ShaderIcon const * sh =
    //for (auto shade : shade_cont) {
    //    shade_cont;
    //}

    // Be nice and list/serialize raw external resources first before anything that might reference them
    // ...
    doc.insert("im_arr", im_arr);
    doc.insert("tx_arr", tx_arr);
    doc.insert("go_arr", go_arr);
    std::cout << doc << "\n\n";

    return serializer_ok;
}

enum Serializer::SerializerCode Serializer::FromToml(void) {
    // Walk inputs and append if not already found
    return serializer_parsing_err;
}
