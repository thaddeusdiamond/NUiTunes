// Author: Thaddeus Diamond (diamond@cs.yale.edu)
//
// Just trying to get acquainted with Kinect

#include "Utils.h"
#include "KinectInterp.pb.h"

#include <MMSystem.h>
#include <csignal>

void GetDepth(KinectInterpProto* storage, HANDLE depth_stream_handle);
void GetVideo(KinectInterpProto* storage, HANDLE video_stream_handle);
void GetSkeleton(KinectInterpProto* storage);

pair<int, bool> active_skeletons[NUI_SKELETON_COUNT];
pair<float, float> hands[NUI_SKELETON_COUNT][NUMBER_OF_HANDS];
int last_update_time = 0;
int last_muted_time = 0;
int last_volume_up[NUI_SKELETON_COUNT][NUMBER_OF_HANDS];
int last_volume_down[NUI_SKELETON_COUNT][NUMBER_OF_HANDS];
int moved_forward[NUI_SKELETON_COUNT][NUMBER_OF_HANDS];
int moved_backward[NUI_SKELETON_COUNT][NUMBER_OF_HANDS];
int mute_action[NUI_SKELETON_COUNT][NUMBER_OF_HANDS];
float last_hip_x[] = { 0, 0, 0, 0, 0, 0 },
      last_hip_y[] = { 0, 0, 0, 0, 0, 0 };

int main(int argc, char* argv[]) {
  // Initialize volume up/down
  for (int i = 0; i < NUI_SKELETON_COUNT; i++) {
    for (int j = 0; j < NUMBER_OF_HANDS; j++)
      last_volume_up[i][j] = last_volume_down[i][j] = moved_backward[i][j] = moved_forward[i][j] = mute_action[i][j] = 0;
  }

  // Set up the handles to be used in kinect NUI tracking
  HANDLE nui_events[4];
  nui_events[STOP_EVT] = CreateEvent(NULL, TRUE, FALSE, NULL);
  nui_events[VIDE_EVT] = CreateEvent(NULL, TRUE, FALSE, NULL);
  nui_events[DEPT_EVT] = CreateEvent(NULL, TRUE, FALSE, NULL);
  nui_events[SKEL_EVT] = CreateEvent(NULL, TRUE, FALSE, NULL);

  // Initialize and enable the Kinect and make sure it exists
  HRESULT hr;
  hr = NuiInitialize(NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX |
                     NUI_INITIALIZE_FLAG_USES_SKELETON |
                     NUI_INITIALIZE_FLAG_USES_COLOR);
  if (FAILED(hr))
    DIE(hr, "Uh oh, couldn't find your Kinect device\n");

  // Enable skeleton tracking
  hr = NuiSkeletonTrackingEnable(nui_events[SKEL_EVT], 0);
  if (FAILED(hr))
    DIE(hr, "Could not enable skeletal tracking on the system\n");

  // Get the image stream from the Kinect
  HANDLE video_stream_handle;
  hr = NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR, NUI_IMAGE_RESOLUTION_640x480,
                          0, 2, nui_events[VIDE_EVT], &video_stream_handle);
  if (FAILED(hr))
    DIE(hr, "Could not open Kinect image stream\n");

  // Get the depth stream from the Kinect
  HANDLE depth_stream_handle;
  hr = NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX,
                          NUI_IMAGE_RESOLUTION_320x240, 0, 2, 
                          nui_events[DEPT_EVT], &depth_stream_handle);
  if (FAILED(hr))
    DIE(hr, "Could not open Kinect depth stream\n");
  
  // We want to keep track of the FPS in a time var and handle SIGINT
  int last_fps_time = -1,
      total_frames = 0;
  signal(SIGINT, &Signal::StopProgram);

  // Globally modified loop variables (I know, bad comment...)
  int next_event_id = -1;
  for (int i = 0; i < NUI_SKELETON_COUNT; i++) {
    active_skeletons[i].first = 0;
    active_skeletons[i].second = false;
  }

  do {
    // Create a proto to serialize (THIS SIDE OF WIRE)
    KinectInterpProto* storage = new KinectInterpProto();
    next_event_id = WaitForMultipleObjects(
      sizeof(nui_events) / sizeof(nui_events[STOP_EVT]),
      nui_events,  // Array containing the events we're waiting on
      FALSE,       // We don't want to wait on all events, just one
      100);        // Max wait of 100ms

    switch (next_event_id) {
    case STOP_EVT:
      Signal::StopProgram(0);
      break;
    
    case VIDE_EVT:
      GetVideo(storage, video_stream_handle);
      break;

    case SKEL_EVT:
      GetSkeleton(storage);
      break;

    case DEPT_EVT:
      GetDepth(storage, depth_stream_handle);
      total_frames++;
      break;
    }

    // Serialize and pass along the way
    for (int i = 0; i < NUI_SKELETON_COUNT; i++)
      storage->add_active_skeletons(active_skeletons[i].second);
    string serialization;
    assert(storage->SerializeToString(&serialization));

    // TODO(Thad): Pass on wire

    // Create a proto to deserialize after rest (OTHER SIDE OF WIRE)
    KinectInterpProto* decompressor = new KinectInterpProto();
    assert(decompressor->ParseFromString(serialization));

    // Get the current time and calculate FPS
    int current_time = timeGetTime();
    if (last_fps_time < 0)
      last_fps_time = current_time;

    // Only show output approximately once per second
    if (current_time - last_fps_time > 1000) {
      fprintf(stderr, "The following skeleton IDs are being tracked: ");
      for (int i = 0; i < decompressor->active_skeletons_size(); i++)
        decompressor->active_skeletons(i) ? fprintf(stderr, "%d ", i) : NULL;
      
      fprintf(stderr, "\nProcessing at %5.3f FPS\n", (1000 * total_frames) /
              static_cast<double>(current_time - last_fps_time));
      last_fps_time = current_time;
      total_frames = 0;
    }

  } while (Signal::ProgramIsRunning);

  // Shut down the Kinect gracefully
  NuiShutdown();
  for (int i = 0; i < sizeof(nui_events) / sizeof(nui_events[0]); i++) {
    if (nui_events[i] && nui_events[i] != INVALID_HANDLE_VALUE)
      CloseHandle(nui_events[i]);
  }

  // Let the user see the CMD and then exit
  fprintf(stderr, "Press any key to exit...\n");
  getchar();
}

/**
 * Clear out the video back buffer so we can keep receiving events
 *
 *   @param storage             A protocol buffer for storing the image data
 *   @param video_stream_handle Handle representing the video stream of a
 *                              Kinect device
 */
void GetVideo(KinectInterpProto* storage, HANDLE video_stream_handle) {
  const NUI_IMAGE_FRAME* image_frame_ptr = NULL;

  HRESULT hr = NuiImageStreamGetNextFrame(video_stream_handle, 0,
                                          &image_frame_ptr);
  if (FAILED(hr))
    DIE(hr, "Could not get image frame from Kinect\n");

  NuiImageStreamReleaseFrame(video_stream_handle, image_frame_ptr);
}

/**
 * Clear out the depth back buffer so we can keep receiving events
 *
 *   @param storage             A protocol buffer for storing the depth data
 *   @param depth_stream_handle Handle representing the depth stream of a
 *                              Kinect device
 */
void GetDepth(KinectInterpProto* storage, HANDLE depth_stream_handle) {
  const NUI_IMAGE_FRAME* image_frame_ptr = NULL;

  HRESULT hr = NuiImageStreamGetNextFrame(depth_stream_handle, 0,
                                          &image_frame_ptr);
  if (FAILED(hr))
    DIE(hr, "Could not get depth frame from Kinect\n");

  NuiImageStreamReleaseFrame(depth_stream_handle, image_frame_ptr);
}

/**
 * Clear out the skeleton back buffer so we can keep receiving events
 *
 *   @param storage             A protocol buffer for storing the skeleton data
 */
void GetSkeleton(KinectInterpProto* storage) {
  NUI_SKELETON_FRAME skeleton_frame;
  
  HRESULT hr = NuiSkeletonGetNextFrame(0, &skeleton_frame);
  if (FAILED(hr))
    DIE(hr, "Could not get skeleton frame from Kinect\n");
  
  // Smooth out the frame
  NuiTransformSmooth(&skeleton_frame, NULL);

  for (int i = 0; i < NUI_SKELETON_COUNT; i++) {
    // This skeleton is active, let's track it
    if (skeleton_frame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED) {
      active_skeletons[i].first = timeGetTime();

      // Alert the user that we found a new skeleton
      if (!active_skeletons[i].second) {
        active_skeletons[i].second = true;
        fprintf(stderr, "Welcome, newbie! I'm gonna call you #%d!\n", i);

        bool first = true;
        for (int j = 0; j < NUI_SKELETON_COUNT; j++) {
          if (j != i && active_skeletons[j].second)
           first = false;
        }

        if (first)
          system("scripts\\Play.vbs");
      }

    // Consider a skeleton out of frame if we haven't seen them for 0.5sec
    } else if (active_skeletons[i].first &&
               active_skeletons[i].first + 500 < timeGetTime()) {
      fprintf(stderr, "Bye #%d, it was good to have you around!\n", i);
      active_skeletons[i].first = 0;
      active_skeletons[i].second = false;
      hands[i][LEFT_HAND].first = -100.0;
      hands[i][LEFT_HAND].second = -100.0;
      hands[i][RIGHT_HAND].first = -100.0;
      hands[i][RIGHT_HAND].second = -100.0;

      bool last = true;
      for (int j = 0; j < NUI_SKELETON_COUNT; j++) {
        if (j != i && active_skeletons[j].second)
          last = false;
      }

      if (last)
        system("scripts\\Pause.vbs");
    }
    
    float hip_center_x = 0, hip_center_y = 0,
          head_x = 0, head_y = 0;
    NuiTransformSkeletonToDepthImageF(
      skeleton_frame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HIP_CENTER], &hip_center_x, &hip_center_y);
    NuiTransformSkeletonToDepthImageF(
      skeleton_frame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HEAD], &head_x, &head_y);

    // Transform the skeleton to depth coordinates
    for (int j = 0; j < NUMBER_OF_HANDS; j++) {
      float hand_x = 0, hand_y = 0,
            hip_x = 0, hip_y = 0;
      NuiTransformSkeletonToDepthImageF(
        skeleton_frame.SkeletonData[i].SkeletonPositions[kinect_hand[j]], &hand_x, &hand_y);
      NuiTransformSkeletonToDepthImageF(
        skeleton_frame.SkeletonData[i].SkeletonPositions[kinect_hip[j]], &hip_x, &hip_y);

      // Don't do commands if our head is really shaky
      if (hip_center_x < RIGHT_BOUNDS && hip_center_x > LEFT_BOUNDS && sqrt(pow(hip_center_x - last_hip_x[i], 2) + pow(hip_center_y - last_hip_y[i], 2)) < MOVING) {
        // Get a mute command
        if (last_muted_time + 2000 < timeGetTime()) {
          if (abs(head_x - hand_x) < SAME_POSITION && abs(head_y - hand_y + 0.05) < SAME_POSITION)
            mute_action[i][nui_hand[j]]++;
          else
            mute_action[i][nui_hand[j]] = 0;
          if (mute_action[i][nui_hand[j]] > VOLUME_DELAY) {
            last_muted_time = timeGetTime();
            system("scripts\\Mute.vbs");
          }
        }

        // Get the right hand
        if (hands[i][nui_hand[j]].first != 0 && hands[i][nui_hand[j]].second != 0) {
          // Detect the "next" key
          if (abs(hands[i][nui_hand[j]].second - hand_y) < STABLE &&
              hands[i][nui_hand[j]].first - hand_x > SWIPE)
            moved_forward[i][nui_hand[j]]++;
          else
            moved_forward[i][nui_hand[j]] = 0;

          // Detect the "backward" motion
          if (abs(hands[i][nui_hand[j]].second - hand_y) < STABLE &&
              hand_x - hands[i][nui_hand[j]].first > SWIPE)
            moved_backward[i][nui_hand[j]]++;
          else
            moved_backward[i][nui_hand[j]] = 0;

          // Change iTunes
          if (last_update_time + 1000 < timeGetTime() && (moved_forward[i][nui_hand[j]] >= SWIPE_FRAMES || moved_backward[i][nui_hand[j]] >= SWIPE_FRAMES)) {
            last_update_time = timeGetTime();
            if (moved_forward[i][nui_hand[j]] >= SWIPE_FRAMES) {
              moved_forward[i][nui_hand[j]] = 0;
              system("scripts\\Next.vbs");
            } else if (moved_backward[i][nui_hand[j]] >= SWIPE_FRAMES) {
              moved_backward[i][nui_hand[j]] = 0;
              system("scripts\\Previous.vbs");
            }
          }
        }

        if ((nui_hand[j] == RIGHT_HAND ? hand_x - hip_x : hip_x - hand_x) > AWAY_FROM_BODY) {
          // Volume up
          if (hand_y <= head_y + HIGH_OFFSET) {
            last_volume_up[i][nui_hand[j]]++;
            if (last_volume_up[i][nui_hand[j]] >= VOLUME_DELAY && last_volume_up[i][nui_hand[j]] % 5 == 0)
              system("scripts\\VolumeUp.vbs");
          } else {
            last_volume_up[i][nui_hand[j]] = 0;
          }

          // Volume down
          if (hand_y >= head_y + LOW_OFFSET) {
            last_volume_down[i][nui_hand[j]]++;
            if (last_volume_down[i][nui_hand[j]] >= VOLUME_DELAY && last_volume_down[i][nui_hand[j]] % 5 == 0)
              system("scripts\\VolumeDown.vbs");
          } else {
            last_volume_down[i][nui_hand[j]] = 0;
          }
        
        // Reset variables
        } else {
          last_volume_up[i][nui_hand[j]] = 0;
          last_volume_down[i][nui_hand[j]] = 0;
        }
      // Moving body, reset variables
      } else {
        last_volume_up[i][nui_hand[j]] = 0;
        last_volume_down[i][nui_hand[j]] = 0;
        moved_forward[i][nui_hand[j]] = 0;
        moved_backward[i][nui_hand[j]] = 0;
      }

      // Set up the hands and head to new value
      last_hip_x[i] = hip_center_x;
      last_hip_y[i] = hip_center_y;
      hands[i][nui_hand[j]].first = hand_x;
      hands[i][nui_hand[j]].second = hand_y;
    }
  }
}