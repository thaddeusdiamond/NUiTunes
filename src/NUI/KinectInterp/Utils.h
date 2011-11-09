// Simple util methods for Kinect app

#include <Windows.h>
#include <map>
#include <cstdio>
#include <cstdarg>
#include <string>
#include <utility>

#include "MSR_NuiApi.h"

#define SKEL_EVT 0
#define VIDE_EVT 1
#define DEPT_EVT 2
#define STOP_EVT 3

#define MOVING 0.001
#define STABLE 0.2
#define AWAY_FROM_BODY 0.15
#define SAME_POSITION 0.1
#define RIGHT_BOUNDS 0.7
#define LEFT_BOUNDS 0.3
#define HIGH_OFFSET 0.1
#define LOW_OFFSET 0.35
#define SWIPE 0.04
#define SWIPE_FRAMES 2
#define VOLUME_DELAY 25

#define NUMBER_OF_HANDS 2
#define LEFT_HAND 0
#define RIGHT_HAND 1

int nui_hand[] = {
  RIGHT_HAND,
  LEFT_HAND,
};
int kinect_hand[] = {
  NUI_SKELETON_POSITION_HAND_RIGHT,
  NUI_SKELETON_POSITION_HAND_LEFT,
};
int kinect_hip[] = {
  NUI_SKELETON_POSITION_HIP_RIGHT,
  NUI_SKELETON_POSITION_HIP_LEFT,
};


using std::string;
using std::pair;
using std::map;

static inline void DIE(HRESULT hr, const char* format, ...) {
  va_list arguments;
  va_start(arguments, format);

  fprintf(stderr, format, arguments);
  fprintf(stderr, "  [Code - 0x%08X, Facility - 0x%08X, Severity - 0x%08X]\n",
          HRESULT_CODE(hr), HRESULT_FACILITY(hr), HRESULT_SEVERITY(hr));

  fprintf(stderr, "Press any key to exit...\n");
  getchar();
  exit(hr);
}

namespace Signal {
  bool ProgramIsRunning = true;

  static inline void StopProgram(int status) {
    ProgramIsRunning = false;
  }
}