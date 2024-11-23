#include "../include/containers.hpp"
#include "../include/serializer.hpp"
#include <raylib.h>
#include <stdio.h>

//#include <toml++/toml.hpp>

enum Serializer::SerializerCode Serializer::ToToml(GameObjectContainer& g_obj_cont, IconContainer& icon_cont /*, &ShaderIcon shade_cont*/) {
    g_obj_cont.foreach ([](GameObject *obj) {
        printf("GameObject()\n.uid %d\n", obj->UID);
        printf(".matrix %f %f %f %f\n", obj->matrix.x, obj->matrix.y, obj->matrix.width, obj->matrix.height);  // Rectangle
        printf(".scale %f %f\n", obj->scale.x, obj->scale.y);
        printf(".rotation %f\n", obj->rotation);

        if (obj->collider.is_initialized()) {
            printf(".collider %d\n", (int)obj->collider.get().type);
        } else {
            printf(".collider NA\n");
        }

        printf(".texture_path `%s`\n", obj->texture_path.c_str());

        if (obj->texture) {
            printf(".texture %d\n", obj->texture->id);  // Texture{} raylib.h
        } else {
            printf(".texture NA\n");
        }

        printf(".LayerID %d\n", obj->LayerID);

        if (obj->ObjectListKey) {
            GameObject const * go = obj->ObjectListKey->data;
            printf(".ObjectListKey UID %d\n", go->UID);
        } else {
            printf(".ObjectListKey UID NA\n");
        }
    });

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

    return serializer_ok;
}

enum Serializer::SerializerCode Serializer::FromToml(void) {
    // Walk inputs and append if not already found
    return serializer_parsing_err;
}
