#pragma once

struct GameOptionsS
{
  bool mAutoBudget = true;
  bool mAutoBulldoze = true;
  bool mAutoGoto = true;
  bool mDisasters = true;
  bool mPlayMusic = true;
  bool mPlaySounds = true;
  bool mNotices = true;
  bool mAnimation = true;
};

inline GameOptionsS gameOptions;
