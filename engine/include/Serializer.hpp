#pragma once
#include "types.hpp"

class Serializer {
private:
  //
public:

  static bool ToToml(  std::string const & outfile, GameObjectContainer& g_obj_cont, IconContainer& icon_cont /*, ShaderIcon & shade_cont*/);
  static bool FromToml(std::string const & infile,  GameObjectContainer& g_obj_cont, IconContainer& icon_cont /*, ShaderIcon & shade_cont*/);


  //                                                |-This sucks... c++ won't allow passing `Type const &` non-NULL refs into std::array, and other pass-by-value issues
  //                                               \/               so must use bare C-style pointer/arrays
  static bool VerifyLoad(GameObjectContainer const * g_obj_arr[2], IconContainer const * icon_arr[2] /*, ShaderIcon const & shade_cont[2]*/);
};
