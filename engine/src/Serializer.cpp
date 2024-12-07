#include "../include/containers.hpp"
#include "../include/serializer.hpp"
#include <raylib.h>
#include <iostream>

#include <../include/toml.hpp>
#if 0
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
void Serializer::ToToml(std::string const & outfile, GameObjectContainer& g_obj_cont, IconContainer& icon_cont /*, &ShaderIcon shade_cont*/) {
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
        g_obj_cont.foreach (
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
    //std::cout << doc << "\n\n";
    std::ofstream fout(outfile);
    fout << doc;
    fout << std::endl;
    fout.close();

    return;
}
#endif
#if 0
bool Serializer::FromToml(std::string const & infile, GameObjectContainer& g_obj_cont, IconContainer& icon_cont /*, &ShaderIcon shade_cont*/) {
    // Walk inputs and append if not already found
    toml::parse_result loadfile = toml::parse_file(infile);
    if (!loadfile) return false;
    //std::cout << toml::json_formatter{ loadfile } << std::endl;

    // Need to loop over GameObject, which can require finding a Texture, which can require finding an image
    // Keep track of used items and remove parsed, but don't prune sub-items, as items could be deduplicated
    //
    // Steps:
    // 1. Create empty instances of everything found in the TOML
    // 2. Count number of times an item was referenced
    // 3. Remove and warn on unreferenced items
    // 4. Populated output argument containers based on collected item info
    //

    // Maps of counted items in TOML
    typedef struct {
        long count;
        void* ref;
    } ItemRef;
    std::map<long, ItemRef> game_object_id_to_ref;    // {id = {count, obj_ptr} , ...}  refs into toml_tmp_game_obj_list
    std::map<long, ItemRef> texture_icon_id_to_ref;   // {id = {count, obj_ptr} , ...}  refs into toml_tmp_icon_obj_list
    std::map<long, ItemRef> texture_obj_id_to_ref;    // {id = {count, obj_ptr} , ...}  refs into toml_tmp_texture_obj_list
    std::map<long, ItemRef> texture_image_id_to_ref;  // {id = {count, obj_ptr} , ...}  refs into toml_tmp_img_obj_list

    // Lists of temp item data read out from TOML, for later inserting into argument containers
    std::vector<GameObject>  toml_tmp_game_obj_list;
    std::vector<TextureIcon> toml_tmp_icon_obj_list;
    std::vector<Texture>     toml_tmp_texture_obj_list;
    std::vector<TextSerList> toml_tmp_img_obj_list;

    // Start finding TOML items
    {
        // Need to iterate leaf-dependencies first
        auto file_table = loadfile.table();
        {   // Images
            file_table.for_each(
                [&toml_tmp_img_obj_list, &texture_image_id_to_ref]
                (auto& key, auto& value) {
                if (key == "texture_img_arr") {
                    toml::array* arr = value.as_array();
                    if (arr) {
                        arr->for_each(
                            [&toml_tmp_img_obj_list, &texture_image_id_to_ref]
                            (auto& elm){
                            toml::table* tbl = elm.as_table();
                            if (tbl) {
                                // Info to be found inside this table
                                long img_id = -1;  // Assumed invalid index value
                                TextSerList img;

                                {   // Find image info
                                    tbl->for_each(
                                        [&img_id, &img]
                                        (auto& key, auto& value){
                                        //texture_img_id
                                        if (key == "texture_img_id") {
                                            if constexpr (toml::is_number<decltype(value)>) {
                                                img_id = value.value_or(-1);
                                            }
                                            if (img_id < 0) {
                                                std::cout << "WARN: FromToml texture_img_arr texture_img_id -- " << value << std::endl;
                                            }
                                        }
                                        //t_name
                                        if (key == "t_name") {
                                            img.t_name = value.value_or("");
                                            if (img.t_name.size() <= 0) {
                                                std::cout << "WARN: FromToml texture_img_arr t_name -- " << value << std::endl;
                                            } else {
                                                bool ok = false;
                                                FILE* fh = fopen(img.t_name.c_str(), "r");
                                                if (fh) {
                                                    if (fclose(fh) == 0) {
                                                        ok = true;
                                                    }
                                                }
                                                if (!ok) {
                                                    std::cout << "WARN: FromToml texture_img_arr t_name FILE_MISSING -- " << img.t_name << std::endl;
                                                }
                                            }
                                        }
                                        //tx_inst_arr
                                        if (key == "tx_inst_arr") {
                                            toml::array* id_arr = value.as_array();
                                            if (id_arr) {
                                                id_arr->for_each(
                                                    [&img]
                                                    (auto& elm){
                                                    if (elm.is_integer()) {
                                                        img.tx_inst_arr.emplace_back(elm.value_or(-1));  // long
                                                    }
                                                });
                                            }
                                            if (img.tx_inst_arr.size() <= 0) {
                                                std::cout << "WARN: FromToml texture_img_arr tx_inst_arr -- " << value << std::endl;
                                            }
                                        }
                                    });
                                }

                                // Insert into specified index, even if previous items not yet found
                                {
                                    if ((long)toml_tmp_img_obj_list.size() < img_id) {
                                        auto ind = toml_tmp_img_obj_list.begin() + img_id;
                                        toml_tmp_img_obj_list.insert(ind, img);
                                    } else if (img_id >= 0) {
                                        toml_tmp_img_obj_list[img_id] = img;
                                    } else {
                                        std::cout << "WARN: FromToml texture_img_arr UNKNOWN_ITEM -- " << tbl << std::endl;
                                    }
                                }
                            }
                        });
                    }
                }
            });
        }
        {   // Raylib Texture{}
            file_table.for_each(
                [&toml_tmp_texture_obj_list, &texture_obj_id_to_ref]
                (auto& key, auto& value) {
                if (key == "texture_obj_arr") {
                    toml::array* arr = value.as_array();
                    if (arr) {
                        arr->for_each(
                            [&toml_tmp_texture_obj_list, &texture_obj_id_to_ref]
                            (auto& elm){
                            toml::table* tbl = elm.as_table();
                            if (tbl) {
                                // Info to be found inside this table
                                long tex_id = -1;  // Assumed invalid index of self structure
                                long img_id = -1;  // Assumed invalid reference to pixel data source
                                Texture t;
                                tbl->for_each(
                                    [&t, &tex_id, &img_id]
                                    (auto& k, auto& v){
                                        if (k == "texture_obj_id") {
                                            if (v.is_integer()) {
                                                tex_id = v.value_or(-1);
                                            }
                                        }
                                        if (k == "texture_img_id") {
                                            if (v.is_integer()) {
                                                img_id = v.value_or(-1);
                                            }
                                        }
                                        if (k == "gl_id") {
                                            if (v.is_integer()) {
                                                t.id = v.value_or(0);
                                            }
                                        }
                                        if (k == "width") {
                                            if (v.is_integer()) {
                                                t.width = v.value_or(0);
                                            }
                                        }
                                        if (k == "height") {
                                            if (v.is_integer()) {
                                                t.height = v.value_or(0);
                                            }
                                        }
                                        if (k == "mipmaps") {
                                            if (v.is_integer()) {
                                                t.mipmaps = v.value_or(0);
                                            }
                                        }
                                        if (k == "format") {
                                            if (v.is_integer()) {
                                                t.format = v.value_or(0);
                                            }
                                        }
                                    });

                                {   // Insert into specified index, even if previous items not yet found
                                    if ((long)toml_tmp_texture_obj_list.size() < tex_id) {
                                        auto ind = toml_tmp_texture_obj_list.begin() + tex_id;
                                        toml_tmp_texture_obj_list.insert(ind, t);
                                    } else if (tex_id >= 0) {
                                        toml_tmp_texture_obj_list[tex_id] = t;
                                    } else {
                                        std::cout << "WARN: FromToml texture_img_arr UNKNOWN_ITEM -- " << tbl << std::endl;
                                    }
                                }
                            }
                        });
                    }
                }
            });
        }
        {   // Icons
            file_table.for_each(
                [&toml_tmp_img_obj_list, &texture_image_id_to_ref]
                (auto& key, auto& value) {
                if (key == "icon_obj_arr") {
                    //
                    std::cout << " icon_obj_arr TEST " << value << std::endl;
                }
            });
        }
        {   // GameObjects
            file_table.for_each(
                [&toml_tmp_img_obj_list, &texture_image_id_to_ref]
                (auto& key, auto& value) {
                if (key == "texture_obj_arr") {
                    //
                    std::cout << " icon_obj_arr TEST " << value << std::endl;
                }
            });
        }
#if 0
        loadfile.table().for_each(
            [&game_obj_arr, &icon_obj_arr, &texture_obj_arr, &texture_img_arr]
            (auto& key, auto& value) {
            if (key == "game_obj_arr") {
                std::cout << "GAME_OBJ_KEY ----- " << key << " " << value << std::endl;
                toml::array* arr = value.as_array();
                if (arr) {
                    arr->for_each([](auto&& elm) {
                        toml::table* tbl = elm.as_table();
                        if (tbl) {
                            tbl->for_each([](const toml::key& key, auto&& val) {
                                std::cout << "GAME_OBJ_ELM_X ----- " << key << " ==== " << val << std::endl;
                            });
                        }
                    });
                }
            } else if (key == "icon_obj_arr") {
                std::cout << "ICON_OBJ_KEY ----- " << key << " " << value << std::endl;
                //icon_obj_arr = value;
            } else if (key == "texture_obj_arr") {
                std::cout << "TEXTURE_OBJ_KEY ----- " << key << " " << value << std::endl;
                //texture_obj_arr = value;
            } else if (key == "texture_img_arr") {
                std::cout << "TEXTURE_IMG_KEY ----- " << key << " " << value << std::endl;
                //texture_img_arr = value;
            } else {
                std::cout << "OTHER_KEY_OBJ ----- " << key << " " << value << " key.typename " << typeid(key).name() << std::endl;
            }
        });
#endif
    }

    // Need to loop over TextureIcon, which can require finding a Texture, which can require finding an image
    // Keep track of used items and remove parsed, but don't prune sub-items as items could be deduplicated

    // Prune used items

    // Throw errors for remaining items

    (void)g_obj_cont;
    (void)icon_cont;
    return true;
}
#endif
bool Serializer::VerifyLoad(GameObjectContainer const * g_obj_arr[2], IconContainer const * icon_obj_arr[2] /*, ShaderIcon const & shade_cont[2]*/) {
    (void)g_obj_arr;
    (void)icon_obj_arr;
    return true;
}
