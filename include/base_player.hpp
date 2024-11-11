#pragma once

#include "types.hpp"
#include <boost/static_assert.hpp>
#include <memory>
#include <raylib.h>
#include <type_traits>

// abstract class for binding
class BasePlayer {

public:
  virtual void start() = 0;
  virtual void update() = 0; // call once per frame

  // // optional and gets called every time the
  // engine needs to redo the physics calcs
  virtual void static_update();

  // calling this won't trigger a destructor directly but
  // it will remove this from the list of active entities,
  // so free its memory here if nescessary
  virtual void destruct();

  // pls for the love of god if you override this make sure that you know what
  // the fuck you are doing
  virtual ~BasePlayer() { destruct(); };
};

// T should be the spawning character
template <typename T> class PlayerContainer {
  BOOST_STATIC_ASSERT(
      (std::is_base_of<BasePlayer, T>::value)); // make sure we only get valid
                                                // players at compile time
                                                // hopefully
  unique_ptr<BasePlayer> Player;

public:
  PlayerContainer<T>() {
    Player = make_unique<T>();
    Player->start();
  }

  void update() { Player->update(); }

  template <typename NewPlayerType> void switchPlayerType() {

    auto t = Player.release();
    delete t;

    Player = make_unique<NewPlayerType>();
    Player->start();
  }

  ~PlayerContainer(){
      // write last type to save data via an enum
  };
};
