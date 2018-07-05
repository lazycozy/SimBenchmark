//
// Created by kangd on 26.04.18.
//

#include <raiSim/World_RG.hpp>

#include "AtlasContactBenchmark.hpp"
#include "raiCommon/utils/StopWatch.hpp"

rai_sim::World_RG *sim;
std::vector<rai_sim::ArticulatedSystemHandle> robots;
po::options_description desc;

void setupSimulation() {
  if(benchmark::atlas::options.gui)
    sim = new rai_sim::World_RG(800, 600, 0.5, rai_sim::NO_BACKGROUND);
  else
    sim = new rai_sim::World_RG();

  // time step
  sim->setTimeStep(benchmark::atlas::params.dt);
//  sim->setContactSolverParam(1.0, 0.7, 1.0,
//                             50,
//                             1e-7);
}

void resetWorld() {
  auto checkerboard = sim->addCheckerboard(2, 100, 100, 0.1, -1, rai_sim::GRID);

  for(int i = 0; i < benchmark::atlas::options.numRow; i++) {
    for(int j = 0; j < benchmark::atlas::options.numRow; j++) {
      auto robot = sim->addArticulatedSystem(
          benchmark::atlas::getURDFpath()
      );
//      atlas->setColor({1, 0, 0, 1});

      Eigen::VectorXd gc(robot->getGeneralizedCoordinateDim());
      Eigen::VectorXd gv(robot->getDOF());
      Eigen::VectorXd tau(robot->getDOF());
      gc.setZero();
      gc.segment<7>(0) << i * 2.5, j * 2.5, benchmark::atlas::params.H,
          benchmark::atlas::params.baseQuat[0],
          benchmark::atlas::params.baseQuat[1],
          benchmark::atlas::params.baseQuat[2],
          benchmark::atlas::params.baseQuat[3];

      robot->setState(gc, gv);
      robot->setGeneralizedForce(Eigen::VectorXd::Zero(robot->getDOF()));
      robots.push_back(robot);
    }
  }

  sim->setGravity({0, 0, benchmark::atlas::params.g});

  if(benchmark::atlas::options.gui)
    sim->cameraFollowObject(checkerboard, {1.0, 1.0, 1.0});
}

double simulationLoop(bool timer = true, bool cntNumContact = true) {
  // resever error vector
  benchmark::atlas::data.setN(unsigned(benchmark::atlas::params.T / benchmark::atlas::params.dt));

  // timer start
  StopWatch watch;
  if(timer)
    watch.start();

  if(benchmark::atlas::options.gui) {
    // gui
    while(sim->visualizerLoop()) {
      sim->integrate();
      if(cntNumContact) benchmark::atlas::data.numContactList.push_back(sim->getContactProblem().size());
    }
  } else {
    // no gui
    for(int t = 0; t < (int) (benchmark::atlas::params.T / benchmark::atlas::params.dt); t++) {
      sim->integrate();
      if(cntNumContact) benchmark::atlas::data.numContactList.push_back(sim->getContactProblem().size());
    }
  }
  double time = 0;
  if(timer)
    time = watch.measure();
  return time;
}

int main(int argc, const char* argv[]) {

  benchmark::atlas::addDescToOption(desc);
  benchmark::atlas::getOptionsFromArg(argc, argv, desc);

  benchmark::atlas::getParamsFromYAML(benchmark::atlas::getYamlpath().c_str(),
                                      benchmark::RAI);

  RAIINFO(
      std::endl << "=======================" << std::endl
                << "Simulator: RAI" << std::endl
                << "GUI      : " << benchmark::atlas::options.gui << std::endl
                << "Row      : " << benchmark::atlas::options.numRow << std::endl
                << "-----------------------"
  )

  // trial1: get Error
  setupSimulation();
  resetWorld();
  simulationLoop(false, true);
  double avgNumContacts = benchmark::atlas::data.computeAvgNumContact();

  // reset
  delete sim;

  // trial2: get CPU time
  setupSimulation();
  resetWorld();
  double time = simulationLoop(true, false);

  // print to screen
  std::cout<<"time taken for "
           << (int) (benchmark::atlas::params.T / benchmark::atlas::params.dt)
           << " steps "<< time <<"s \n";

  if(benchmark::atlas::options.csv)
    benchmark::atlas::printCSV(benchmark::atlas::getCSVpath(),
                               "RAI",
                               "RAI",
                               "RAI",
                               "RAI",
                               benchmark::atlas::options.numRow,
                               avgNumContacts,
                               time);


  delete sim;
  return 0;
}