#include "PHG4SpacalPrototype4Subsystem.h"

#include "PHG4SpacalPrototype4Detector.h"
#include "PHG4SpacalPrototype4SteppingAction.h"

#include <phparameter/PHParameters.h>

#include <g4main/PHG4HitContainer.h>
#include <g4main/PHG4SteppingAction.h>  // for PHG4SteppingAction

#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>    // for PHIODataNode
#include <phool/PHNode.h>          // for PHNode
#include <phool/PHNodeIterator.h>  // for PHNodeIterator
#include <phool/PHObject.h>        // for PHObject
#include <phool/getClass.h>

#include <iostream>  // for operator<<, basic_ost...
#include <sstream>

class PHG4Detector;

using namespace std;

//_______________________________________________________________________
PHG4SpacalPrototype4Subsystem::PHG4SpacalPrototype4Subsystem(const std::string& na)
  : PHG4DetectorSubsystem(na, 0)
  , detector_(nullptr)
  , steppingAction_(nullptr)
{
  InitializeParameters();
}

//_______________________________________________________________________
int PHG4SpacalPrototype4Subsystem::InitRunSubsystem(PHCompositeNode* topNode)
{
  PHNodeIterator iter(topNode);
  PHCompositeNode* dstNode = dynamic_cast<PHCompositeNode*>(iter.findFirst("PHCompositeNode", "DST"));

  if (Verbosity() > 0)
    cout
        << "PHG4SpacalPrototype4Subsystem::InitRun - use PHG4SpacalPrototype4Detector"
        << endl;
  detector_ = new PHG4SpacalPrototype4Detector(this, topNode, GetParams(), Name());

  detector_->SetActive(GetParams()->get_int_param("active"));
  detector_->SetAbsorberActive(GetParams()->get_int_param("absorberactive"));
  detector_->SuperDetector(SuperDetector());
  detector_->OverlapCheck(CheckOverlap());

  if (GetParams()->get_int_param("active"))
  {
    ostringstream nodename;
    if (SuperDetector() != "NONE")
    {
      nodename << "G4HIT_" << SuperDetector();
    }
    else
    {
      nodename << "G4HIT_" << Name();
    }
    PHG4HitContainer* cylinder_hits = findNode::getClass<PHG4HitContainer>(
        topNode, nodename.str());
    if (!cylinder_hits)
    {
      dstNode->addNode(
          new PHIODataNode<PHObject>(
              cylinder_hits = new PHG4HitContainer(nodename.str()),
              nodename.str(), "PHObject"));
    }
    cylinder_hits->AddLayer(0);
    if (GetParams()->get_int_param("absorberactive"))
    {
      nodename.str("");
      if (SuperDetector() != "NONE")
      {
        nodename << "G4HIT_ABSORBER_" << SuperDetector();
      }
      else
      {
        nodename << "G4HIT_ABSORBER_" << Name();
      }
      PHG4HitContainer* cylinder_hits =
          findNode::getClass<PHG4HitContainer>(topNode, nodename.str());
      if (!cylinder_hits)
      {
        cylinder_hits = new PHG4HitContainer(nodename.str());
        dstNode->addNode(new PHIODataNode<PHObject>(cylinder_hits, nodename.str(), "PHObject"));
      }
      cylinder_hits->AddLayer(0);
    }
    steppingAction_ = new PHG4SpacalPrototype4SteppingAction(detector_);
    steppingAction_->Verbosity(Verbosity());
  }

  return 0;
}

//_______________________________________________________________________
int PHG4SpacalPrototype4Subsystem::process_event(PHCompositeNode* topNode)
{
  // pass top node to stepping action so that it gets
  // relevant nodes needed internally
  if (steppingAction_)
  {
    steppingAction_->SetInterfacePointers(topNode);
  }
  return 0;
}

//_______________________________________________________________________
PHG4Detector*
PHG4SpacalPrototype4Subsystem::GetDetector(void) const
{
  return detector_;
}

void PHG4SpacalPrototype4Subsystem::Print(const std::string& what) const
{
  detector_->Print(what);
  cout << Name() << " Parameters: " << endl;
  if (!BeginRunExecuted())
  {
    cout << "Need to execute BeginRun() before parameter printout is meaningful" << endl;
    cout << "To do so either run one or more events or on the command line execute: " << endl;
    cout << "Fun4AllServer *se = Fun4AllServer::instance();" << endl;
    cout << "PHG4Reco *g4 = (PHG4Reco *) se->getSubsysReco(\"PHG4RECO\");" << endl;
    cout << "g4->InitRun(se->topNode());" << endl;
    cout << "PHG4SpacalPrototype4Subsystem *sys = (PHG4SpacalPrototype4Subsystem *) g4->getSubsystem(\"" << Name() << "\");" << endl;
    cout << "sys->Print()" << endl;
    return;
  }
  GetParams()->Print();
  return;
}

void PHG4SpacalPrototype4Subsystem::SetDefaultParameters()
{
  set_default_double_param("xpos", 0.);               // translation in 3D
  set_default_double_param("ypos", 0.);               // translation in 3D
  set_default_double_param("zpos", 0.);               // translation in 3D
  set_default_double_param("z_rotation_degree", 0.);  // roation in the vertical plane
  set_default_int_param("construction_verbose", 0.);  // roation in the vertical plane
  return;
}
