#ifndef GUARD_PLTTelescope_h
#define GUARD_PLTTelescope_h

#include "bril/mypackage/PLTPlane.h"
#include "bril/mypackage/PLTTrack.h"
#include "bril/mypackage/PLTU.h"

#include "TCanvas.h"
#include "TLine.h"
#include "TGraph.h"

#include <stdint.h>

class PLTTelescope
{
 public:
  PLTTelescope ();
  ~PLTTelescope ();

  void      AddPlane (PLTPlane*);
  int       Channel ();
  PLTPlane* Plane(size_t i);
  void      DrawTracksAndHits (std::string const);
  void      Draw2D (int const, TString const);
  size_t    NPlanes ();
  size_t    NHits ();
  size_t    NClusters ();
  size_t    NTracks ();
  PLTTrack* Track (size_t);
  int       HitPlaneBits ();
  int       NHitPlanes ();
  void      AddTrack (PLTTrack*);
  void      FillAndOrderTelescope ();


 private:
  std::vector<PLTPlane*> fPlanes;
  std::vector<PLTTrack*> fTracks;
  int fChannel;


};



#endif
