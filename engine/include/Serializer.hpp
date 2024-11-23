#pragma once
#include "types.hpp"

class Serializer {
private:
  //
public:
  enum SerializerCode {
    serializer_parsing_err = -3,
    serializer_missing_outputfiles = -2,
    serializer_missing_inputfiles = -1,
    serializer_ok = 0,
    serializer_ok_duplicates = 1,
    //serializer_ok_
  };
  static enum SerializerCode ToToml(GameObjectContainer& g_obj_cont, IconContainer& icon_cont /*, &ShaderIcon shade_cont*/);
  enum SerializerCode FromToml(void);
};
