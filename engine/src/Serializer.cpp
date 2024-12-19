#include "../include/containers.hpp"
#include "../include/serializer.hpp"
//#include <cstddef>  // nullptr
#include <raylib.h>
#include <iostream>

#include <../include/toml.hpp>

// To help with debug tracking of what items used a image
typedef struct {
    std::string t_name;                 // Texture file/object location name
    std::vector<long int> tx_inst_arr;  // ids of all Texture{} (inside GameObject or TextureIcon) which needed to reference this texture image
} TextSerList;

/**
 * @brief Create TOML of loaded items
 *
 * @note layout:
 * The engine items exported are:
 *   - GameObject{}  --> Texture ptr
 *                   --> std::string (filepath)
 *   - TextureIcon{} --> Texture
 *                   --> std::string (filepath)
 *   - Texture{}              ... not deduplicated
 *   - std::string (filepath) ... is deduplicated
 *   - Sub-items are referenced by index of output iteration
 *
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
bool Serializer::ToToml(std::string const & outfile, GameObjectContainer& g_obj_cont, IconContainer& icon_cont /*, &ShaderIcon shade_cont*/) {
    auto doc = toml::table{};     // Final TOML document
    auto game_obj_arr = toml::array{};  // List of GameObject items, can reference texture_obj_arr[] or texture_img_arr[] item indexes
    auto texture_obj_arr = toml::array{};  // List of texture instances and metadata, can reference texture_img_arr[] item indexes
    auto icon_obj_arr = toml::array{};  // List of TextureIcon items, can reference texture_obj_arr items
    int  texture_obj_count = 0;
    auto texture_img_arr = toml::array{};  // Deduplicated list of texture resources
    int  texture_img_count = 0;

    std::vector<TextSerList> texture_list;  // Used for deduplicating textures

    {   // For each GameObject, capture lambda scope and possibly deduplicate while inserting
        auto game_obj_arr_ref   = &game_obj_arr;
        auto texture_obj_arr_ref   = &texture_obj_arr;
        auto tex_arr_ref  = &texture_list;
        auto texture_img_count_ref = &texture_img_count;
        auto texture_obj_count_ref = &texture_obj_count;
        g_obj_cont.foreach(
            [&game_obj_arr_ref, &texture_obj_arr_ref, &tex_arr_ref, &texture_img_count_ref, &texture_obj_count_ref]
            (GameObject *obj) {
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
                            elm.insert("texture_img_id", im_ind);
                            (*tex_arr_ref)[im_ind].tx_inst_arr.push_back((*texture_obj_count_ref));  // Ref of Texture{} instance (inside GameObject or TextureIcon) which needs this image resource
                            std::cout << "texture_img_id " << im_ind << ", tx_inst_arr insertion" << std::endl;
                        } else {
                            // Insert new item into list of texture resources
                            elm.insert("texture_img_id", *texture_img_count_ref);  // text_arr_ref->size()
                            tex_arr_ref->push_back({obj->texture_path, {*texture_obj_count_ref}});  // Ref of Texture{} instance (inside GameObject or TextureIcon) which needs this image resource
                            (*texture_img_count_ref)++;
                        }
                    }

                    // If associated texture instance exists, assume only exists when resource reference exists (texture_path != None)
                    if (obj->texture) {
                        // At least one Texture{} struct also has position metadata in refrence to the populated texture_list[] resource
                        // This is assumed to be a new Texture{} instance, so for now no dedup implemented on these types

                        // First, insert texture metadata ref/index into current GameObject
                        elm.insert("texture_obj_id", (*texture_obj_count_ref));

                        // Second, insert new texture metadata instance into seperate serialization section for later ref of im_ind
                        {
                            // Given Texture{} raylib.h
                            auto tex_arr = toml::table{};
                            tex_arr.insert("texture_obj_id", (*texture_obj_count_ref));   // Debug info
                            tex_arr.insert("texture_img_id", im_ind);
                            tex_arr.insert("gl_id",        obj->texture->id);  // Debug info
                            tex_arr.insert("width",        obj->texture->width);
                            tex_arr.insert("height",       obj->texture->width);
                            tex_arr.insert("mipmaps",      obj->texture->mipmaps);
                            tex_arr.insert("format",       obj->texture->format);
                            texture_obj_arr_ref->emplace_back(tex_arr);
                        }

                        // Increment last populated index, into new total count
                        (*texture_obj_count_ref)++;
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
            game_obj_arr_ref->emplace_back(elm);
        });
    }

    {   // For each TextureIcon, capture lambda scope and possibly deduplicate while inserting
        auto icon_obj_arr_ref = &icon_obj_arr;
        auto texture_img_count_ref = &texture_img_count;
        auto texture_obj_count_ref = &texture_obj_count;
        auto tex_arr_ref  = &texture_list;
        auto texture_obj_arr_ref   = &texture_obj_arr;
        icon_cont.foreach (
            [&icon_obj_arr_ref, &texture_img_count_ref, &texture_obj_count_ref, &tex_arr_ref, &texture_obj_arr_ref]
            (TextureIcon *ico) {
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
                            elm.insert("texture_img_id", im_ind);
                            (*tex_arr_ref)[im_ind].tx_inst_arr.push_back(*texture_obj_count_ref);  // Ref of Texture{} instance (inside GameObject or TextureIcon) which needs this image resource
                            std::cout << "texture_img_id " << im_ind << ", tx_inst_arr insertion" << std::endl;
                        } else {
                            // Insert new item into list of texture resources
                            elm.insert("texture_img_id", *texture_img_count_ref);  // text_arr_ref->size()
                            tex_arr_ref->push_back({ico->text, {*texture_obj_count_ref}});    // Ref of Texture{} instance (inside GameObject or TextureIcon) which needs this image resource
                            (*texture_img_count_ref)++;
                        }
                    }

                    // If associated texture instance exists, assume only exists when resource reference exists (text != None)
                    if (&ico->texture) {  // Always true as in contained
                        // At least one Texture{} struct also has position metadata in refrence to the populated texture_list[] resource
                        // This is assumed to be a new Texture{} instance, so for now no dedup implemented on these types

                        // First, insert texture metadata ref/index into current GameObject
                        elm.insert("texture_obj_id", (*texture_obj_count_ref));

                        // Second, insert new texture metadata instance into seperate serialization section for later ref of im_ind
                        {
                            // Given Texture{} raylib.h
                            auto tex_arr = toml::table{};
                            tex_arr.insert("texture_obj_id", (*texture_obj_count_ref));  // Debug info
                            tex_arr.insert("texture_img_id", im_ind);
                            tex_arr.insert("gl_id",        ico->texture.id);  // Debug info
                            tex_arr.insert("width",        ico->texture.width);
                            tex_arr.insert("height",       ico->texture.width);
                            tex_arr.insert("mipmaps",      ico->texture.mipmaps);
                            tex_arr.insert("format",       ico->texture.format);
                            texture_obj_arr_ref->emplace_back(tex_arr);
                        }

                        // Increment last populated index, into new total count
                        (*texture_obj_count_ref)++;
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
            icon_obj_arr_ref->emplace_back(elm);
        });
    }

    {   // For each possibly deduplicated resource (texture_list/texture_img_arr) insert into seralization queue now
        int count = 0;
        for (auto t : texture_list) {
            auto t_and_tids = toml::table{};
            t_and_tids.insert("texture_img_id", count);  // Debug info
            t_and_tids.insert("t_name", t.t_name);
            {
                auto tx_inst_arr = toml::array{};
                for (auto r : t.tx_inst_arr) {
                    tx_inst_arr.emplace_back(r);
                }
                t_and_tids.insert("tx_inst_arr", tx_inst_arr);
            }
            count++;
            texture_img_arr.emplace_back(t_and_tids);
        }
    }

    // Be nice and list/serialize raw external resources first before anything that might reference them
    // But the toml++ serializer doesn't respect insertion order???
    // ...
    doc.insert("game_obj_arr", game_obj_arr);
    doc.insert("icon_obj_arr", icon_obj_arr);
    doc.insert("texture_obj_arr", texture_obj_arr);
    doc.insert("texture_img_arr", texture_img_arr);

    std::ofstream fout(outfile);
    if (!fout) return false;
    fout << doc;
    fout << std::endl;
    fout.close();

    return !fout.is_open();
}

#if 0
bool Serializer::FromToml(std::string const & infile, GameObjectContainer& g_obj_cont, IconContainer& icon_cont /*, &ShaderIcon shade_cont*/) {
    // Walk inputs and append if not already found
    toml::parse_result loadfile = toml::parse_file(infile);
    if (!loadfile) return false;

    return true;
}
#endif












bool Serializer::FromToml(std::string const & infile, GameObjectContainer& g_obj_cont, IconContainer& icon_cont /*, &ShaderIcon shade_cont*/) {
    // Walk inputs and append if not already found
    toml::parse_result loadfile = toml::parse_file(infile);
    if (!loadfile) return false;
    //std::cout << toml::json_formatter{ loadfile } << std::endl;

    // Need to loop over GameObject, which can require finding a Texture, which can require finding an image
    // Keep track of used items and remove parsed, but don't prune sub-items, as items could be deduplicated
    //
    // Steps:
    // 1. Read out raw values into structs for referencing when constructing items
    // 2. Populated output argument containers based on collected item info
    //

    // Lists of temp item data read out from TOML, for later inserting into argument containers
    typedef struct {
        i64   self_id;
        u8    layer_id;
        float matrix_x;
        float matrix_y;
        float matrix_width;
        float matrix_height;
        float rotation;
        u8    collider_type;
        float collider_center_x;
        float collider_center_y;
        float collider_radius;
        i32   collider_top;
        i32   collider_left;
        i32   collider_width;
        i32   collider_height;
        std::vector<Vector2> vertecies;
        GameObject* inst_ptr;
    } ParseGameObject;
    typedef struct {
        i64 self_id;
        float scale_x;
        float scale_y;
        float scale_width;
        float scale_height;
        i32   fontsize;
        i32   border_thickness;
        i64   texture_img_id;
    } ParseIconObj;
    typedef struct {
        i64 self_id;
        std::string t_name;
        std::vector<i64> textures_using_arr;
    } ParseImageLocation;
    typedef struct {
        i64 self_id;
        i64 img_id;
        unsigned int gl_id;
        int width;
        int height;
        int mipmaps;
        int format;
        Texture* inst_ptr;
    } ParseTextureObj;
    std::vector<ParseGameObject>    toml_tmp_game_obj_list;
    std::vector<ParseIconObj>       toml_tmp_icon_obj_list;
    std::vector<ParseImageLocation> toml_tmp_img_obj_list;
    std::vector<ParseTextureObj>    toml_tmp_texture_obj_list;

    // Start finding TOML items
    toml::table doc = loadfile.table();
    {
        {   // GameObject{}
            toml::array* arr = doc.get_as<toml::array>(std::string_view("game_obj_arr"));
            if (!arr) return false;
            bool inner_ok = true;
            i64 obj_ind = 0;
            arr->for_each(
                [&obj_ind, &toml_tmp_game_obj_list, &inner_ok]
                (toml::table& nth_obj)
            {
                i64 obj_num = nth_obj["texture_obj_id"].value_or(-1);
                inner_ok &= (obj_num == obj_ind);

                std::vector<Vector2> vertecies;
                toml::array* verts = nth_obj.get_as<toml::array>(std::string_view("vertecies"));
                if (verts) {
                    verts->for_each(
                        [&vertecies]
                        (toml::table& pair)
                    {
                        vertecies.emplace_back(
                            pair["x"].value_or(NAN),
                            pair["y"].value_or(NAN)
                        );
                    });
                }

                toml_tmp_game_obj_list.emplace_back(
                    // ParseGameObject{}
                    obj_num,  // self_id
                    nth_obj["layer_id"].value_or(-1),
                    nth_obj["matrix"]["x"].value_or(NAN),
                    nth_obj["matrix"]["y"].value_or(NAN),
                    nth_obj["matrix"]["width"].value_or(NAN),
                    nth_obj["matrix"]["height"].value_or(NAN),
                    nth_obj["rotation"].value_or(NAN),
                    nth_obj["collider"]["type"].value_or((u8)~0),
                    nth_obj["collider"]["center"]["x"].value_or(NAN),
                    nth_obj["collider"]["center"]["y"].value_or(NAN),
                    nth_obj["collider"]["radius"].value_or(NAN),
                    nth_obj["collider"]["top"].value_or(-1),
                    nth_obj["collider"]["left"].value_or(-1),
                    nth_obj["collider"]["width"].value_or(-1),
                    nth_obj["collider"]["height"].value_or(-1),
                    vertecies,
                    nullptr
                );
                obj_ind++;
            });
            if (!inner_ok) return false;
        }

        {   // Icon{}
            toml::array* arr = doc.get_as<toml::array>(std::string_view("icon_obj_arr"));
            if (!arr) return false;
            bool inner_ok = true;
            i64 ico_ind = 0;
            arr->for_each(
                [&toml_tmp_icon_obj_list, &ico_ind, &inner_ok]
                (toml::table& nth_obj)
            {
                i64 ico_num = nth_obj["texture_obj_id"].value_or(-1);
                inner_ok &= (ico_num == ico_ind);

                toml_tmp_icon_obj_list.emplace_back(
                    // ParseIconObj
                    ico_num,  // self_id
                    nth_obj["scale"]["x"].value_or(NAN),
                    nth_obj["scale"]["y"].value_or(NAN),
                    nth_obj["scale"]["width"].value_or(NAN),
                    nth_obj["scale"]["height"].value_or(NAN),
                    nth_obj["fontsize"].value_or(INT32_MAX),
                    nth_obj["border_thickness"].value_or(INT32_MAX),
                    nth_obj["texture_img_id"].value_or(INT32_MAX)
                );
                ico_ind++;
            });
            if (!inner_ok) return false;
        }

        {   // Image location (as std::string)
            toml::array* arr = doc.get_as<toml::array>(std::string_view("icon_obj_arr"));
            if (!arr) return false;
            bool inner_ok = true;
            i64 img_ind = 0;
            arr->for_each(
                [&toml_tmp_img_obj_list, &img_ind, &inner_ok]
                (toml::table& nth_obj)
            {
                i64 itm_ind = nth_obj["texture_img_id"].value_or(-1);
                inner_ok &= (img_ind == itm_ind);

                std::vector<i64> texture_ids_arr;
                toml::array* verts = nth_obj.get_as<toml::array>(std::string_view("vertecies"));
                if (verts) {
                    verts->for_each(
                        [&texture_ids_arr]
                        (toml::node& id)
                    {
                        texture_ids_arr.emplace_back(id.value_or(-1));
                    });
                }

                toml_tmp_img_obj_list.emplace_back(
                    // ParseImageLocation
                    itm_ind,
                    nth_obj["t_name"].value_or(""),
                    texture_ids_arr
                );
                img_ind++;
            });
            if (!inner_ok) return false;
        }

        {   // Texture{}
            toml::array* arr = doc.get_as<toml::array>(std::string_view("texture_obj_arr"));
            if (!arr) return false;
            bool inner_ok = true;
            i64 tex_ind = 0;
            arr->for_each(
                [&toml_tmp_texture_obj_list, &tex_ind, &inner_ok]
                (toml::table& nth_obj)
            {
                i64 tex_num = nth_obj["texture_obj_id"].value_or(-1);
                inner_ok &= (tex_num == tex_ind);

                toml_tmp_texture_obj_list.emplace_back(
                    tex_num,
                    nth_obj["texture_img_id"].value_or(-1),
                    nth_obj["gl_id"].value_or((unsigned int)~0),
                    nth_obj["width"].value_or(-1),
                    nth_obj["height"].value_or(-1),
                    nth_obj["mipmaps"].value_or(-1),
                    nth_obj["format"].value_or(-1),
                    nullptr
                );
            });
            if (!inner_ok) return false;
        }
    }

    // The Texture{} contains instances of Icon{} and image location (std::string) if required, and Texture{} is placed inside TextureContainer{}
    // The GameObject{} inside GameObjectContainer{} may use by ref pre-existing Texture{} elements

#if 0
    toml_tmp_texture_obj_list.for_each(
        []
        (&texture_info)
    {
        i64 img_ind = texture_info.img_id;
        if (toml_tmp_img_obj_list.size() >= img_ind) return false;
        std::string& img_name_ref = toml_tmp_img_obj_list[img_ind];

        //TextureIcon *IconContainer::add_new(string path, i32 x, i32 y)
        Texture* tex_ptr = icon_cont.add_new(img_name_ref, )
    });
    icon_cont.
    // loop
#endif
    (void)g_obj_cont;
    (void)icon_cont;
    return true;
}









bool Serializer::VerifyLoad(GameObjectContainer const * g_obj_arr[2], IconContainer const * icon_obj_arr[2] /*, ShaderIcon const & shade_cont[2]*/) {
    (void)g_obj_arr;
    (void)icon_obj_arr;

    // 0. All ids should be >=0
    // 1. Floats should never be NAN
    // 2. Int fields width and height should be >=0
    // 3. Validate collider based on tagged enum
    // 4. Image system path locations should be len >0
    // 5. Unsigned IDs should not be ~0

    return true;
}
