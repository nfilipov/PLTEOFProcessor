// c++ stuff
#include <string>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <sys/stat.h>
#include <sys/types.h>

// xdaq stuff
#include "bril/mypackage/receiver.h"
#include "bril/mypackage/Events.h"
#include "bril/mypackage/exception/Exception.h"
#include "xdata/InfoSpaceFactory.h"
#include "cgicc/Cgicc.h"
#include "cgicc/HTMLClasses.h"
#include "b2in/nub/Method.h"
#include "toolbox/task/WorkLoop.h"
#include "toolbox/task/WorkLoopFactory.h"
#include "toolbox/mem/HeapAllocator.h"
#include "toolbox/mem/MemoryPoolFactory.h"
#include "toolbox/mem/AutoReference.h"
#include "xcept/tools.h"
#include "interface/bril/BCM1FTopics.hh"
#include "toolbox/task/Guard.h"

#include "bril/mypackage/EventAnalyzer.h"

XDAQ_INSTANTIATOR_IMPL(bril::mypackage::receiver)
using namespace std;
namespace bril {
  namespace mypackage {
        
    receiver::receiver (xdaq::ApplicationStub* s)
      throw (xdaq::exception::Exception) : xdaq::Application(s), eventing::api::Member(this), 
					   m_applock(toolbox::BSem::FULL)
    {
      b2in::nub::bind(this, &bril::mypackage::receiver::onMessage);
      this->getApplicationInfoSpace()->fireItemAvailable("bus", &bus_name_);
      this->getApplicationInfoSpace()->fireItemAvailable("topic", &topic_name_);
      this->getApplicationInfoSpace()->addListener(this, "urn:xdaq-event:setDefaultValues");
      m_oldrun = 1;
      m_oldls = 999999;
    }

    void receiver::actionPerformed(xdata::Event& e)
    {
      if (e.type() == "urn:xdaq-event:setDefaultValues") {
	try {this->getEventingBus(bus_name_.value_).subscribe(topic_name_.value_);}
	catch (eventing::api::exception::Exception &e) {
	  std::cout << "Failed to subscribe - " << 
	    xcept::stdformat_exception(e) << std::endl;
	}
      }
    }

    void receiver::actionPerformed(toolbox::Event& e){}

    void receiver::onMessage(toolbox::mem::Reference *ref, xdata::Properties &plist)
      throw (b2in::nub::exception::Exception) {
      toolbox::mem::AutoReference refguard(ref); // guarantee ref released 
      // when refguard out of scope
      std::string action = plist.getProperty("urn:b2in-eventing:action"); 
      if (action == "notify" && ref!= 0) {
	std::string topic = plist.getProperty("urn:b2in-eventing:topic");
	if (topic == topic_name_.value_) {
	  interface::bril::DatumHead* thead; 
	  thead = (interface::bril::DatumHead*)(ref->getDataLocation());
	  std::string payloaddict = plist.getProperty("PAYLOAD_DICT");
	  interface::bril::CompoundDataStreamer tc(payloaddict);
	  float avgraw = 0;
	  float avg = 0;
	  tc.extract_field(&avgraw, "avgraw", thead->payloadanchor);
	  tc.extract_field(&avg, "avg", thead->payloadanchor);
	  std::cout << "avgraw = " << avgraw << std::endl;
	  std::cout << "avg = " << avg << std::endl;
	  if (m_oldrun == 1)
	    this->initializeOutputFiles(thead->runnum);
	  else if (thead->runnum > m_oldrun) {
	    publishFile.close();
	    this->initializeOutputFiles(thead->runnum);
	  }
	  if (thead->runnum > m_oldrun or thead->lsnum > m_oldls) {
	    publishFile << thead->fillnum << "," << thead->runnum << ","
			<< thead->lsnum << "," << avg << "," 
			<< avgraw;
	    publishFile << "\n" << std::flush;
	  }
	  m_oldrun = thead->runnum;
	  m_oldls = thead->lsnum;
	}
      }                
    }

    void receiver::initializeOutputFiles(unsigned runNumber) {    
      std::stringstream fname;
      string baseDir = "/nfshome0/jbueghly/receiver_test_data";
      fname << baseDir << "/publish_log_" << runNumber << ".csv";
      string fname_str = fname.str();
      publishFile.open(fname_str.c_str(), ios::out);
      fname.str("");
            
      publishFile << "fill,run,lumi,avg,avgraw";
      publishFile << "\n";
    }

    receiver::~receiver(){}

  }  //end namespace mypackage
} //end namespace bril
    
 
